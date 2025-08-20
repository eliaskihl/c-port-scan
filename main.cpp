#include <iostream>
#include <string>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <csignal>
#include <atomic>
#include <vector>
#include <thread>

bool is_port_open(std::string ip, int port);
void pingAddressInNetwork(std::string baseAddr,int start, int end);
void runPing();
int main(){

    runPing();
    exit(0);
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
//TODO: Make main menu

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
std::atomic<bool> stop{false};
void handle_sigint(int) {
          stop = true;
}
void alive(int a) { std::cout<<"Alive! ID:" << a << "\n";}


void runPing(){
  std::string baseAddr = "192.168.1."; // Should be replaced such that is it an argument inputted from the user
  // create threads 
  std::vector<std::thread> threads;
  const size_t number_of_threads = 4;
  threads.reserve(number_of_threads);
  // split the address space from 0 - 255 in even chunks
  const int totalAddr = 256;
  int chunk = totalAddr / number_of_threads;
  int remainder = totalAddr  % number_of_threads;
  std::cout << remainder << std::endl;
  int start = 0;
  int end;
  
  // run threads with specific ranges
  for (int i=0; i < number_of_threads; i++){
    
    start = (i)*(chunk);
    end = (i+1)*(chunk)-1;
    if (i == (number_of_threads-1)){
      end += remainder;
    }
    //std::cout << start << " TO " << end << " DIFF: " << end-start << std::endl;
    threads.emplace_back(std::thread(pingAddressInNetwork, baseAddr, start, end));
  }
  
  // join threads
  for (int i = 0; i < threads.size(); ++i) {
        threads[i].join();
  }
}
void pingAddressInNetwork(std::string baseAddr,int start, int end){
      
      for (int i=start; i <= end; i++){
        std::string ipAddr = baseAddr + std::to_string(i);
        // Add different command dependent on the OS
        std::string command = "ping -c 1 -W 1 " + ipAddr + " > /dev/null";

        int result = system(command.c_str());
        if (result == 0){
          std::cout << "Successfully reached: " << ipAddr << std::endl;
        }
        else{
          std::cout << "Failed to reach: " << ipAddr << std::endl;
        }
      }
}