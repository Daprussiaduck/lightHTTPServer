#ifndef __HTTP_METHODS_HPP__
#define __HTTP_METHODS_HPP__

#include <string>
#include <map>

namespace lightHTTPServer{
    enum HTTP_Method {
        GET = 0,
        HEAD = 1,
        OPTIONS = 2,
        TRACE = 3,
        PUT = 4,
        DELETE = 5,
        POST = 6,
        PATCH = 7,
        CONNECT = 8
    };

    static std::map<int, std::string> HTTP_Method_Strings = {
        {0, "GET"},
        {1, "HEAD"},
        {2, "OPTIONS"},
        {3, "TRACE"},
        {4, "PUT"},
        {5, "DELETE"},
        {6, "POST"},
        {7, "PATCH"},
        {8, "CONNECT"},
    };
};

#endif /*__HTTP_METHODS_HPP__*/