/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include "netero/ECS/world.hpp"

namespace netero {
	namespace ecs {

		World::Cache::Cache()
			:	statusFlag(true)
		{}

		/**
		 * @details tick() the cache consiste of raising the statusFlag to tell to
		 * the world scheduler to perform a cache regeneration and garbage collection
		 */
		void	World::Cache::tick() {
			statusFlag = true;
		}

		/**
		 * @details Put in cache the activated entities and set the statusFlag to false
		 *  - O(n) where n is the number of entities
		 * @warning this could take time with a really big n, the max managed capacity of entities depend
		 * of the quantity of ram and the speed of the precessor of the computer
		 */
		void	World::Cache::generate(std::vector<EntityContainer*> &entities) {
			for (auto *ent : entities) {
				if (ent->status) {
					activeEntities.push_back(ent);
				}
				else {
					unactiveEntities.push_back(ent);
				}
				statusFlag = false;
			}
		}

		/**
		 * @details delete the killedEntities
		 */
		void	World::Cache::collectGarbage() {
			for (auto *killedEnt : killedEntities) {
				delete killedEnt;
			}
			killedEntities.clear();
		}
	}
}
