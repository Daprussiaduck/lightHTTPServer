#ifndef __LIGHT_HTTP_SERVER_HPP__
#define __LIGHT_HTTP_SERVER_HPP__

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <poll.h>
#include <vector>
#include <map>

#include "ThreadPool.hpp"
#include "HTTP_Message.hpp"

#define LIGHT_HTTP_SERVER_POLLING_TIMEOUT 1000

namespace lightHTTPServer {
    typedef std::pair<int, std::string> endpoint;
    class Server {
        public:
            Server(short port = 8888, int numThreads = 1, std::string certFileName = "cert.pem", std::string keyFileName = "key.pem");
            ~Server();
            void addEndpoint(int method, std::string uri, const std::function<nlohmann::json(nlohmann::json)> &endpointTask);
            void runServer();
            bool setStaticDirectory(std::string staticDir);
            void stopServer();
        private:

            void bindSocket();
            void handleClient(int fd);

            const unsigned int BUFFER_LENGTH = 10 * 1024 * 1024;
            std::map<endpoint, std::function<nlohmann::json(nlohmann::json)>> endpoints;
            unsigned short listenPort;
            ThreadPool* pool;
            unsigned int numWorkerThreads;
            volatile bool run;
            int serverSocket;
            struct sockaddr_in serverAddress;
            std::string staticDirectory;
            std::string SSL_CertFile;
            std::string SSL_KeyFile;
            SSL_CTX* sslCtx;
    };
};

#endif /*__LIGHT_HTTP_SERVER_HPP__*/