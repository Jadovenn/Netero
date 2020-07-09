/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <string>
#include <type_traits>
#include <vector>

#include <netero/ecs/component.hpp>
#include <netero/ecs/world.hpp>

netero::ecs::World  world;
netero::ecs::Entity first = world.createEntity();
netero::ecs::Entity second = world.createEntity();
netero::ecs::Entity third = world.createEntity();
netero::ecs::Entity fourth = world.createEntity();
netero::ecs::Entity fifth = world.createEntity();

struct Position: public netero::ecs::Component {
    Position(int x = 0, int y = 0): x(x), y(y) {}
    int x;
    int y;
};

struct Name: public netero::ecs::Component {
    Name(const std::string &name = ""): name(name) {}
    std::string name;
};

struct Text: public netero::ecs::Component {
    Text(const std::string &text = ""): text(text) {}
    std::string text;
};

template<typename T>
struct Path: public netero::ecs::Component {
    public:
    template<typename... Points>
    Path(Points... args)
    {
        decomposer(args...);
        idx = 0;
    }
    std::vector<std::pair<T, T>> points;
    int                          idx;
    std::size_t                  size;

    private:
    template<typename First, typename Second>
    void decomposer(First first, Second second)
    {
        static_assert(std::is_same<First, T>::value && std::is_same<Second, T>::value,
                      "First must be type T");
        points.push_back({ first, second });
        size = points.size();
    }

    template<typename First, typename Second, typename... Rest>
    void decomposer(First first, Second second, Rest... rest)
    {
        static_assert(std::is_same<First, T>::value && std::is_same<Second, T>::value,
                      "First must be type T");
        points.push_back({ first, second });
        decomposer(rest...);
    }
};
