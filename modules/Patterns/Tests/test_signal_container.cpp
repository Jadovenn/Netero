/**
* Netero sources under BSD-3-Clause
* see LICENSE.txt
*/

#include <functional>
#include <iostream>

#include <Netero/Logger.hpp>
#include <Netero/Signal.hpp>

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

    Netero::Signal<void(const std::string &, int)> birthday;

    private:
    const std::string _name;
    int               _age;
};

class Observer {
    public:
    Observer(): onBirthday(&Observer::notifyBirthday, this) {}

    void notifyBirthday(const std::string &name, int age)
    {
        LOG_INFO << "Observer(" << this << ") "
                 << "Happy birthday " << name << ", you have " << age << " now!" << std::endl;
    }

    Netero::Slot<void(const std::string &, int)> onBirthday;
};

TEST(NeteroPatterns, basic_test)
{
    Subject  bob("bob");
    Subject  marina("marina");
    Observer notificationManager;
    bob.birthday.Connect(&notificationManager.onBirthday);
    marina.birthday.Connect(&notificationManager.onBirthday);
    ++bob;
    ++marina;
}

// corrupt subject by connecting corrupted observer
TEST(NeteroPatterns, corrupt_subject)
{
    Subject   subject("Bob");
    Observer *notificationManager = new Observer;
    subject.birthday.Connect(&notificationManager->onBirthday);
    delete notificationManager;
    ++subject;
}

TEST(NeteroPatterns, corrupt_observer)
{
    Subject *bob = new Subject("bob");
    Observer notificationManager;
    bob->birthday.Connect(&notificationManager.onBirthday);
    delete bob;
}

TEST(NeteroPatterns, signal_connect_disconnect)
{
    Netero::Signal<int(const int &, const int &)> signal;
    Netero::Slot<int(const int &, const int &)>   slot1;
    Netero::Slot<int(const int &, const int &)>   slot2;
    signal.Connect(&slot1);
    signal.Connect(&slot2);
    EXPECT_EQ(signal.Size(), 2);
    signal.Disconnect(&slot2);
    EXPECT_EQ(signal.Size(), 1);
    signal.Reset();
    EXPECT_EQ(signal.Size(), 0);
}
