#include "utils.h"

#include <stdarg.h>
#include <iostream>



ColorFloat to_float(ColorRGBA color)
{
    return ColorFloat{float(color.r / 255.0f), float(color.g / 255.0f), float(color.b / 255.0f), float(color.a / 255.0f)};
}

ColorRGBA to_rgba(ColorFloat color)
{
    return ColorRGBA{uint8_t(color.r * 255.0f), uint8_t(color.g * 255.0f), uint8_t(color.b * 255.0f), uint8_t(color.a * 255.0f)};
}

std::string to_hex(ColorRGBA color)
{
    std::string buf;
    buf.resize(8);
    sprintf(&buf[0], "%2X%2X%2X%2X", color.r, color.g, color.b, color.a);
    return buf;
}


std::ostream& operator<<(std::ostream& os, Vector2i const& v)
{
    os << "I2{" << v.x << ", " << v.y << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, Vector2u const& v)
{
    os << "U2{" << v.x << ", " << v.y << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, ColorRGBA const& v)
{
    return os << "#" << to_hex(v);
}

std::ostream& operator<<(std::ostream& os, ColorFloat const& v)
{
    return os << "#" << to_hex(to_rgba(v));
}

ssize_t ssprintf(std::string& res, const char* format, ...)
{
    va_list list;
    va_start(list, format);
    auto s = vssprintf(res, format, list);
    va_end(list);
    return s;
}

ssize_t vssprintf(std::string& res, const char* format, va_list list)
{
    char buf[4096];
    auto s = vsnprintf(buf, sizeof(buf), format, list);
    res += buf;
    return s;
}

uint8_t from_hex(char h) {
    return (h >= '0' and h <= '9' ? h - '0' :
            h >= 'a' and h <= 'f' ? h - 'a' + 0xa :
            h >= 'A' and h <= 'F' ? h - 'A' + 0xA : 0);
}

uint8_t from_hex(char h1, char h2) {
    return (from_hex(h1) << 4) | from_hex(h2);
}

ColorRGBA from_hex(const std::string& color)
{
    std::string r, g, b, a = "FF";
    if (color.size() > 4) {
        r = color.substr(0, 2);
        g = color.substr(2, 2);
        b = color.substr(4, 2);
        if (color.size() > 6)
            a = color.substr(6, 2);
    }
    else {
        r = std::string(2, color[0]);
        g = std::string(2, color[1]);
        b = std::string(2, color[2]);
        if (color.size() > 3)
           a = std::string(2, color[3]);
    }
    return ColorRGBA{from_hex(r[0], r[1]), from_hex(g[0], g[1]), from_hex(b[0], b[1]), from_hex(a[0], a[1])};
}

ColorFloat mix(ColorFloat c1, ColorFloat c2, float fact)
{
    return c1 * fact + c2 * (1 - fact);
}

ColorFloat operator+(ColorFloat c1, ColorFloat c2)
{
    return ColorFloat{c1.r + c2.r, c1.g + c2.g, c1.b + c2.b, c1.a + c2.a};
}

ColorFloat operator-(ColorFloat c1, ColorFloat c2)
{
    return ColorFloat{c1.r - c2.r, c1.g - c2.g, c1.b - c2.b, c1.a - c2.a};
}

ColorFloat operator*(ColorFloat c1, float c2)
{
    return ColorFloat{c1.r * c2, c1.g * c2, c1.b * c2, c1.a};
}

ColorFloat operator/(ColorFloat c1, float c2)
{
    return ColorFloat{c1.r / c2, c1.g / c2, c1.b / c2, c1.a};
}
