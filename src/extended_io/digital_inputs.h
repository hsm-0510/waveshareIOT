#include <Arduino.h>
#include "../tcp_server/tcp_server.h"

// digital inputs pin congigurations
extern int pinConfig[8];

// Function declarations
void initialize_inputs();
void update_inputs();