#include <lightHTTPServer.hpp>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>

lightHTTPServer::Server* serv;

void signalHandler(int sig){
    signal(sig, SIG_IGN);
    if (serv != nullptr){
        std::cout << "Stopping Server" << std::endl;
        serv -> stopServer();
    }
}

int main(int argc, char** argv){
    std::cout << "Running the main test for lightHTTPServer" << std::endl;
    signal(SIGINT, signalHandler);
    serv = new lightHTTPServer::Server(8888, -1, "../ThinkArch.cert.pem", "../ThinkArch.key.pem");
    if (!(serv -> setStaticDirectory("../public"))){
        return EXIT_FAILURE;
    }
    serv -> addEndpoint(lightHTTPServer::HTTP_Method::GET, "/test", [](nlohmann::json jason){
        std::cerr << "Hello from the endpoint" << std::endl;
        std::cout << "Query parameters from endpoint:" << jason << std::endl;
        return nlohmann::json::parse("{\"test\": 0}");
    });
    serv -> addEndpoint(lightHTTPServer::HTTP_Method::POST, "/test", [](nlohmann::json jason){
        std::cerr << "Hello from the endpoint" << std::endl;
        return nlohmann::json::parse("{\"test\": 0}");
    });
    serv -> runServer();
    delete serv;
    std::cout << "\nMain Testing Done" <<std::endl;
    return EXIT_SUCCESS;
}