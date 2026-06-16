#include "tcp_server.h"
#include "../extended_io/digital_inputs.h"

// Digital Inputs Key/Value Pairs
// keyValue inputArr[] = {
//     {"entranceLB_status", 0},
//     {"exitLB_status", 0},
//     {"irSens_01_entranceLB", 0},
//     {"irSens_02_entranceLB", 0},
//     {"irSens_03_entranceLB", 0},
//     {"irSens_01_exitLB", 0},
//     {"irSens_02_exitLB", 0},
//     {"irSens_03_exitLB", 0},
// };
keyValue inputArr[] = {
    {"irSens_01_02_entranceLB", 0},
    {"irSens_03_entranceLB", 0},
    {"irSens_01_02_exitLB", 0},
    {"irSens_03_exitLB", 0},
    {"entranceLB_open_status", 0},
    {"entranceLB_close_status", 0},
    {"exitLB_open_status", 0},
    {"exitLB_close_status", 0}
};
// Digital Outputs Key/Value Pairs
keyValue outputArr[] = {
    {"open_entranceLB", 0},
    {"close_entranceLB", 0},
    {"open_exitLB", 0},
    {"close_exitLB", 0},
    {"channel_5", 0},
    {"channel_6", 0},
    {"channel_7", 0},
    {"channel_8", 0}
};
// System Statuses Key/Value Pairs
keyValue statusArr[] = {
    {"vehicle_alignment_status_camera_ent", 0},
    {"driver_absence_status_camera_ent", 0},
    {"vehicle_alignment_status_camera_ext", 0},
    {"driver_absence_status_camera_ext", 0},
    {"gross_weight_entranceWB1", 0},
    {"gross_weight_exitWB2", 0},
    {"weight_capture_entranceControl1", 0},
    {"weight_capture_exitControl2", 0},
    {"scan_status_rfid1", 0},
    {"scan_status_rfid2", 0},
    {"kiosk_print_control_entrance", 0},
    {"kiosk_print_control_exit", 0},
    {"kiosk_button_entrance", 0},
    {"kiosk_button_exit", 0}
};

// Json Docs
StaticJsonDocument<256> send;
StaticJsonDocument<256> recv;

// Json doc send function
void tcpSend(EthernetClient &client)
{
    static String lastSentPayload = "";
    send.clear();
    const int size = 8;
    JsonObject inputs = send.createNestedObject("inputs");
    
    for(int i = 0; i < size; i++)
    {
        inputs[inputArr[i].key] = inputArr[i].value;
    }
    
    String currentPayload;
    serializeJson(send, currentPayload);

    if (currentPayload != lastSentPayload)
    {
        client.println(currentPayload);
        lastSentPayload = currentPayload;
    }
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

        int size = 8;

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

        int size = 14;

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
    int size = 8;

    for(int i = 0; i < size; i++)
    {
        if (strcmp(array[i].key, setKey) == 0)
        {
            array[i].value = setValue;
        }
    }
}