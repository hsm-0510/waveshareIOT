#pragma once
#include "SPI.h"
#include <Ethernet.h>
//#include <EthernetServer.h>

#define SCK       15
#define CS        16
#define MOSI      13
#define MISO      14
#define SPI_FREQ  32000000

class EthernetServerCompat : public EthernetServer {
public:
    explicit EthernetServerCompat(uint16_t port) : EthernetServer(port) {}

    void begin(uint16_t port = 0) override {
        (void)port;           // constructor already set the port
        EthernetServer::begin();
    }
};

//extern variables
extern EthernetServerCompat server;
extern byte mac[];
extern IPAddress ip;

// Function declarations
void initiateEthernet();
void initiateServer();