#include "digital_inputs.h"

// Pin Configuration of Digital Inputs DI1 till DI8
int pinConfig[] = {
    4,  // DI1
    5,  // DI2
    6,  // DI3
    7,  // DI4
    8,  // DI5
    9,  // DI6
    10, // DI7
    11  // DI8
};

// Function to initialize the digital inputs
void initialize_inputs()
{
    for(int i = 0; i < sizeof(pinConfig); i++)
    {
        pinMode(pinConfig[i], INPUT);
    }
}

// Function to update the digital input values in TCP (key-value pair)
void update_inputs(int size)
{
    for(int i = 0; i < size; i++)
    {
        set_key_value(
            inputArr[i].key,
            digitalRead(pinConfig[i]),
            inputArr,
            8
        );
    }
}