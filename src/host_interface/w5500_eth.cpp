#include "w5500_eth.h"

EthernetServerCompat server(80);

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

IPAddress ip(192, 168, 10, 60);

void initiateEthernet()
{
    SPI.begin(SCK, MISO, MOSI, CS);
    SPI.setFrequency(SPI_FREQ);
    Ethernet.init(CS);
    delay(1000);
    Ethernet.begin(mac, ip);
    delay(500);
    
    // No Hardware Present
    if (Ethernet.hardwareStatus() == EthernetNoHardware)
    {
        Serial.println("W5500 not found. Check wiring/pins.");
    }

    // No Ethernet Cable
    if (Ethernet.linkStatus() == 2)
    {
        Serial.println("No Ethernet cable");
    }
}

void initiateServer()
{
    server.begin();
    Serial.print("MOSI: ");
    Serial.println(MOSI);
    Serial.print("MISO: ");
    Serial.println(MISO);
    Serial.print("CLK: ");
    Serial.println(SCK);
    Serial.print("CS: ");
    Serial.println(CS);

    Serial.print("Server IP Address: ");
    Serial.println(Ethernet.localIP());
}