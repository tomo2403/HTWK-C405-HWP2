#include "Observer.hpp"
#include "Codec.hpp"

class CommandObserver : public Observer {
public:
	explicit CommandObserver(bool &finished) : finished(finished) {}

	void update(uint8_t signal) override {
		switch (signal)
		{
			case CodecCommand::everythingSend:
				finished = true;
				break;
			default:
				break;
		}
    }

private:
	bool &finished;
};