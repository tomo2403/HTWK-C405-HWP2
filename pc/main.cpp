#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <cstdint>
#include <vector>

#include "../lib/Encoder.hpp"
#include "../lib/Decoder.hpp"
#include "../lib/ioManager.hpp"
#include <bitset>

void setupSerialPort(int *serial_port) {
	// Öffne den seriellen Port
	const char* portname = "/dev/ttyUSB0"; // Ersetze dies durch deinen Port
	*serial_port = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);

	if (*serial_port < 0) {
		std::cerr << "Fehler beim Öffnen des seriellen Ports!" << std::endl;
		exit(1);
	}

	// Konfiguration des seriellen Ports
	struct termios tty{};
	memset(&tty, 0, sizeof(tty));

	// Hole die aktuellen Einstellungen
	if (tcgetattr(*serial_port, &tty) != 0) {
		std::cerr << "Fehler beim Abrufen der Port-Einstellungen!" << std::endl;
		exit(1);
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
	tcsetattr(*serial_port, TCSANOW, &tty);
}

int main() {
    int serial_port;
	setupSerialPort(&serial_port);

    // Vektor der Daten zum Senden enthält
	std::vector<uint8_t> data = ioManager::getBinaryPipeContent();
	std::vector<uint8_t> receivedDataVector;
    Encoder enc = Encoder(0x80, data);
	Decoder dec = Decoder(0x80, receivedDataVector);

	bool finishedDecoding = false;
    while (enc.hasData() && !finishedDecoding)
    {
        uint8_t message = enc.nextByte().value();

        // Sende Daten an den seriellen Port
        ssize_t n = write(serial_port, &message, sizeof(message)); // Übergib die Adresse von message
        if (n < 0) {
			throw std::runtime_error("Fehler beim Schreiben auf den seriellen Port!");
        }

		// receive data from serial port
		uint8_t receivedData;
		ssize_t m = read(serial_port, &receivedData, sizeof(receivedData));
		if (m < 0) {
			throw std::runtime_error("Fehler beim Lesen vom seriellen Port!");
		}
		dec.nextNibble(receivedData);

		// TODO: break on finish signal
    }

    // Schließe den seriellen Port
    close(serial_port);

	// print received data
	ioManager::printVector(receivedDataVector);

    return 0;
}