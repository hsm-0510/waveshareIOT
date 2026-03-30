#include "tcp_server.h"
#include "../extended_io/digital_inputs.h"

// Digital Inputs Key/Value Pairs
keyValue inputArr[] = {
    {"entranceLB_status", 0},
    {"exitLB_status", 0},
    {"irSens01_entranceLB", 0},
    {"irSens02_entranceLB", 0},
    {"irSens03_entranceLB", 0},
    {"irSens01_exitLB", 0},
    {"irSens02_exitLB", 0},
    {"irSens03_exitLB", 0},
};
// Digital Outputs Key/Value Pairs
keyValue outputArr[] = {
    {"control_entranceLB", 0},
    {"control_exitLB", 0},
};
// System Statuses Key/Value Pairs
keyValue statusArr[] = {
    {"vehicle_alignment_status", 0},
    {"driver_absence_status", 0},
    {"entrance_weight", 0},
    {"exit_weight", 0},
    {"entrance_weight_status", 0},
    {"exit_weight_status", 0},
    {"rfid_scan_status", 0},
    {"entrance_print_status", 0},
    {"exit_print_status", 0}
};

// Json Docs
StaticJsonDocument<256> send;
StaticJsonDocument<256> recv;

// Json doc send function
void tcpSend(EthernetClient &client, int size)
{
    send.clear();
    JsonObject inputs = send.createNestedObject("inputs");
    for(int i = 0; i < size; i++)
    {
        inputs[inputArr[i].key] = inputArr[i].value;
    }
    serializeJson(send, client);
    client.println();
}

// Json doc recieve function
void tcpRecv(String jsonStr)
{
    recv.clear();

    DeserializationError err = deserializeJson(recv, jsonStr);
    if (err) {
        return; // invalid JSON → ignore
    }

    // ================= OUTPUT ARRAY UPDATE =================
    if (recv.containsKey("outputs")) {
        JsonObject outputs = recv["outputs"];

        for (int i = 0; i < 2; i++) {
            const char* key = outputArr[i].key;

            if (outputs.containsKey(key)) {
                outputArr[i].value = outputs[key];
            }
        }
    }

    // ================= STATUS ARRAY UPDATE =================
    if (recv.containsKey("status")) {
        JsonObject status = recv["status"];

        for (int i = 0; i < 9; i++) {
            const char* key = statusArr[i].key;

            if (status.containsKey(key)) {
                statusArr[i].value = status[key];
            }
        }
    }
}

// JSON Receive Function:
String jsonRecvBuffer(EthernetClient client, String jsonBuffer)
{
    Serial.println("Entering While Loop");
    char c;
    while (client.available() && c != '\n')
    {
        Serial.println("Client is Available!");
        Serial.println("Reading Data");
        char c = client.read();
        Serial.print("Read Some Data: ");
        Serial.println(c);

        if (c == '\n')
        {
            tcpRecv(jsonBuffer.c_str());
            jsonBuffer = "";
            Serial.println("Read Some jsonBuffer");
        }
        else
        {
            Serial.println("In Else Section");
            jsonBuffer += c;
        }
    }
    return " ";
}

// Functions gets the current value of the key in argument
int get_key_value(const char* search, keyValue array[], int size)
{
    for(int i = 0; i < size; i++)
    {
        if (strcmp(array[i].key, search) == 0)
        {
            return array[i].value;
        }
    }
    return -1; // Not Found
}

// Function sets the value of the key from the argument
void set_key_value(const char* setKey, int setValue, keyValue array[], int size)
{
    for(int i = 0; i < size; i++)
    {
        if (strcmp(array[i].key, setKey) == 0)
        {
            array[i].value = setValue;
        }
    }
}