#include "UART.h"
#include <iostream>
#include <unistd.h> // For close()
#include <fcntl.h>  // For O_RDWR
#include <termios.h> // For UART settings

UARTComm::UARTComm(const std::string& port, int baudRate) : uart_fd(-1) {
    openConnection(port, baudRate);
}

UARTComm::~UARTComm() {
    closeConnection();
}

void UARTComm::openConnection(const std::string& port, int baudRate) {
    uart_fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart_fd == -1) {
        std::cerr << "Failed to open UART port: " << port << std::endl;
        return;
    }
    configurePort(baudRate);
}

void UARTComm::configurePort(int baudRate) {
    struct termios options;
    tcgetattr(uart_fd, &options);
    options.c_cflag = baudRate | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart_fd, TCIFLUSH);
    tcsetattr(uart_fd, TCSANOW, &options);
}

void UARTComm::closeConnection() {
    if (uart_fd != -1) {
        close(uart_fd);
    }
}

std::vector<uint8_t> UARTComm::createMessage(uint8_t messageType, const std::vector<uint8_t>& payload) {
    std::vector<uint8_t> message;
    message.push_back(0x02); // Start Byte
    message.push_back(messageType);
    message.push_back(payload.size());
    message.insert(message.end(), payload.begin(), payload.end());

    uint8_t checksum = messageType ^ payload.size();
    for (const auto& byte : payload) {
        checksum ^= byte;
    }
    message.push_back(checksum);
    message.push_back(0x03); // End Byte

    return message;
}

bool UARTComm::writeData(const std::vector<uint8_t>& data) {
    if (uart_fd == -1) return false;
    return write(uart_fd, data.data(), data.size()) == data.size();
}

bool UARTComm::readData(std::vector<uint8_t>& buffer) {
    uint8_t byte;
    while (read(uart_fd, &byte, 1) > 0) {
        buffer.push_back(byte);
        if (byte == 0x03) { // End byte
            return true;
        }
    }
    return false;
}

bool UARTComm::sendPing() {
    std::vector<uint8_t> message = createMessage(0x01, {});
    return writeData(message);
}

bool UARTComm::sendCommand(const std::vector<uint8_t>& command) {
    std::vector<uint8_t> message = createMessage(0x02, command);
    return writeData(message);
}

bool UARTComm::readResponse(std::vector<uint8_t>& response) {
    return readData(response);
}
