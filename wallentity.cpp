#include "wallentity.h"



WallEntity& WallEntity::operator=(const WallEntity& copy) {
    position = copy.position;
    color = copy.color;
    speed = copy.speed;
    light = copy.light;
    freezed = copy.freezed;
    updated = copy.updated;
    letter = copy.letter;
    return *this;
}

void WallEntity::summon_inplace(ColorFloat color, float speed, float light)
{
    this->color = color;
    this->speed = speed;
    this->light = light;
    freezed = false;
    updated = true;
    letter = '\0';
}

void WallEntity::summon_inplace(ColorFloat color, float light, float speed, Vector2u position)
{
    summon_inplace(color, light, speed);
    this->position = Vector2f(position);
}

bool WallEntity::alive() const noexcept
{
    return light > 0.f;
}
