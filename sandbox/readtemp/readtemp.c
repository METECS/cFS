#include "i2cdriver.h"
#include <stdio.h>
#include <stdlib.h>

#define MCP9808_BUS              3    // Connected to /dev/i2c-3
#define MCP9808_ADDR             0x18 // MCP9808 slave address
#define MCP9808_REG_AMBIENT_TEMP 0x05 // MCP9808 register for ambient temperature

/**
 * @brief Reads and returns the current temperature from the MCP9808
 *
 * @param i2c_fd I2C bus file descriptor
 *
 * @return the current temperature in Fahrenheit
 */
float getTemp(int i2c_fd) {
  uint8_t rx_buffer[2] = {0};
  int raw_value = 0;
  float converted_value = 0.0f;

  // Set the slave address:
  if (I2C_setSlaveAddress(i2c_fd, MCP9808_ADDR) < 0) {
    printf("*Could set slave address to %d\n", MCP9808_ADDR);
    exit(0);
  }

  // Read the 2 bytes of data:
  I2C_readTransaction(i2c_fd, MCP9808_REG_AMBIENT_TEMP, (void*) rx_buffer, 2);

  // Process the raw temperature value
  raw_value = rx_buffer[0] << 8 | rx_buffer[1];
  raw_value &= 0x0fff;

  converted_value = raw_value * 0.0625;
  converted_value = (converted_value * 9.0f / 5.0f) + 32;

  return converted_value;
}

int main() {
  int i2c_fd;
  float temp;

  // Open the I2C device file:
  i2c_fd = I2C_open(MCP9808_BUS);
  if (i2c_fd < 0) {
    printf("*Could not open I2C bus %d\n", MCP9808_BUS);
    exit(0);
  }

  // Read and print the current temp
  temp = getTemp(i2c_fd);
  printf("Temp : %5.2fF\n", temp);

  // Close the I2C file descriptor:
  I2C_close(i2c_fd);
  return 0;
}
