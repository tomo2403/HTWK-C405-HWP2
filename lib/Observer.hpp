#pragma once

#include <vector>
#include <algorithm>

class Observer {
public:
	virtual ~Observer() = default;
	virtual void update(uint8_t signal) = 0;
};

class Subject {
public:
	void addObserver(Observer* observer);

	void removeObserver(Observer* observer);

	void notifyObservers(uint8_t signal);

private:
	std::vector<Observer*> observers;
};