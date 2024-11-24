#include <mutex>
#include <condition_variable>
#include <b15f/b15f.h>

class B15Communication
{
private:
    B15F& drv = B15F::getInstance();
    
    mutable std::mutex mtx;  // Mutex für synchronisierten Zugriff auf die Register
    std::condition_variable cv;  // Optional, falls ein Thread gewartet werden soll

public:
    // Setzen und Abrufen von DDRA
    void setDDRA(const uint8_t value);
    uint8_t getDDRA();

    // Setzen und Abrufen von PORTA
    void setPORTA(const uint8_t value);
    uint8_t getPINA();
};

void B15Communication::setDDRA(const uint8_t value)
{
    std::lock_guard<std::mutex> lock(mtx);  // Sperre des Mutex beim Setzen von DDRA
    drv.setRegister(&DDRA, value);
}

uint8_t B15Communication::getDDRA()
{
    std::lock_guard<std::mutex> lock(mtx);  // Sperre des Mutex beim Abrufen von DDRA
    return drv.getRegister(&DDRA);
}

void B15Communication::setPORTA(const uint8_t value)
{
    std::lock_guard<std::mutex> lock(mtx);  // Sperre des Mutex beim Setzen von PORTA
    drv.setRegister(&PORTA, value);
}

uint8_t B15Communication::getPINA()
{
    std::lock_guard<std::mutex> lock(mtx);  // Sperre des Mutex beim Abrufen von PORTA
    return drv.getRegister(&PINA);
}
