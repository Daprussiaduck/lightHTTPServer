#include "lightHTTPServer.hpp"

namespace lightHTTPServer {
    Server::Server(short port, int numThreads, std::string certFileName, std::string keyFileName){
        this -> listenPort = port;
        this -> numWorkerThreads = numThreads;
        if (this -> numWorkerThreads < 0){
            this -> numWorkerThreads = std::thread::hardware_concurrency();
        }
        this -> SSL_CertFile = certFileName;
        this -> SSL_KeyFile = keyFileName;
        this -> run = true;
    };

    Server::~Server(){
        if (this -> pool != nullptr){
            pool -> stop();
            delete pool;
        }
        std::cout << "Deleted Server" << std::endl;
    }

    void Server::addEndpoint(int method, std::string uri, const std::function<nlohmann::json()> &endpointTask){
        this -> endpoints[endpoint(method, uri)] = endpointTask;
    }

    void Server::bindSocket(){
        signal(SIGPIPE, SIG_IGN);
        this -> serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (this -> serverSocket < 0){
            std::cerr << "Unable to create socket" << std::endl;
            exit(EXIT_FAILURE);
        }
        memset(&(this -> serverAddress), 0x00, sizeof(this -> serverAddress));
        this -> serverAddress.sin_family = AF_INET;
        this -> serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        this -> serverAddress.sin_port = htons(this -> listenPort);
        if (bind(this -> serverSocket, (struct sockaddr*)&(this -> serverAddress), sizeof(this -> serverAddress)) < 0){
            std::cerr << "Unable to bind to the socket" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (listen(this -> serverSocket, this -> numWorkerThreads) < 0){
            std::cerr << "Unable to listen to the socket" << std::endl;
            exit(EXIT_FAILURE);
        }
        const SSL_METHOD* sslMethod = TLS_server_method();
        this -> sslCtx = SSL_CTX_new(sslMethod);
        if (!(this -> sslCtx)){
            std::cerr << "Unable to create SSL context" << std::endl;
            ERR_print_errors_fp(stderr);
            exit(EXIT_FAILURE);
        }
        if (SSL_CTX_use_certificate_file(this -> sslCtx, (this -> SSL_CertFile).c_str(), SSL_FILETYPE_PEM) <= 0) {
            ERR_print_errors_fp(stderr);
            exit(EXIT_FAILURE);
        }
        if (SSL_CTX_use_PrivateKey_file(this -> sslCtx, (this -> SSL_KeyFile).c_str(), SSL_FILETYPE_PEM) <= 0 ) {
            ERR_print_errors_fp(stderr);
            exit(EXIT_FAILURE);
        }
    }

    void Server::handleClient(int fd){
        SSL* ssl;
        ssl = SSL_new(sslCtx);
        SSL_set_fd(ssl, fd);
        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
        } else {
            char* buffer = new char[BUFFER_LENGTH];
            if (SSL_read(ssl, buffer, BUFFER_LENGTH) < 0){
                ERR_print_errors_fp(stderr);
            } else {
                // Read the message
                HTTP_Message mess;
                mess.parseHTTPRequest(buffer);
                // Respond to the message
                memset(buffer, 0x00, this -> BUFFER_LENGTH);
                HTTP_Message resp;
                int reqMethod = mess.getMethod();
                std::cout << HTTP_Method_Strings.at(reqMethod) << " request for: " << mess.getURI() << std::endl;
                if (reqMethod == HTTP_Method::GET){
                    std::string sendFileName = (this -> staticDirectory) + mess.getURI();
                    if (mess.getURI().compare("/") == 0){
                        sendFileName = (this -> staticDirectory) + "/index.html";
                    }
                    if (std::filesystem::exists(sendFileName)){
                        std::cout << "File exists" << std::endl;
                        resp.setStatusCode(HTTP_RESPONSE_CODES::HTTP_STATUS_OK);
                        resp.setBody(sendFileName);
                        std::string respStr = resp.toString();
                        strcpy(buffer, respStr.c_str());
                        memcpy(buffer + respStr.length(), resp.getBody(), resp.getBodyLength());
                        SSL_write(ssl, buffer, (respStr.length() + resp.getBodyLength()));
                    } else if ((this -> endpoints).contains(std::make_pair(mess.getMethod(), mess.getURI()))){
                        std::cout << "GET Endpoint exists" << std::endl;
                        nlohmann::json retJSON = (this -> endpoints)[std::make_pair(mess.getMethod(), mess.getURI())]();
                        resp.setStatusCode(HTTP_RESPONSE_CODES::HTTP_STATUS_OK);
                        resp.setBody(retJSON);
                        std::string respStr = resp.toString();
                        strcpy(buffer, respStr.c_str());
                        strcpy(buffer + respStr.length(), resp.getBody());
                        SSL_write(ssl, buffer, respStr.length() + resp.getBodyLength());
                    } else {
                        std::cout << "Not Found" << std::endl;
                        std::string notFoundFileName = this -> staticDirectory + "/" + "404.html";
                        resp.setStatusCode(HTTP_RESPONSE_CODES::HTTP_STATUS_NOT_FOUND);
                        std::string respStr = resp.toString();
                        strcpy(buffer, respStr.c_str());
                        if (std::filesystem::exists(notFoundFileName)){
                            std::cout << "Nout Found File exists" << std::endl;
                            resp.setBody(notFoundFileName);
                            memcpy(buffer + respStr.length(), resp.getBody(), resp.getBodyLength());
                        }
                        SSL_write(ssl, buffer, (respStr.length() + resp.getBodyLength()));
                    }
                } else {
                    // std::cout << "Method: " << HTTP_Method_Strings.at(mess.getMethod()) << ", URI: " << mess.getURI() << std::endl;
                    // for (std::map<endpoint, std::function<nlohmann::json()>>::iterator i = this -> endpoints.begin(); i != this -> endpoints.end(); i++){
                    //     std::cout << "\t" << i-> first.first << " : " << i -> first.second << std::endl;
                    // }
                    if ((this -> endpoints).contains(std::make_pair(mess.getMethod(), mess.getURI()))){
                        std::cout << HTTP_Method_Strings.at(reqMethod) << " Endpoint exists" << std::endl;
                        nlohmann::json retJSON = (this -> endpoints)[std::make_pair(mess.getMethod(), mess.getURI())]();
                        resp.setStatusCode(HTTP_RESPONSE_CODES::HTTP_STATUS_OK);
                        resp.setBody(retJSON);
                        std::string respStr = resp.toString();
                        strcpy(buffer, respStr.c_str());
                        strcpy(buffer + respStr.length(), resp.getBody());
                        SSL_write(ssl, buffer, respStr.length() + resp.getBodyLength());
                    } else {
                        std::cout << HTTP_Method_Strings.at(reqMethod) << " " << mess.getURI() << " Not Found" << std::endl;
                        std::string notFoundFileName = this -> staticDirectory + "/" + "501.html";
                        resp.setStatusCode(HTTP_RESPONSE_CODES::HTTP_STATUS_NOT_IMPLEMENTED);
                        std::string respStr = resp.toString();
                        strcpy(buffer, respStr.c_str());
                        if (std::filesystem::exists(notFoundFileName)){
                            std::cout << "Nout Found File exists" << std::endl;
                            resp.setBody(notFoundFileName);
                            memcpy(buffer + respStr.length(), resp.getBody(), resp.getBodyLength());
                        }
                        SSL_write(ssl, buffer, (respStr.length() + resp.getBodyLength()));
                    }
                }
            }
            delete buffer;
        }
        // std::cout << "Client Handled" << std::endl;
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(fd);
    }

