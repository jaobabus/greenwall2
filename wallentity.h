#ifndef WALLENTITY_H
#define WALLENTITY_H

#include "utils.h"




class WallEntity
{
public:
    WallEntity()
        : position(0.f),
          color{0.f, 0.f, 0.f, 0.f},
          speed(0.f),
          light(0.f),
          freezed(false),
          updated(false),
          letter('\0') {}
    WallEntity(WallEntity const& copy) {
        *this = copy;
    }

    WallEntity& operator=(WallEntity const& copy);

public:
    void summon_inplace(ColorFloat color, float speed, float light);
    void summon_inplace(ColorFloat color, float speed, float light, Vector2u position);
    bool alive() const noexcept;

public:
    Vector2f position;
    ColorFloat color;
    float speed;        // 0.0-1.0
    float light;        // 0.0-1.0 if 0.0 then died
    bool freezed;
    char letter;

public:
    bool updated;

};



#endif // WALLENTITY_H
