#ifndef WALL_H
#define WALL_H

#include <functional>
#include <chrono>

#include "map2d.h"
#include "wallentity.h"



class Wall
{
public:
    Wall()
        : _last_frame_time(),
          _pixel_mod([](ColorFloat p, Vector2u, int) -> std::pair<ColorFloat, ColorFloat>
                     { return {p, ColorFloat{0.f, 0.f, 0.f, 0.f}}; }),
          _random([]{ return rand(); }),
          _fade_speed(1.0),
          _speed_factor(0.1),
          _alphabet("0123456789qwertyuiop[]asdfghjkl;'zxcvbnm,./!@#$%^&*()_~`=+-QWERTYUIOP{}\\|ASDFGHJKL:\"ZXCVBNM<>?"),
          _frame(0) {}

public:
    void create(Vector2u size, bool clear);
    void set_img(std::function<std::pair<ColorFloat, ColorFloat>(ColorFloat, Vector2u, int frame)> pixel);
    void set_fade_speed(float speed);
    void set_speed_factor(float speed);
    void set_alphabet(std::string s);
    void set_linear_mode(bool s);
    void set_random(std::function<uint32_t()>);
    std::pair<float, float> fps() const;
    size_t entities_count() const;

public:
    void tick(float elapsed);
    bool summon(ColorFloat color, float speed = 1.0, float light = 1.0, bool force = true, char letter = '\0');
    WallEntity& select(Vector2u pos);
    void draw_console(std::string& out);
    Vector2D<WallEntity>& map();

private:
    void _fps_point();
    void _tick_entity(WallEntity& entity, float elapsed);
    char _random_letter(Vector2u pos) const;

private:
    std::chrono::steady_clock::time_point _last_frame_time;
    float _last_fps, _fps_avg;
    std::function<std::pair<ColorFloat, ColorFloat>(ColorFloat, Vector2u, int)> _pixel_mod;
    std::function<uint32_t()> _random;
    float _fade_speed;
    float _speed_factor;
    std::string _alphabet;
    bool _linear_mode;
    int _frame;

private:
    Vector2D<WallEntity> _map;


};




#endif // WALL_H
