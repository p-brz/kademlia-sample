#include "main.h"

int main(int argc, const char ** argv){
    return Main{}.run(argc, argv);    
}

int Main::run(int argc, const char ** argv){
    if(!processCommandLine(argc, argv)){
        return -1;
    }

    if(cliArgs.endpoint.empty()){
        runFirstNode();
    }
    else{
        runNode(cliArgs.endpoint);
    }
    
    return 0;
}

bool Main::processCommandLine(int argc, const char ** argv){
    CommandLine<Options> cli;

    cli[1]
        .describe("Dht endpoint used to bootstraping. ex.: 1.2.3.4:8080")
        .bind(&Options::endpoint, "endpoint");
    cli["-p"]["--port"]
        .describe("Local port (defaults to " + std::to_string(kademlia::session_base::DEFAULT_PORT) + ")")
        .bind(&Options::port, "port");
    cli["-h"]["--help"]
        .describe("Display usage information")
        .bind(&Options::showHelp);

    cli.parseInto(Clara::argsToVector(argc, argv), this->cliArgs);

    if(this->cliArgs.showHelp){
        cli.usage(std::cout, argv[0]);
        return false;
    }

    return true;
}

void Main::runFirstNode(){
    firstNode = std::make_shared<kademlia::first_session>(
        kademlia::endpoint{ "0.0.0.0", cliArgs.port },
        kademlia::endpoint{ "::", cliArgs.port });
    
    auto main_loop_result = std::async( std::launch::async
                                        , &kademlia::first_session::run, firstNode );
                                        
    cout << "Bootstraping DHT at port " << cliArgs.port << endl;
    cout << "Press ENTER to finish" << endl;
    
    string ignore;
    std::getline(cin, ignore);

    cout << "finishing..." << endl;
    
    firstNode->abort();
    
    main_loop_result.get();    
}

void Main::runNode(const std::string & endpointStr){
    auto separatorIdx = endpointStr.find(':');
    auto host = endpointStr.substr(0, separatorIdx);
    auto port = endpointStr.substr(separatorIdx + 1);
    if(port.empty()){
        port = std::to_string(kademlia::session_base::DEFAULT_PORT);
    }

    auto endpoint = kademlia::endpoint{host, port};
    node = std::make_shared<kademlia::session>(
        endpoint,
        kademlia::endpoint{ "0.0.0.0", cliArgs.port },
        kademlia::endpoint{ "::", cliArgs.port });
        
    auto main_loop_result = std::async( std::launch::async
                                        , &kademlia::session::run, node );
                                        
    cout << "Running node at port " << cliArgs.port << endl;
    cout << "Connecting to " << endpoint << endl;
    
    
    processInput();
    
    node->abort();
    
    main_loop_result.get();    
}

void Main::processInput(){
    printCommands();
    
    std::string line;
    
    while(std::getline(std::cin, line) && !line.empty()){
        std::stringstream input(line);
        handleCommand(input);
    }
    
    std::cout << "Bye!" << std::endl;
}

void Main::printCommands(){
    std::cout << "Commands:" << std::endl;
    std::cout << "\tget <key>           : try get (asynchronously) the given key value" << std::endl;
    std::cout << "\tput <key> <value>   : put a value with the given key" << std::endl;
    std::cout << "\thelp                : print this instruction" << std::endl;    
    std::cout << "Press ENTER to exit" << std::endl;    
}

void Main::handleCommand(std::stringstream & input){
    std::string command, key, value;
    input >> command;

    std::transform(command.begin(), command.end(), command.begin(), ::tolower);

    if(command == "get"){
        key = readArg(input);
        doGet(key);
        return;
    }
    
    if(command == "put"){
        key = readArg(input);
        value = readArg(input);
        
        doPut(key, value);
        return;
    }
    
    if(command != "help"){
        cout << "unknown command '" << command << "'" << endl;
    }
    printCommands();
    return;
}

void Main::doGet(const string & key){

    this->node->async_load(asBinary(key), []( std::error_code const& failure
                     , kademlia::session::data_type const& data )
    {
        if ( failure ){
            std::cerr << failure.message() << std::endl;
        }
        else{
            std::string dataAsString;
            
            for(const auto byte : data){
                dataAsString.push_back(byte);
            }
            
            cout << dataAsString << endl;                     
        }
    });
}
void Main::doPut(const string & key, const string & value){
    this->node->async_save(asBinary(key), asBinary(value),
        [key, value]( std::error_code const& failure ){
            if ( failure ){
                std::cerr << failure.message() << std::endl;
            }
            else{
                cout << "put succeded: " << key << " -> " << value << endl;
            }
        });
}

const kademlia::session_base::data_type Main::asBinary(const string& value){
    return kademlia::session::data_type{ value.begin(), value.end() };
}

std::string Main::readArg(std::stringstream& input){
    std::string quoted;
    
    discardBlanks(input);
    
    if(!input.good()){
        return string();
    }
    
    if(input.peek() == '"'){
        input.ignore(1);
        std::getline(input, quoted, '"');
    }
    else{
        input >> quoted;
    }
    
    return quoted;
}

void Main::discardBlanks(std::istream& input){
    while(input.good() && isblank(input.peek())){
        input.get();
    }    
}
