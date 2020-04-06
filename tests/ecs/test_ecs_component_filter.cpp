/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <netero/ecs/component.hpp>
#include <netero/ecs/component_filter.hpp>

struct CompA: public netero::ecs::Component {
	int	data;
};

struct CompB : public netero::ecs::Component {
	char data;
};

using	GroupAB = netero::ecs::ComponentFilter<CompA, CompB>;
using	GroupA = netero::ecs::ComponentFilter<CompA>;
using	GroupB = netero::ecs::ComponentFilter<CompB>;
using	GroupC = netero::ecs::ComponentFilter<>;

int	main() {
	const netero::set<std::size_t>	&filterAB = GroupAB::getFilter();
	const netero::set<std::size_t>	&filterA = GroupA::getFilter();
	const netero::set<std::size_t>	&filterB = GroupB::getFilter();
	const netero::set<std::size_t>	&filterC = GroupC::getFilter();
	if (!filterA.isSubsetOf(filterAB))
		return 1;
	if (!filterB.isSubsetOf(filterAB))
		return 1;
	if (filterA.isSubsetOf(filterB))
		return 1;
	if (filterAB.isSubsetOf(filterA))
		return 1;
	return 0;
}
