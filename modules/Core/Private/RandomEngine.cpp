/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <cstdlib>

#include <Netero/RandomEngine.hpp>

namespace Netero::Rand {

thread_local Engine Engine::anEngine;

Engine::Engine()
{
    srand(time(nullptr));
    myGenerator.seed(rand());
}

void Engine::Seed(uint32_t aSeed)
{
    anEngine.myGenerator.seed(aSeed);
}

uint32_t Engine::IntegerUniformDistribution()
{
    return anEngine.myIntegerUniformDistributor(anEngine.myGenerator);
}

double Engine::DoubleUniformDistribution()
{
    return anEngine.myDoubleUniformDistributor(anEngine.myGenerator);
}

uint32_t Engine::IntegerRangedUniformDistribution(uint32_t aFirst, uint32_t aSecond)
{
    std::uniform_int_distribution<uint32_t> uint_dist(aFirst, aSecond);
    return uint_dist(anEngine.myGenerator);
}

double Engine::DoubleRangedUniformDistribution(double aFirst, double aSecond)
{
    std::uniform_real_distribution<double> double_dist(aFirst, aSecond);
    return double_dist(anEngine.myGenerator);
}

} // namespace Netero::Rand
