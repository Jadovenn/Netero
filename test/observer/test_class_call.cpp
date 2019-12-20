#include <netero/observer/slots.hpp>
#include <netero/observer/IObserverDelegate.hpp>

/**
 *  Created by Jean-Adrien DOMAGE on 12/12/2019.
 */

class	Number {
public:

	Number() = delete;

	explicit Number(int nb)
		:	number(nb)
	{}

	int		add(int other) {
		number += other;
		return number;
	}

	[[nodiscard]] int get() const {
		return number;
	}

private:
	int number;
};

int main() {
	Number	integer(12);
	netero::slot<int(int)>	slot(&Number::add, &integer);
	slot(30);
	if (integer.get() != 42) {
		return 1;
	}
	return 0;
}
