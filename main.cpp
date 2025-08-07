#include <iostream>
#include <string>
bool testConnection(std::string hostName, int port);
int main(){
    // Scan specific port
    std::string hostName;
    int port = 22;
    std::string portStr;
    try {
        port = std::stoi(portStr);
    }
    catch (...) {
        std::cout << "Invalid port number." << std::endl;
        exit(0);
    }

    if (port < 0 || port > 65535){
        std::cout << "Invalid port number\n";
        exit(0);
    }
    if (testConnection(hostName, port)){
            std::cout << "Port " << port << " for " << hostName << " is open!\n";
    }
    else{
        std::cout << "Port " << port << " for " << hostName << " is closed.\n";
    }


    return 0;
}

bool testConnection(std::string hostName, int port){
    
}