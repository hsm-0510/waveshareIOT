#include <Arduino.h>
#include "extended_io/i2c_driver.h"
#include "extended_io/tca9554.h"
#include "host_interface\w5500_eth.h"

void setup() {
  // Initialize I2C
  I2C_Init();
  // Set all TCA95544 IOs to Output Mode
  Mode_EXIOS(0x00);

  //Ethernet Setup
  Serial.begin(9600);
  Serial.println("\n=== W5500 Ethernet Test ===");
  initiateEthernet();
  initiateServer();
}

void loop() {
  // put your main code here, to run repeatedly:
  // Operating Relay Contacts:
  // Set_EXIO(EXIO_PIN1, High);
  delay(1000);
  //Set_Toggle(EXIO_PIN1);
  Serial.println("Trying to Connect...");
}