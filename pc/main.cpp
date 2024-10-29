#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>

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

    char buffer[256];
    while (true) {
        // Lies Daten vom seriellen Port
        int n = read(serial_port, buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0'; // Null-terminiere den String
            std::cout << "Empfangen: " << buffer << std::endl;
        } else if (n < 0) {
            std::cerr << "Fehler beim Lesen vom seriellen Port!" << std::endl;
        }
    }

    // Schließe den seriellen Port
    close(serial_port);
    return 0;
}
