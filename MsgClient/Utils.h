#pragma once
#include <string>

class Utils
{
public:
    Utils();
    ~Utils();

    static std::string G2U(const std::string & str);
    static std::string U2G(const std::string & str);
};

