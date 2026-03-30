#pragma once

#include <ArduinoJson.h>
#include <iostream>
#include "../host_interface/w5500_eth.h"

using namespace std;

// TCP Server Data Struct
struct keyValue{
    const char* key;
    int value;
};

// Digital Inputs Key/Value Pairs
extern keyValue inputArr[8];
// Digital Outputs Key/Value Pairs
extern keyValue outputArr[2];
// System Statuses Key/Value Pairs
extern keyValue statusArr[12];


// Json docs
extern StaticJsonDocument<256> send;
extern StaticJsonDocument<256> recv;

// Function declarations
void tcpSend(EthernetClient &client);
void tcpRecv(String jsonStr);
int get_key_value(const char* search, keyValue array[]);
void set_key_value(const char* setKey, int setValue, keyValue array[]);
String jsonRecvBuffer(EthernetClient client, String jsonBuffer);