#include "Observer.h"

namespace neu {
	IObserver::~IObserver() {
		REMOVE_OBSERVER;
	}
}