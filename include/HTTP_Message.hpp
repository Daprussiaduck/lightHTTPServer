#ifndef __HTTP_MESSAGE_HPP__
#define __HTTP_MESSAGE_HPP__

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <nlohmann/json.hpp>

#include "HTTP_StatusCodes.hpp"
#include "HTTP_MimeTypes.hpp"
#include "HTTP_Methods.hpp"

// TODO: Query parameters
namespace lightHTTPServer {
    static const std::string HTTP_VERSION = "HTTP/1.1";
    /**
     * An attempt to conform to standard HTTP specification
     * https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides/Messages
     */
    class HTTP_Message {
        public:
            HTTP_Message(std::string startLine = "", std::map<std::string, std::string> headers = std::map<std::string, std::string>(), char* body = nullptr);

            ~HTTP_Message();
            
            void addHeader(std::string headerName, std::string headerContent);

            void addHeaders(std::map<std::string, std::string> headers);
            
            char* getBody();

            int getBodyLength();
            
            std::string getHeader(std::string headerName);
            
            std::map<std::string, std::string> getHeaders();

            std::string getMetaData();

            int getMethod();

            std::string getStartLine();

            std::string getURI();

            bool parseHTTPRequest(char* buffer);

            nlohmann::json parseHTTPRequestJSON(char* buffer);

            void replaceHeaders(std::map<std::string, std::string> headers);
            
            void setBody(char* body, int len, std::string mimeType);

            void setBody(std::string fileName);

            void setBody(nlohmann::json json);

            void setMethod(int method);

            void setStatusCode(int statusCode);

            void setURI(std::string uri, int method = HTTP_Method::GET);

            std::string toString();
        private:
            int bodyLength = 0;
            int HTTP_Method;
            std::string HTTP_StartLine;
            std::map<std::string, std::string> HTTP_Headers;
            char* HTTP_Body;
            bool read;
    };
};

#endif /*__HTTP_MESSAGE_HPP__*/