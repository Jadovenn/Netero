/**
* Netero sources under BSD-3-Clause
* see LICENSE.txt
*/

#include <cassert>
#include <functional>
#include <iostream>

#include <netero/signal.hpp>

#include <gtest/gtest.h>

class Subject {
    public:
    explicit Subject(const std::string &name): _name(name), _age(0) {}

    int operator++()
    {
        _age += 1;
        birthday(_name, _age);
        return _age;
    }

    netero::signal<void(const std::string &, int)> birthday;

    private:
    const std::string &_name;
    int                _age;
};

class Observer {
    public:
    Observer(): onBirthday(&Observer::notifyBirthday, this) {}

    void notifyBirthday(const std::string &name, int age)
    {
        std::cout << "\033[0;32m"
                  << "[     cout ] "
                  << "\033[0;0m"
                  << "Observer(" << this << ") "
                  << "Happy birthday " << name << ", you have " << age << " now!" << std::endl;
    }

    netero::slot<void(const std::string &, int)> onBirthday;
};

TEST(NeteroPatterns, basic_test)
{
    Subject  bob("bob");
    Subject  marina("marina");
    Observer notificationManager;
    bob.birthday.connect(&notificationManager.onBirthday);
    marina.birthday.connect(&notificationManager.onBirthday);
    ++bob;
    ++marina;
}

// corrupt subject by connecting corrupted observer
TEST(NeteroPatterns, corrupt_subject)
{
    Subject   subject("Bob");
    Observer *notificationManager = new Observer;
    subject.birthday.connect(&notificationManager->onBirthday);
    delete notificationManager;
    ++subject;
}

TEST(NeteroPatterns, corrupt_observer)
{
    Subject *bob = new Subject("bob");
    Observer notificationManager;
    bob->birthday.connect(&notificationManager.onBirthday);
    delete bob;
}

TEST(NeteroPatterns, signal_connect_disconnect)
{
    netero::signal<int(const int &, const int &)> signal;
    netero::slot<int(const int &, const int &)>   slot1;
    netero::slot<int(const int &, const int &)>   slot2;
    signal.connect(&slot1);
    signal.connect(&slot2);
    EXPECT_EQ(signal.size(), 2);
    signal.disconnect(&slot2);
    EXPECT_EQ(signal.size(), 1);
    signal.reset();
    EXPECT_EQ(signal.size(), 0);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
