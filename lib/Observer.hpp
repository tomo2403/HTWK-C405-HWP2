#pragma once

#include <vector>
#include <algorithm>

class Observer {
public:
	virtual ~Observer() = default;
	virtual void update() = 0;
};

class Subject {
public:
	void addObserver(Observer* observer) {
		observers.push_back(observer);
	}

	void removeObserver(Observer* observer) {
		observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
	}

	void notifyObservers() {
		for (Observer* observer : observers) {
			observer->update();
		}
	}

private:
	std::vector<Observer*> observers;
};