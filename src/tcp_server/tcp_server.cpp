#include "tcp_server.h"
#include "../extended_io/digital_inputs.h"

// Digital Inputs Key/Value Pairs
keyValue inputArr[] = {
    {"entranceLB_status", 0},
    {"exitLB_status", 0},
    {"irSens_01_entranceLB", 0},
    {"irSens_02_entranceLB", 0},
    {"irSens_03_entranceLB", 0},
    {"irSens_01_exitLB", 0},
    {"irSens_02_exitLB", 0},
    {"irSens_03_exitLB", 0},
};
// Digital Outputs Key/Value Pairs
keyValue outputArr[] = {
    {"control_entranceLB", 0},
    {"control_exitLB", 0},
    {"channel_3", 0},
    {"channel_4", 0},
    {"channel_5", 0},
    {"channel_6", 0},
    {"channel_7", 0},
    {"channel_8", 0}
};
// System Statuses Key/Value Pairs
keyValue statusArr[] = {
    {"vehicle_alignment_status", 0},
    {"driver_absence_status", 0},
    {"gross_weight_WB1", 0},
    {"gross_weight_WB2", 0},
    {"weight_capture_status1", 0},
    {"weight_capture_status2", 0},
    {"scan_status_rfid1", 0},
    {"scan_status_rfid2", 0},
    {"print_status_entrance", 0},
    {"print_status_exit", 0},
    {"kiosk_button_entrance", 0},
    {"kiosk_button_exit", 0}
};

// Json Docs
StaticJsonDocument<256> send;
StaticJsonDocument<256> recv;

// Json doc send function
void tcpSend(EthernetClient &client)
{
    send.clear();
    size_t size = sizeof(inputArr) / sizeof(inputArr[0]);
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

        size_t size = sizeof(outputArr) / sizeof(outputArr[0]);

        for (int i = 0; i < size; i++) {
            const char* key = outputArr[i].key;

            if (outputs.containsKey(key)) {
                outputArr[i].value = outputs[key];
            }
        }
    }

    // ================= STATUS ARRAY UPDATE =================
    if (recv.containsKey("status")) {
        JsonObject status = recv["status"];

        size_t size = sizeof(statusArr) /sizeof(statusArr[0]);

        for (int i = 0; i < size; i++) {
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
int get_key_value(const char* search, keyValue array[])
{
    size_t size = sizeof(array) / sizeof(array[0]);

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
void set_key_value(const char* setKey, int setValue, keyValue array[])
{
    size_t size = sizeof(array) / sizeof(array[0]);

    for(int i = 0; i < size; i++)
    {
        if (strcmp(array[i].key, setKey) == 0)
        {
            array[i].value = setValue;
        }
    }
}