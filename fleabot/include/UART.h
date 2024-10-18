#ifndef UARTCOMM_H
#define UARTCOMM_H

#include <string>
#include <vector>

class UARTComm {
public:
    UARTComm(const std::string& port, int baudRate);
    ~UARTComm();

    bool sendPing();
    bool sendCommand(const std::vector<uint8_t>& command);
    bool readResponse(std::vector<uint8_t>& response);

private:
    int uart_fd;

    void openConnection(const std::string& port, int baudRate);
    void configurePort(int baudRate);
    void closeConnection();

    std::vector<uint8_t> createMessage(uint8_t messageType, const std::vector<uint8_t>& payload);
    bool writeData(const std::vector<uint8_t>& data);
    bool readData(std::vector<uint8_t>& buffer);
};

#endif // UARTCOMM_H
