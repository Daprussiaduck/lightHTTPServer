#include "HTTP_Message.hpp"

namespace lightHTTPServer {
    HTTP_Message::HTTP_Message(std::string startLine, std::map<std::string, std::string> headers, char* body){
        this -> HTTP_StartLine = startLine;
        this -> HTTP_Headers = headers;
        this -> HTTP_Body = body;
        read = false;
    }

    HTTP_Message::~HTTP_Message(){
        if (read){
            delete this -> HTTP_Body;
        }
    }

    void HTTP_Message::addHeader(std::string headerName, std::string headerValue){
        this -> HTTP_Headers.emplace(headerName, headerValue);
    }

    void HTTP_Message::addHeaders(std::map<std::string, std::string> headers){
        this -> HTTP_Headers.merge(headers);
    }

    char* HTTP_Message::getBody(){
        return this -> HTTP_Body;
    }

    int HTTP_Message::getBodyLength(){
        return this -> bodyLength;
    }

    std::string HTTP_Message::getHeader(std::string headerName){
        if (this -> HTTP_Headers.contains(headerName)){
            return this -> HTTP_Headers.at(headerName);
        }
        return "";
    }

    std::map<std::string, std::string> HTTP_Message::getHeaders(){
        return this -> HTTP_Headers;
    }

    std::string HTTP_Message::getMetaData(){
        std::string ret = "";
        if (!(this -> HTTP_StartLine.ends_with("\r\n"))){
            this -> HTTP_StartLine += "\r\n";
        }
        ret += HTTP_StartLine;
        for (std::map<std::string, std::string>::iterator i = this -> HTTP_Headers.begin(); i != this -> HTTP_Headers.end(); i++){
            ret += i-> first + ": " + i -> second + "\r\n";
        }
        ret += "\r\n";
        return ret;
    }

    int HTTP_Message::getMethod(){
        return this -> HTTP_Method;
    }

    nlohmann::json HTTP_Message::getQueryParameters(){
        nlohmann::json ret = nlohmann::json::parse("{}");
        if (this -> HTTP_QueryStr.compare("") != 0 && (this -> getMethod() == HTTP_Method::GET || this -> getMethod() == HTTP_Method::POST)){
            std::string temp = this -> HTTP_QueryStr;
            int eqPos = this -> HTTP_QueryStr.find_first_of("=");
            while (eqPos != std::string::npos){
                std::string parameterName = temp.substr(0, eqPos);
                temp.erase(0, eqPos + 1);
                int ampersandPos = temp.find_first_of("&");
                std::string parameterValue = temp.substr(0, ampersandPos);
                temp.erase(0, ampersandPos == std::string::npos ? std::string::npos : ampersandPos + 1);
                ret[parameterName] = parameterValue;
                eqPos = temp.find_first_of("=");
            }
        } else if (this -> getMethod() == HTTP_Method::POST){
            if (this -> getBody() != nullptr){
                if (this -> getHeader("Content-Type").compare("application/x-www-form-urlencoded") == 0){
                    this -> HTTP_QueryStr = this -> getBody();
                    return this -> getQueryParameters();
                } else if (this -> getHeader("Content-Type").contains("multipart/form-data")){
                    std::string contentTypeHeader = this -> getHeader("Content-Type");
                    // TODO: Actually handle this
                    throw std::logic_error("Not yet fully implemented yet.");
                    // if (this -> getHeader("Content-Type").contains("boundary")){
                    //     std::string boundaryKey = "--" + contentTypeHeader.substr(contentTypeHeader.find("boundary=") + 10);
                    //     nlohmann::json ret = nlohmann::json::parse("{}");
                    //     std::string bodyString = this -> getBody();
                    //     while (bodyString.find(boundaryKey) != -1){
                    //         bodyString.erase(0, boundaryKey.length());
                    //         if (bodyString.length() > 0){
                    //             std::string ContentDisposition = bodyString.substr(0, bodyString.find_first_of("\r\n\r\n"));
                    //             bodyString.erase(0, ContentDisposition.length() + 4);

                    //         }
                    //     }
                    // } else {
                    // }
                } else if (this -> getHeader("Content-Type").compare("application/json") == 0){
                    return nlohmann::json::parse(this -> getBody());
                } else {
                    // TODO: What am I supposed to do here? Cry?
                }
            }
        }
        return ret;
    }

    std::string HTTP_Message::getStartLine(){
        return this -> HTTP_StartLine;
    }

    std::string HTTP_Message::getURI(){
        std::string methodString = HTTP_Method_Strings[this -> getMethod()];
        int uriLength = (this -> getStartLine()).length() - (4 + methodString.length() + HTTP_VERSION.length());
        return (this -> getStartLine()).substr(methodString.length() + 1, uriLength);
    }