    // TODO: Figure out why this hangs when server should be stopping
    // TODO: Use handle client and a branching poll structure?
    void Server::runServer(){
        this -> bindSocket();
        this -> pool = new ThreadPool(this -> numWorkerThreads);
        std::vector<pollfd> pinkPonyClub;
        pollfd acceptPoll;
        memset(&acceptPoll, 0x00, sizeof(pollfd));
        acceptPoll.fd = this -> serverSocket;
        acceptPoll.events = POLLIN;
        pinkPonyClub.emplace_back(acceptPoll);
        while (this -> run){
            int pollRet = poll(pinkPonyClub.data(), pinkPonyClub.size(), LIGHT_HTTP_SERVER_POLLING_TIMEOUT);
            if (pollRet < 0){
                if (this -> run){
                    std::cerr << "Error Polling fd" << std::endl;
                    return;
                    exit(EXIT_FAILURE);
                }
            } else if (pollRet == 0){
                continue;
            } else {
                for (pollfd polled : pinkPonyClub){
                    if (polled.fd == (this -> serverSocket) && (this -> run)){
                        struct sockaddr_in clientAddr;
                        unsigned int clientLen = sizeof(clientAddr);
                        pollfd newClient;
                        memset(&newClient, 0x00, sizeof(pollfd));
                        newClient.fd = accept(this -> serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
                        newClient.events = POLLOUT;
                        //pinkPonyClub.emplace_back(newClient);
                        this -> pool -> addTask([this, newClient]{
                            this -> handleClient(newClient.fd);
                        });
                    } else { // We shouldn't be here
                        // this -> handleClient(polled.fd);
                    }
                }
            }
        }
        close(this -> serverSocket);
        SSL_CTX_free(this -> sslCtx);
    }

    bool Server::setStaticDirectory(std::string staticDir){
        if (std::filesystem::exists(staticDir)){
            this -> staticDirectory = staticDir;
            return true;
        }
        std::cerr << "Failed to find the directory: " << staticDir << std::endl;
        return false;
    }

    void Server::stopServer(){
        this -> run = false;
        if (this -> pool != nullptr){
            this -> pool -> stop();
            delete this -> pool;
            this -> pool = nullptr; 
            std::cout << "Pool Stopped" << std::endl;
        }
        // close(this -> serverSocket);
        // SSL_CTX_free(this -> sslCtx);
        std::cout << "Server Stopped: " << (this -> run ? "false" : "true") <<  std::endl;
    }
};