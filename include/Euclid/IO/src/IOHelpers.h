#pragma once

#include <exception>
#include <fstream>
#include <iostream>
#include <string>

namespace Euclid
{

namespace _impl
{

template<typename CharT>
void check_fstream(std::basic_ifstream<CharT>& stream,
                   const std::string& filename)
{
    if (!stream.is_open()) {
        std::string err_str("Can't open file ");
        err_str.append(filename);
        throw std::runtime_error(err_str);
    }
}

template<typename CharT>
void check_fstream(std::basic_ofstream<CharT>& stream,
                   const std::string& filename)
{
    if (!stream.is_open()) {
        std::string err_str("Can't open file ");
        err_str.append(filename);
        throw std::runtime_error(err_str);
    }
}

} // namespace _impl

} // namespace Euclid
