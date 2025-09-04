#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

// Define the I2C address of the MLX90614
#define MLX90614_I2C_ADDR 0x5A
// Define temperature registers
#define MLX90614_TA 0x06
#define MLX90614_TOBJ1 0x07

// Function to simulate i2c_smbus_read_word_data using ioctl
int i2c_smbus_read_word_data(int file, uint8_t command) {
    union i2c_smbus_data {
        uint8_t  byte;
        uint16_t word;
        uint8_t  block[32];
    } data;
    
    struct i2c_smbus_ioctl_data {
        char read_write;
        uint8_t command;
        int size;
        union i2c_smbus_data *data;
    } args;

    args.read_write = I2C_SMBUS_READ;
    args.command = command;
    args.size = I2C_SMBUS_WORD_DATA;
    args.data = &data;

    if (ioctl(file, I2C_SMBUS, &args) < 0) {
        std::cerr << "SMBus read word data failed" << std::endl;
        return -1;
    }

    // Return the data in correct format
    return data.word;
}

// Function to convert raw temperature to Celsius
double read_temperature(int fd, int reg) {
    int raw = i2c_smbus_read_word_data(fd, reg);
    if (raw < 0) {
        return -273.15;  // Return a default value on error
    }

    // Convert to Celsius
    return (raw * 0.02) - 273.15;
}

int main() {
    const char *i2c_device = "/dev/i2c-1";  // I2C bus on Raspberry Pi
    int fd = open(i2c_device, O_RDWR);
    
    if (fd < 0) {
        std::cerr << "Failed to open I2C bus." << std::endl;
        return -1;
    }

    // Connect to the MLX90614 sensor
    if (ioctl(fd, I2C_SLAVE, MLX90614_I2C_ADDR) < 0) {
        std::cerr << "Failed to connect to the sensor." << std::endl;
        close(fd);
        return -1;
    }

    // Continuously read temperature values
    while (true) {
        double ambient_temp = read_temperature(fd, MLX90614_TA);  // Ambient temperature
        double object_temp = read_temperature(fd, MLX90614_TOBJ1);  // Object temperature

        std::cout << "Ambient Temperature: " << ambient_temp << " °C" << std::endl;
        std::cout << "Object Temperature: " << object_temp << " °C" << std::endl;

        usleep(1000000);  // Wait for 1 second before reading again
    }

    close(fd);
    return 0;
}
