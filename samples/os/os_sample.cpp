/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <fstream>
#include <iostream>

#include <netero/logger.hpp>
#include <netero/os.hpp>

class CustomLogger: public netero::Logger {
    public:
    CustomLogger(std::ostream& out): netero::Logger(out) {}

    protected:
    void printPlaceholder() const final { *this->_stream << "[Custom placeholder] "; }
};

int main()
{
    netero::os::acquireSystemResources();
    // On windows COM library is now initialized.
    // But if you or another library in your program have already done
    // netero modules will conform to it. Netero Modules are likely to use this
    // then you don't need to do it explicitly.
    // But this function are a debug starting point if
    // you are getting into trouble on windows/netero/COM interfaces.
    netero::os::releaseSystemResources();

    LOG << netero::os::getSessionUsername() << std::endl;
    LOG << netero::os::getUserHomeDirectoryPath() << std::endl;
    LOG << netero::os::getUserAppDataRoamingPath() << std::endl;
    LOG << netero::os::getBundlePath() << std::endl;
    LOG << netero::os::getExecutablePath() << std::endl;

    LOG << "A raw log." << std::endl;
    LOG_INFO << "A simple log." << std::endl;
    LOG_DEBUG << "A debug log." << std::endl;
    LOG_WARNING << "A Warning!" << std::endl;
    LOG_ERROR << "An Error!" << std::endl;

    CustomLogger myCustomLogger(std::cout);

    netero::log(myCustomLogger, netero::Level::info) << "A custom Logger" << std::endl;

    std::ofstream  myLogFile("MyLogFile.txt", std::ofstream::out | std::ofstream::trunc);
    netero::Logger myOtherLogger(myLogFile);

    netero::log(myOtherLogger, netero::Level::info) << "Hello friend!" << std::endl;
    netero::log(myOtherLogger, netero::Level::info) << "This log is saved in a file." << std::endl;

    myLogFile.close();

    // This is that simple, but this may not have outstanding performance
    return 0;
}