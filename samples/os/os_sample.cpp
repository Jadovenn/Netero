/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <fstream>
#include <iostream>

#include <Netero/Logger.hpp>
#include <Netero/Os.hpp>

class CustomLogger: public Netero::Logger {
    public:
    explicit CustomLogger(std::ostream& out): Netero::Logger(out) {}

    protected:
    void printPlaceholder() const final { *this->_stream << "[Custom placeholder] "; }
};

int main()
{
    Netero::Os::AcquireSystemResources();
    // On windows COM library is now initialized.
    // But if you or another library in your program have already done
    // netero modules will conform to it. Netero Modules are likely to use this
    // then you don't need to do it explicitly.
    // But this function are a debug starting point if
    // you are getting into trouble on windows/netero/COM interfaces.
    Netero::Os::ReleaseSystemResources();

    LOG << Netero::Os::GetSessionUsername() << std::endl;
    LOG << Netero::Os::GetUserHomeDirectoryPath() << std::endl;
    LOG << Netero::Os::GetUserAppDataRoamingPath() << std::endl;
    LOG << Netero::Os::GetBundlePath() << std::endl;
    LOG << Netero::Os::GetExecutablePath() << std::endl;

    LOG << "A raw log." << std::endl;
    LOG_INFO << "A simple log." << std::endl;
    LOG_DEBUG << "A debug log." << std::endl;
    LOG_WARNING << "A Warning!" << std::endl;
    LOG_ERROR << "An Error!" << std::endl;

    CustomLogger myCustomLogger(std::cout);

    Netero::Log(myCustomLogger, Netero::Level::Info) << "A custom Logger" << std::endl;

    std::ofstream  myLogFile("MyLogFile.txt", std::ofstream::out | std::ofstream::trunc);
    Netero::Logger myOtherLogger(myLogFile);

    Netero::Log(myOtherLogger, Netero::Level::Info) << "Hello friend!" << std::endl;
    Netero::Log(myOtherLogger, Netero::Level::Info) << "This log is saved in a file." << std::endl;

    myLogFile.close();

    // This is that simple, but this may not have outstanding performance
    return 0;
}