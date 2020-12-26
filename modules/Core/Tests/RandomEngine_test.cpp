/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <Netero/Logger.hpp>
#include <Netero/RandomEngine.hpp>

#include <gtest/gtest.h>

TEST(NeteroCore, RandomEngine)
{
    LOG_INFO << Netero::Rand::Engine::IntegerUniformDistribution() << std::endl;
    LOG_INFO << Netero::Rand::Engine::DoubleUniformDistribution() << std::endl;
    LOG_INFO << Netero::Rand::Engine::IntegerRangedUniformDistribution(0, 9) << std::endl;
    LOG_INFO << Netero::Rand::Engine::DoubleRangedUniformDistribution(0.5, 0.7) << std::endl;
    LOG_INFO << Netero::Rand::Engine::IntegerNormalDistribution<5, 2>() << std::endl;
    LOG_INFO << Netero::Rand::Engine::DoubleNormalDistribution<100, 20>() << std::endl;
}

TEST(NeteroCore, RandEngineSeedTest)
{
    unsigned anInteger = Netero::Rand::Engine::IntegerUniformDistribution();
    Netero::Rand::Engine::Seed(5);
    unsigned anotherInteger = Netero::Rand::Engine::IntegerUniformDistribution();
    ASSERT_NE(anInteger, anotherInteger);
    ASSERT_EQ(anotherInteger, 953453411);
}