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

namespace lightHTTPServer {
    /**
     * @brief The version of HTTP we are using (HTTP version 1.1)
     */
    static const std::string HTTP_VERSION = "HTTP/1.1";
    
    /**
     * @brief An attempt to conform to standard HTTP specification
     * https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides/Messages
     */
    class HTTP_Message {
        public:
            /**
             * @brief Constructs a new HTTP message
             * 
             * @param startLine The HTTP Start Line of the HTTP Message
             * @param headers The HTTP headers of the HTTP Message
             * @param body Pointer to the body data of the HTTP Message
             */
            HTTP_Message(std::string startLine = "", std::map<std::string, std::string> headers = std::map<std::string, std::string>(), char* body = nullptr);

            /**
             * @brief Deconstructs a HTTP message
             */
            ~HTTP_Message();
            
            /**
             * @brief Adds a Header to the HTTP Message
             * 
             * @param headerName The HTTP compliant name of the header
             * @param headerContent The value to set for the header 
             */
            void addHeader(std::string headerName, std::string headerContent);

            /**
             * @brief Adds multiple headers to the HTTP Message
             * 
             * @param headers The headers to add to the HTTP Message
             */
            void addHeaders(std::map<std::string, std::string> headers);
            
            /**
             * @brief Returns the body of the HTTP Message
             * 
             * @returns Pointer to the body of the HTTP Message
             */
            char* getBody();

            /**
             * @brief Gets the length of the body stored in the HTTP Message
             * 
             * @return The length of the HTTP Message body
             */
            int getBodyLength();
            
            /**
             * @brief Returns the content of the header in the HTTP Message
             * 
             * @param headerName The name of the Header to get the value of
             * @return The value of the specified header
             */
            std::string getHeader(std::string headerName);
            
            /**
             * @brief Returns all Headers of the HTTP Message
             * 
             * @return A map of the HTTP headers in the HTTP Message
             */
            std::map<std::string, std::string> getHeaders();

            /**
             * @brief Gets the MetaData of the HTTP Message
             * 
             * @brief Returns the HTTP headers as a string
             */
            std::string getMetaData();

            /**
             * @brief Gets the HTTP Method the HTTP Message has
             * 
             * @return The HTTP Header that the HTTP Message has
             */
            int getMethod();

            /**
             * @brief Returns the Query parameters the HTTP message has
             * 
             * @return The query parameters in the HTTP Message as a JSON object 
             */
            nlohmann::json getQueryParameters();

            /**
             * @brief Returns the Start line in the HTTP Message
             * 
             * @return The HTTP Start Line in the HTTP Message
             */
            std::string getStartLine();

            /**
             * @brief Returns the URI of the HTTP Message
             * 
             * @return the URI of the HTTP Message
             */
            std::string getURI();

            /**
             * @brief Parses a HTTP Message from a HTTP request
             * 
             * @param buffer The buffer to parse
             * @return True if successful, false if not
             */
            bool parseHTTPRequest(char* buffer);

            /**
             * @brief Parses the JSON out of a HTTP Message
             * 
             * @param buffer The buffer to parse
             * @return The JSON parsed from the buffer
             */
            nlohmann::json parseHTTPRequestJSON(char* buffer);

            /**
             * @brief Replaces the HTTP headers with the ones provided
             * 
             * @param headers The headers to place into the HTTP Message
             */
            void replaceHeaders(std::map<std::string, std::string> headers);
            
            /**
             * @brief Sets the body of the HTTP Message to the given buffer
             * 
             * @param body Pointer to the data of the body
             * @param len The length of the body to attach
             * @param mimeType The Mime Type of the body
             */
            void setBody(char* body, int len, std::string mimeType);

            /**
             * @brief Sets the body to the given file path
             * 
             * @param filename The file to load into the body
             */
            void setBody(std::string fileName);

            /**
             * @brief Sets the body to the given JSON
             * 
             * @param json The JSON to set the body to
             */
            void setBody(nlohmann::json json);

            /**
             * @brief Sets the HTTP method of the HTTP Message
             * 
             * @param method The method to set the HTTP Message
             */
            void setMethod(int method);

            /**
             * @brief Sets the status code of the HTTP Message
             * 
             * @param statusCode The status code to set the HTTP Message to
             */
            void setStatusCode(int statusCode);

            /**
             * @brief Sets the URI of the HTTP Message
             * 
             * @param uri The URI to set
             * @param method The HTTP Method to use 
             */
            void setURI(std::string uri, int method = HTTP_Method::GET);

            /**
             * @brief Returns the HTTP Message as a string
             * 
             * @return The HTTP Message as a string
             */
            std::string toString();

        private:
            /**
             * @brief The length of the body of the HTTP Message
             */
            int bodyLength = 0;

            /**
             * @brief The HTTP Method of the HTTP Message
             */
            int HTTP_Method = HTTP_Method::GET;

            /**
             * @brief The Query String of the HTTP Message
             */
            std::string HTTP_QueryStr = "";

            /**
             * @brief The Start Line of the HTTP Message
             */
            std::string HTTP_StartLine = "";

            /**
             * @brief The HTTP Headers of the HTTP Message
             */
            std::map<std::string, std::string> HTTP_Headers;
            
            /**
             * @brief Pointer to the data of the HTTP Message's body
             */
            char* HTTP_Body = nullptr;

            /**
             * @brief Weather or not we have read data into this HTTP Method object
             */
            bool read = false;
    };
};

#endif /*__HTTP_MESSAGE_HPP__*/