    bool HTTP_Message::parseHTTPRequest(char* buffer){
        std::string bufferStr(buffer);
        if (bufferStr.length() < 15){
            std::cout << "Not a HTTP Request: " << buffer << std::endl;
            return false;
        }
        unsigned int length = 0;
        this -> HTTP_StartLine = bufferStr.substr(0, bufferStr.find_first_of("\r\n") + 2);
        int qStringPos = this -> HTTP_StartLine.find_first_of("?");
        if (qStringPos != std::string::npos){
            this -> HTTP_QueryStr = this -> HTTP_StartLine.substr(qStringPos + 1);
            int spacePos = this -> HTTP_QueryStr.find_last_of(" ");
            this -> HTTP_QueryStr = this -> HTTP_QueryStr.substr(0, spacePos);
            this -> HTTP_StartLine = this -> HTTP_StartLine.substr(0, qStringPos);
        }
        // std::cout << "HTTP Request Start Line: |" << this -> HTTP_StartLine << "|" << std::endl;
        length += this -> HTTP_StartLine.length();
        std::string method = this -> HTTP_StartLine.substr(0, this -> HTTP_StartLine.find_first_of(" "));
        for (int i = HTTP_Method::GET; i <= HTTP_Method::CONNECT; i++){
            // std::cout << "Checking method " << i << " " << method << std::endl;
            if (method.compare(HTTP_Method_Strings.at(i)) == 0){
                //std::cout << "found method " << i << std::endl;
                this -> HTTP_Method = i;
                break;
            }
        }
        bufferStr.erase(0, bufferStr.find_first_of("\r\n") + 2);
        this -> HTTP_Headers.clear();
        while (!bufferStr.starts_with("\r\n") && !bufferStr.empty()){
            std::string header = bufferStr.substr(0, bufferStr.find_first_of("\r\n") + 2);
            //std::cout << "HTTP Request Header: |" << header << "|" << std::endl;
            length += header.length();
            std::string headerName = header.substr(0, bufferStr.find_first_of(": "));
            std::string headerValue = header.substr(bufferStr.find_first_of(": ") + 2);
            if (headerName.compare("Content Length") == 0){
                this -> bodyLength = std::stoi(headerValue);
            }
            bufferStr.erase(0, bufferStr.find_first_of("\r\n") + 2);
        }
        length += 2;
        this -> HTTP_Body = (char*)(buffer + length);
        // std::cout << "HTTP Request: |" << this -> HTTP_Body << "|" << std::endl;
        return true;
    }

    nlohmann::json HTTP_Message::parseHTTPRequestJSON(char* buffer){
        bool ret = this -> parseHTTPRequest(buffer);
        if (ret){
            return nlohmann::json::parse(this -> HTTP_Body);
        }
        return nlohmann::json::parse("{}");
    }

    void HTTP_Message::replaceHeaders(std::map<std::string, std::string> headers){
        this -> HTTP_Headers.clear();
        this -> HTTP_Headers.merge(headers);
    }

    void HTTP_Message::setBody(char* body, int len, std::string mimeType){
        this -> HTTP_Body = body;
        if (this -> HTTP_Headers.contains("Content-Type")){
            this -> HTTP_Headers.erase("Content-Type");
        }
        this -> HTTP_Headers.emplace("Content-Type", mimeType);
        if (this -> HTTP_Headers.contains("Content-Length")){
            this -> HTTP_Headers.erase("Content-Length");
        }
        this -> bodyLength = len;
        this -> HTTP_Headers.emplace("Content-Length", std::to_string(len));
    }

    void HTTP_Message::setBody(std::string fileName){
        std::ifstream bodyInput;
        bodyInput.open(fileName, std::ifstream::binary);
        if (bodyInput.good()){
            bodyInput.seekg (0, bodyInput.end);
            this -> bodyLength = bodyInput.tellg();
            bodyInput.seekg (0, bodyInput.beg);
            char* temp = new char[this -> bodyLength];
            bodyInput.read(temp, this -> bodyLength);
            this -> read = true;
            std::string fileExt = fileName.substr(fileName.find_last_of("."));
            if (fileExt.compare("") == 0 || !(HTTP_MimeTypes.contains(fileExt))){
                fileExt = ".unknown";
            }
            this -> setBody(temp, this -> bodyLength, HTTP_MimeTypes.at(fileExt));
        }
    }

    void HTTP_Message::setBody(nlohmann::json json){
        this -> read = true;
        std::string jsonStr = json.dump();
        char* temp = new char[jsonStr.length()];
        strcpy(temp, jsonStr.c_str());
        this -> setBody(temp, jsonStr.length(), HTTP_MimeTypes.at(".json"));
    }

    void HTTP_Message::setMethod(int method){
        this -> HTTP_Method = method;
        if (this -> HTTP_StartLine.compare("") == 0){
            this -> setURI("/", this -> HTTP_Method);
            return;
        }
        int space = this -> HTTP_StartLine.find_first_of(" ");
        this -> HTTP_StartLine = HTTP_Method_Strings.at(this -> HTTP_Method) + this -> HTTP_StartLine.substr(space);
    }

    void HTTP_Message::setStatusCode(int statusCode){
        this -> HTTP_StartLine = HTTP_VERSION + " " + std::to_string(statusCode) + " " + HTTP_STATUS_CODE_STRINGS.at(statusCode) + "\r\n";
    }

    void HTTP_Message::setURI(std::string uri, int method){
        this -> HTTP_Method = method;
        this -> HTTP_StartLine = HTTP_Method_Strings.at(this -> HTTP_Method) + " " + uri + " " + HTTP_VERSION;
    }

    std::string HTTP_Message::toString(){
        return this -> getMetaData();
    }
};