/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <random>

namespace Netero::Rand {

class Engine;

class Engine {
    public:
    Engine();

    static void     Seed(uint32_t);
    static uint32_t IntegerUniformDistribution();
    static double   DoubleUniformDistribution();

    static uint32_t IntegerRangedUniformDistribution(uint32_t, uint32_t);
    static double   DoubleRangedUniformDistribution(double, double);

    template<int mean, int stddev>
    static uint32_t IntegerNormalDistribution()
    {
        static std::normal_distribution<double> normalDistribution(mean, stddev);
        return normalDistribution(anEngine.myGenerator);
    }

    template<int mean, int stddev>
    static double DoubleNormalDistribution()
    {
        static std::normal_distribution<double> normalDistribution(mean, stddev);
        return normalDistribution(anEngine.myGenerator);
    }

    private:
    std::seed_seq                           mySeed;
    std::mt19937                            myGenerator;
    std::uniform_int_distribution<uint32_t> myIntegerUniformDistributor;
    std::uniform_real_distribution<double>  myDoubleUniformDistributor;
    static thread_local Engine              anEngine;
};

} // namespace Netero::Rand
