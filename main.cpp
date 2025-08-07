#include <iostream>
#include <string>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

bool is_port_open(std::string ip, int port);
int main(){
    // Scan specific port
    std::string hostName;
    hostName = "172.20.32.1";
    int port;
    std::string portStr = "139";
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
    if (is_port_open(hostName, port)){
            std::cout << "Port " << port << " for " << hostName << " is open!\n";
    }
    else{
        std::cout << "Port " << port << " for " << hostName << " is closed.\n";
    }


    return 0;
}

bool is_port_open(std::string ip, int port){
    struct sockaddr_in addr;
    int new_socket = -1;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr((ip.c_str()));

    new_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (new_socket==-1) {
      std::cout << "Socket creation failed on port " << port << std::endl;
      return false;
    }
    fcntl(new_socket, F_SETFL, O_NONBLOCK);
    connect(new_socket, (struct sockaddr*) &addr, sizeof(addr));

    // create file descriptor for socket
    fd_set file_descriptor;
    struct timeval timeout;

    // Reset the file descriptor (null)
    FD_ZERO(&file_descriptor);
    // Set socket to file descriptor
    FD_SET(new_socket, &file_descriptor);

    // Set timeout
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    int con_res = select(new_socket+1,NULL,&file_descriptor,NULL,&timeout);

    if (con_res == 1) {
      int sock_err;
      socklen_t len = sizeof sock_err;

      getsockopt(new_socket, SOL_SOCKET, SO_ERROR, &sock_err, &len);

      if (sock_err==0) {
        close(new_socket);
        return true;
      }
      else {
        close(new_socket);
        return false;
      }
    }
    close(new_socket);
    return false;



}