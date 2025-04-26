#ifndef MAP2D_H
#define MAP2D_H


#include <vector>
#include "utils.h"




template<typename T>
class Vector2D : public std::vector<T>
{
public:
    Vector2D(Vector2u size = {0, 0}, T const& def = {})
        : _size(size), std::vector<T>(size.x * size.y, def) {}

    void resize(Vector2u size, T const& def = {}) {
        _size = size;
        this->std::vector<T>::resize(size.x * size.y, def);
    }

    T& at(Vector2u pos) {
        return this->std::vector<T>::at(pos.x + _size.x * pos.y);
    }
    const T& at(Vector2u pos) const {
        return this->std::vector<T>::at(pos.x + _size.x * pos.y);
    }

    T& operator[](Vector2u pos) {
        return this->std::vector<T>::operator[](pos.x + _size.x * pos.y);
    }
    const T& operator[](Vector2u pos) const {
        return this->std::vector<T>::operator[](pos.x + _size.x * pos.y);
    }

    Vector2u size2() const {
        return _size;
    }

private:
    Vector2u _size;

};



#endif // MAP2D_H
