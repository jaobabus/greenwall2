#ifndef PROFILE_H
#define PROFILE_H

#include <map>
#include <variant>
#include <string>
#include <chrono>
#include <functional>
#include <sstream>




class ProfileMap : std::map<std::string, std::variant<std::chrono::nanoseconds, std::string>>
{
public:
    ProfileMap() {}

    std::string format() {
        std::ostringstream oss;
        for (auto& [k, v] : *this) {
            oss << k << ":" << format(v) << ",";
        }
        return oss.str();
    }

    std::string format(std::variant<std::chrono::nanoseconds, std::string> v) {
        std::string res;
        std::visit([&](auto v) {
                       res = format(v);
                   }, v);
        return res;
    }

    std::string format(std::chrono::nanoseconds v) {
        auto s = std::to_string(v.count());
        if (v < std::chrono::nanoseconds(10000))
            return std::string(5 - s.size(), ' ') + s + "ns";
        if (v < std::chrono::nanoseconds(1000000))
            return std::string(6 - s.size(), ' ') + s.substr(0, s.size() - 3) + '.' + s.substr(s.size() - 3, 1) + "us";
        if (v < std::chrono::nanoseconds(1000000000))
            return std::string(9 - s.size(), ' ') + s.substr(0, s.size() - 6) + '.' + s.substr(s.size() - 6, 1) + "ms";
        if (v < std::chrono::nanoseconds(10000000000000))
            return std::string(13 - s.size(), ' ') + s.substr(0, s.size() - 9) + '.' + s.substr(s.size() - 9, 1) + "s";
        return s.substr(0, s.size() - 9) + '.' + s.substr(s.size() - 9, 1) + "s";
    }

    std::string format(std::string const& s) {
        if (s.size() < 5)
            return std::string(5 - s.size(), ' ') + s;
        if (s.size() < 10)
            return std::string(10 - s.size(), ' ') + s;
        return s;
    }

    void profile(const char* name, std::function<void()> cb) {
        auto s = std::chrono::steady_clock::now();
        cb();
        auto e = std::chrono::steady_clock::now();
        (*this)[name] = (e - s);
    }

    void profile(const char* name, std::string const& s) {
        (*this)[name] = s;
    }

    void profile(const char* name, int s) {
        (*this)[name] = std::to_string(s);
    }

};


#endif // PROFILE_H
