#ifndef MAIN_H
#define MAIN_H

#include <kademlia/session.hpp>
#include <kademlia/first_session.hpp>

#include "clara.h"

#include <memory>
#include <iostream>
#include <thread>
#include <future>
#include <iterator>

using namespace std;
using namespace Clara;

struct Options {
    Options() : port(kademlia::session_base::DEFAULT_PORT) {}

    std::string endpoint;
    std::uint16_t port;
    bool showHelp;
};

struct Main{
    int run(int argc, const char ** argv);
    
    bool processCommandLine(int argc, const char ** argv);
    
    void runFirstNode();
    void runNode(const string & endpointStr);
    
public:
    const kademlia::session::data_type asBinary(const string& value);
    
protected:
    void processNodeInput();
    
    void printCommands();
    void processInput();
    void discardBlanks(std::istream & input);
    std::string readArg(std::stringstream & input);
    void handleCommand(std::stringstream & input);
    
    void doGet(const std::string & key);
    void doPut(const std::string & key, const std::string & value);
private:    
    Options cliArgs;

    std::shared_ptr<kademlia::first_session> firstNode;
    std::shared_ptr<kademlia::session> node;
};

#endif // MAIN_H
