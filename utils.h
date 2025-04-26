#ifndef UTILS_H
#define UTILS_H


#include <string>
#include <string.h>




template<typename T>
struct Vector2
{
    template<typename D>
    explicit Vector2(Vector2<D> r) : x(r.x), y(r.y) {}
    explicit Vector2(T x) : x(x), y(x) {}
    Vector2() : x(0), y(0) {}
    Vector2(T x, T y) : x(x), y(y) {}

    T x, y;
};


using Vector2i = Vector2<int>;
using Vector2u = Vector2<unsigned int>;
using Vector2f = Vector2<float>;

template<typename T>
bool operator==(Vector2<T> a, Vector2<T> b) {
    return a.x == b.x and a.y == b.y;
}
template<typename T>
bool operator!=(Vector2<T> a, Vector2<T> b) {
    return not (a == b);
}


struct ColorRGBA
{
    uint8_t r, g, b, a;
};

struct ColorFloat
{
    float r, g, b, a;
};

template<typename T>
bool eq_rgb(T c1, T c2)
{
    return c1.r == c2.r and c1.g == c2.g and c1.b == c2.b;
}
ColorFloat mix(ColorFloat c1, ColorFloat c2, float fact);


ColorFloat to_float(ColorRGBA color);
ColorRGBA to_rgba(ColorFloat color);
std::string to_hex(ColorRGBA color);
ColorRGBA from_hex(std::string const& color);

ColorFloat operator+(ColorFloat c1, ColorFloat c2);
ColorFloat operator-(ColorFloat c1, ColorFloat c2);
ColorFloat operator*(ColorFloat c1, float c2);
ColorFloat operator/(ColorFloat c1, float c2);

std::ostream& operator<<(std::ostream& os, Vector2i const& v);
std::ostream& operator<<(std::ostream& os, Vector2u const& v);
std::ostream& operator<<(std::ostream& os, ColorRGBA const& v);
std::ostream& operator<<(std::ostream& os, ColorFloat const& v);

ssize_t ssprintf(std::string& res, const char* format, ...);
ssize_t vssprintf(std::string& res, const char* format, va_list);



#endif // UTILS_H
