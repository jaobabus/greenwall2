#include "wall.h"

#include "console.h"




void Wall::create(Vector2u size, bool clear)
{
    Vector2D<WallEntity> old = std::move(_map);
    _map.resize(size);
    if (not clear) {
        for (unsigned y = 0; y < std::min<unsigned>(old.size2().y, size.y); y++) {
            for (unsigned x = 0; x < std::min<unsigned>(old.size2().x, size.x); x++) {
                _map[{x, y}] = std::move(old[{x, y}]);
            }
        }
    }
    for (unsigned y = 0; y < size.y; y++) {
        for (unsigned x = 0; x < size.x; x++) {
            _map[{x, y}].position = Vector2f(x, y);
        }
    }
}

void Wall::set_img(std::function<std::pair<ColorFloat, ColorFloat>(ColorFloat, Vector2u, int)> pixel)
{
    _pixel_mod = pixel;
}

void Wall::set_fade_speed(float speed)
{
    _fade_speed = speed;
}

void Wall::set_speed_factor(float speed)
{
    _speed_factor = speed;
}

void Wall::set_alphabet(std::string s)
{
    _alphabet = s;
}

void Wall::set_linear_mode(bool s)
{
    _linear_mode = s;
}

void Wall::set_random(std::function<uint32_t()> rd)
{
    _random = rd;
}

std::pair<float, float> Wall::fps() const
{
    return {_fps_avg, _last_fps};
}

size_t Wall::entities_count() const
{
    size_t count = 0;
    for (auto& e : _map)
        if (e.alive())
            count++;
    return count;
}

void Wall::tick(float elapsed)
{
    for (auto& entity : _map) {
        if (entity.alive())
            _tick_entity(entity, elapsed);
    }
}

bool Wall::summon(ColorFloat color, float speed, float light, bool force, char letter)
{
    if (_map.empty())
        return false;
    if (force) {
        std::vector<Vector2u> avails;
        for (auto& e : _map)
            if (not e.alive())
                avails.push_back(Vector2u(e.position));
        if (avails.empty())
            return false;
        auto pos = avails[_random() % avails.size()];
        _map[pos].summon_inplace(color, speed, light, pos);
        if (letter == ' ')
            letter = _random_letter(pos);
        _map[pos].letter = letter;
        return true;
    }
    else {
        Vector2u pos(_random() % _map.size2().x, _random() % _map.size2().y);
        auto& entity = _map[pos];
        if (not entity.alive()) {
            entity.summon_inplace(color, speed, light, pos);
            if (letter == ' ')
                letter = _random_letter(pos);
            entity.letter = letter;
            return true;
        }
    }
    return false;
}

WallEntity& Wall::select(Vector2u pos)
{
    return _map[pos];
}

void Wall::draw_console(std::string& out)
{
    int last_y = -1;
    ColorFloat last_bg{-1.f, 0.f, 0.f, 0.f};
    console::str_style(out, console::AsciiStyle::Bold);
    for (auto& entity : _map) {
        if (not entity.updated) {
            last_y = -1;
            continue;
        }
        if (last_y != int(entity.position.y)) {
            console::str_position(out, Vector2i(entity.position));
            last_y = int(entity.position.y);
        }
        auto pix = _pixel_mod(entity.color, Vector2u(entity.position), _frame);
        if (pix.second.a > 0.f and not eq_rgb(pix.second, last_bg)) {
            console::str_bg_color24(out, to_rgba(pix.second));
            last_bg = pix.second;
        }
        auto col = pix.first * entity.light;
        if (col.r > 0.f || col.g> 0.f || col.b > 0.f) {
            console::str_color24(out, to_rgba(pix.first * entity.light));
            out += (entity.letter ? entity.letter : _random_letter(Vector2u(entity.position)));
        }
        else {
            out += ' ';
        }
        entity.updated = false;
    }
    _fps_point();
    _frame++;
}

Vector2D<WallEntity>& Wall::map()
{
    return _map;
}

void Wall::_fps_point()
{
    if (_last_frame_time.time_since_epoch().count() != 0) {
        auto now = std::chrono::steady_clock::now();
        _last_fps = 1000000000.0 / std::chrono::nanoseconds(now - _last_frame_time).count();
        _fps_avg = _last_fps * 0.7 + _fps_avg * 0.3;
    }
    _last_frame_time = std::chrono::steady_clock::now();
}

void Wall::_tick_entity(WallEntity& entity, float elapsed)
{
    auto ep = entity.speed * elapsed * _fade_speed;
    entity.light -= ep;
    float y = entity.position.y + (entity.freezed ? 0.0f : entity.speed) * _speed_factor;
    unsigned iy = y;
    entity.updated = true;
    if (iy >= _map.size2().y) {
        entity.light = 0.f;
        return;
    }
    if (int(entity.position.y) != iy) {
        iy = int(entity.position.y) + 1;
        _map[{unsigned(entity.position.x), iy}] = entity;
        _map[{unsigned(entity.position.x), iy}].position.y = iy;
        entity.freezed = true;
    }
    else {
        entity.position.y = y;
    }
}

char Wall::_random_letter(Vector2u pos) const
{
    auto ri = _random();
    auto c = _alphabet[ri % _alphabet.size()];
    return c;
}
