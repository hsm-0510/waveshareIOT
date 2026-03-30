#include <Arduino.h>
#include "extended_io/i2c_driver.h"
#include "extended_io/tca9554.h"
#include "host_interface/w5500_eth.h"
#include "tcp_server/tcp_server.h"
#include "extended_io/digital_inputs.h"

//Client object
EthernetClient client;

//Buffer for incoming JSON
String jsonBuffer = "";

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize I2C
  I2C_Init();
  // Set all TCA95544 IOs to Output Mode
  Mode_EXIOS(0x00);
  Set_EXIOS(0x00);

  // Initialize all Inputs
  initialize_inputs();
  
  // Establish TCP Server
  Serial.println("\n=== TCP SERVER START ===");
  initiateEthernet();
  initiateServer();
}

void loop() {
  // put your main code here, to run repeatedly:

  // Wait for Client
  EthernetClient newClient = server.available();

  if (newClient)
  {
    client = newClient;
    Serial.println("Client Connected");

    while (client.connected())
    {
      // Receiving JSON Buffer
      Serial.println("Receiving JSON");
      jsonRecvBuffer(client, jsonBuffer);
      Serial.println("Received JSON!");

      // Updating Outputs and Statuses, 2 output & 9 status updates
      //Serial.println("Updating Arrays");
      //tcpRecv(jsonBuffer);
      //Serial.println("Updated Arrays");
      Serial.println("Status Array: ");
      for(int i = 0; i < 9; i++)
      {
        Serial.print(statusArr[i].value);
        Serial.print(" ");
      }
      Serial.print("\n");
      //delay(1000);

      // Update Outputs, 2 relay outputs control
      Serial.println("Updating Outputs");
      update_outputs(2);
      Serial.println("Updated Outputs: ");
      for(int i = 0; i < 2; i++)
      {
        Serial.print(outputArr[i].value);
        Serial.print(" ");
      }
      Serial.print("\n");
      //delay(1000);

      // Update Inputs, 8 digital input statuses
      Serial.println("Updating Inputs");
      update_inputs(8);
      Serial.println("Updated Inputs: ");
      for(int i = 0; i < 8; i++)
      {
        Serial.print(inputArr[i].value);
        Serial.print(" ");
      }
      Serial.print("\n");
      //delay(1000);

      // Send Json Doc, 8 digital input statuses
      Serial.println("Sending JSON Doc");
      tcpSend(client, 8);
      Serial.println("Sent JSON Doc");

      //Delay
      Serial.println("========================================");
      delay(500);
    }
  }

  // Operating Relay Contacts:
  // Set_EXIO(EXIO_PIN1, High);
  delay(1000);
  //Set_Toggle(EXIO_PIN1);
  Serial.println("Trying to Connect...");
}