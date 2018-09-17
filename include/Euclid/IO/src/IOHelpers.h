#pragma once

#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

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

inline std::vector<std::string_view>
split(std::string_view str, char delim, std::string_view::size_type start = 0)
{
    std::vector<std::string_view> substrs;
    if (!str.empty()) {
        std::string_view::size_type beg = start;
        std::string_view::size_type pos = str.find(delim, beg);
        while (pos != std::string_view::npos) {
            if (beg != pos) { substrs.push_back(str.substr(beg, pos - beg)); }
            beg = pos + 1;
            pos = str.find(delim, beg);
        }
        if (beg < str.size()) { substrs.push_back(str.substr(beg, pos - beg)); }
    }
    return substrs;
}

} // namespace _impl

} // namespace Euclid
