#include "Observer.hpp"

void Subject::addObserver(Observer* observer) {
	observers.push_back(observer);
}

void Subject::removeObserver(Observer* observer) {
	observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void Subject::notifyObservers(uint8_t signal) {
	for (Observer* observer : observers) {
		observer->update(signal);
	}
}