/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include "ecs_dataset.hpp"

namespace ecs = netero::ecs;

// Will get all the enabeled entities
struct Alltest : public ecs::System<> {

};

// this system will change position depending to the path of entities that contains a Path<int> and Position component
class PathSystem : public ecs::System<ecs::ComponentFilter<Path<int>, Position>>
{
public:

	virtual ~PathSystem() = default;
	void	exec() final {
		/**
		for (auto entity : getEntities()) {
			auto &position = entity->getComponent<Position>();
			auto &path = entity->getComponent<Path<int>>();
			int next_x = path.points[path.idx].first;
			int next_y = path.points[path.idx].second;
			position.x = next_x;
			position.y = next_y;
		}
		*/
	}
};

int		main() {
	// Set up first entity
	first->addComponent<Position>(10, 4);
	first->addComponent<Name>("PNJ-1");
	first->addComponent<Path<int>>(10, 4, 12, 16, 18, 30);
	first.enable();

	// Set up second entity
	second->addComponent<Position>();
	second->addComponent<Name>("PNJ-2");
	second->addComponent<Path<int>>(0, 0, 5, 10, 10, 5, 10, 20);
	second.enable();

	// Set up third entity
	third->addComponent<Position>(40, 40);
	third->addComponent<Name>("Village Pannel");
	third->addComponent<Text>("Welecome to the village!");
	third.enable();

	// Set up fourth entity
	fourth->addComponent<Position>(100, 100);
	fourth->addComponent<Name>("House");
	fourth.enable();

	world.addSystem<PathSystem>();
	world.removeSystem<PathSystem>();

	return 0;
}
