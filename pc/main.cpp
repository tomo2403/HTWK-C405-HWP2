#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <cstdint>
#include <vector>

#include "../lib/encoder.hpp"
#include <bitset>

int main() {
    // Öffne den seriellen Port
    const char* portname = "/dev/ttyUSB0"; // Ersetze dies durch deinen Port
    int serial_port = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);

    if (serial_port < 0) {
        std::cerr << "Fehler beim Öffnen des seriellen Ports!" << std::endl;
        return 1;
    }

    // Konfiguration des seriellen Ports
    struct termios tty;
    memset(&tty, 0, sizeof(tty));

    // Hole die aktuellen Einstellungen
    if (tcgetattr(serial_port, &tty) != 0) {
        std::cerr << "Fehler beim Abrufen der Port-Einstellungen!" << std::endl;
        return 1;
    }

    // Setze die Baudrate, Datenbits, Parität und Stopbits
    cfsetispeed(&tty, B57600); // Eingangsbaudrate
    cfsetospeed(&tty, B57600); // Ausgangsbaudrate
    tty.c_cflag &= ~PARENB;   // Keine Parität
    tty.c_cflag &= ~CSTOPB;   // 1 Stopbit
    tty.c_cflag &= ~CSIZE;    // Lösche die Datenbits
    tty.c_cflag |= CS8;       // 8 Datenbits
    tty.c_cflag |= CREAD | CLOCAL; // Aktiviere Lesen und ignoriere Modemsteuerung

    // Setze die Eingabe- und Ausgabe-Flags
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Keine Flusskontrolle
    tty.c_oflag &= ~OPOST; // Rohausgabe
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Rohmodus

    // Wende die Einstellungen an
    tcsetattr(serial_port, TCSANOW, &tty);


    // Vektor der Daten zum Senden enthält
    std::vector<uint8_t> bytes = {0x02, 0x09, 0x01};
    encoder enc = encoder(0x80, bytes);

    while (enc.hasData())
    {
        uint8_t message = enc.nextByte().value();

        // Sende Daten an den seriellen Port
        int n = write(serial_port, &message, sizeof(message)); // Übergib die Adresse von message
        if (n < 0) {
            std::cerr << "Fehler beim Schreiben auf den seriellen Port!" << std::endl;
        } else {
            std::cout << "Gesendet: " << static_cast<int>(message) << std::endl; // Konvertiere zu int für die Ausgabe
        }
    }

    // Schließe den seriellen Port
    close(serial_port);
    return 0;
}