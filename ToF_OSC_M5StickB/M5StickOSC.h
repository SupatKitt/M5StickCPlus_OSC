#include <WiFiUdp.h>
#include <OSCMessage.h> // Adrien Freed

WiFiUDP _udp;

const char *_udpAddress;
int _udpPort;

template <typename T>

void sendOscMessage(const char *_address, T _message)
{
    OSCMessage _oscMsg(_address); // First argument is OSC address
    _oscMsg.add((T)_message);     // Then append the data
    _udp.beginPacket(_udpAddress, _udpPort);
    _oscMsg.send(_udp); // send the bytes to the SLIP stream
    _udp.endPacket();       // mark the end of the OSC Packet
    _oscMsg.empty();   // free space occupied by message
}

void sendTo(const char *_address, int _port)
{
    _udpAddress = _address;
    _udpPort = _port;

    M5.Lcd.println("Sending to:");
    M5.Lcd.println("IP: " + String(_udpAddress));
    M5.Lcd.printf("Port: %i", _udpPort);
}