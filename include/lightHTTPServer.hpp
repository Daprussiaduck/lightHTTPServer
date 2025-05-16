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
    /**
     * @brief An endpoint to attach to the server
     */
    typedef std::pair<int, std::string> endpoint;

    /**
     * @brief An HTTP Server with SSL support
     */
    class Server {
        public:
            /**
             * @brief Constructs a Server object
             * 
             * @param port The port to run the server on
             * @param numThreads The number of threads to run the client handlers on
             * @param certFileName The name of the Certificate file to use for SSL
             * @param keyFileName The name of the Key file to use for SSL
             */
            Server(short port = 8888, int numThreads = 1, std::string certFileName = "cert.pem", std::string keyFileName = "key.pem");
            
            /**
             * @brief Destructs the server and stops the client handlers
             */
            ~Server();

            /**
             * @brief Adds an endpoint to the server to handle
             * 
             * @param method The HTTP method to listen for
             * @param uri The URI to attach to
             * @param endpointTask The Function/task to run when a valid request has been made
             */
            void addEndpoint(int method, std::string uri, const std::function<nlohmann::json(nlohmann::json)> &endpointTask);
            
            /**
             * @brief Starts the threadpool to handle clients
             */
            void runServer();

            /**
             * @brief Sets the directory to serve static files from
             * 
             * @param staticDir The directory containing the files for staticly sending
             * @return True if successful, false if not
             */
            bool setStaticDirectory(std::string staticDir);
            
            /**
             * @brief Stops the server and thread pool
             */
            void stopServer();

        private:

            /**
             * @brief Binds the server to the port
             */
            void bindSocket();

            /**
             * @brief Handles the client at the given file descriptor
             * 
             * @param fd The file descriptor attache dto the connected client
             */
            void handleClient(int fd);

            /**
             * @brief The maximum length of the buffer for a HTTP message read or write
             */
            const unsigned int BUFFER_LENGTH = 10 * 1024 * 1024;

            /**
             * @brief The endpoints to also look for and execute on
             */
            std::map<endpoint, std::function<nlohmann::json(nlohmann::json)>> endpoints;
            
            /**
             * @brief The port to listen to connections on
             */
            unsigned short listenPort;

            /**
             * @brief The threadpool to handle clients on
             */
            ThreadPool* pool;

            /**
             * @brief The number of worker threads to use for handling clients
             */
            unsigned int numWorkerThreads;

            /**
             * @brief The flag used to stop the server
             */
            volatile bool run;

            /**
             * @brief The file descriptor for the socket to listen for connections
             */
            int serverSocket;

            /**
             * @brief The socket address of ther server
             */
            struct sockaddr_in serverAddress;

            /**
             * @brief The directory to serve static files from
             */
            std::string staticDirectory;

            /**
             * @brief The file path of the Certificate File for SSL
             */
            std::string SSL_CertFile;

            /**
             * @brief The file path of the Key File for SSL
             */
            std::string SSL_KeyFile;

            /**
             * @brief The OpenSSL context to handle the SSL encryption
             */
            SSL_CTX* sslCtx;
    };
};

#endif /*__LIGHT_HTTP_SERVER_HPP__*/