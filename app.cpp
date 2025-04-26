#include "app.h"

#include <thread>
#include <numeric>
#include <random>

#include "image_backend.h"



void App::_report(std::string& out, Wall const& wall) {
    console::str_position(out, {0, 0});
    console::str_color24(out, from_hex("B80"));
    auto prof = _profile.format();
    ssprintf(out, " Entities %5d FPS %3d Prof %s", wall.entities_count(), int(wall.fps().first), prof.c_str());
}

ColorFloat App::_blend_middle(ColorFloat p1, ColorFloat p2)
{
    auto b = p1.a + p2.a;
    auto p = p1 * p1.a / b + p2 * p2.a / b;
    return p;
}

ColorFloat App::_blend_multiply(ColorFloat p1, ColorFloat p2)
{
    auto p = p1 * (1.0 - p2.a) + p2 * p2.a;
    return p;
}


class MyRand
{
public:
    MyRand() {}

    int operator()() {
        return _uniform_dist(_e1) | (_uniform_dist(_e1) << 16);
    }

private:
    std::random_device _dev;
    std::default_random_engine _e1{_dev()};
    std::uniform_int_distribution<int> _uniform_dist{0, 65536};

};

bool App::_init_config()
{
    _wall.create({240, 50}, true);
    _wall.set_linear_mode(_config.linear_mode);
    _wall.set_img([this](auto c, auto p, auto f) { return _pixel_mod(c, p, f); });
    _wall.set_speed_factor(_config.speed_factor);
    _wall.set_fade_speed(_config.fade_factor);
    _wall.set_alphabet(_config.alphabet);

    if (_config.blend_type == "middle")
        _blend = [this](auto a, auto b) { return _blend_middle(a, b); };
    else
        _blend = [this](auto a, auto b) { return _blend_multiply(a, b); };

    if (_config.random_type == "steady_clock")
        _wall.set_random([]() { return std::chrono::steady_clock::now().time_since_epoch().count(); });
    if (_config.random_type == "cpprandom")
        _wall.set_random([rd=std::make_shared<MyRand>()]() mutable { return (*rd)(); });
    else
        _wall.set_random([]() { return rand() | (rand() << 16); });
    return true;
}

bool App::_load_image()
{
    if (not _config.image_path.empty()) {
        _image = ImageBackend().load_async(_config.image_path);
        if (not _image)
            return false;
        _image->start();
    }
    return true;
}

std::string format_bytes_size(size_t s) {
    if (s < 10U * 1024)
        return std::to_string(s) + "B";
    if (s < 10U * 1024 * 1024)
        return std::to_string(s / 1024) + "K";
    if (s < 10U * 1024 * 1024 * 1024)
        return std::to_string(s / 1024 / 1024) + "M";
    return std::to_string(s / 1024 / 1024 / 1024) + "G";
}

void App::_run()
{
    auto last_time = std::chrono::steady_clock::now();
    auto last_tick_time = std::chrono::steady_clock::now();
    std::vector<int> summon_fails;
    while (true) {
        _profile.profile("i", [&] {
            auto ws = console::get_size();
            if (ws != _wall.map().size2()) {
                _wall.create(ws, false);
                for (auto& e : _wall.map()) {
                    if (not e.alive())
                        e.summon_inplace(to_float(from_hex(_config.fg_color)), 1.0, 1.0);
                }
            }
            _profile.profile("sz", std::to_string(ws.x) + "x" + std::to_string(ws.y));
        });
        _profile.profile("a", [&] {
            _profile.profile("s", [&] {
                int fails = 0;
                for (int i = 0; i < _config.spawn_rate; i++)
                    if (not _wall.summon(to_float(from_hex(_config.fg_color)), 1.0, 1.0, _config.guaranted_spawn, ' '))
                        fails++;
                summon_fails.push_back(fails);
                if (summon_fails.size() > 60)
                    summon_fails.erase(begin(summon_fails));
                _profile.profile("ns", _config.spawn_rate - fails);
                _profile.profile("sf", std::accumulate(begin(summon_fails), end(summon_fails), 0));
            });
            _profile.profile("t", [&] {
                auto now = std::chrono::steady_clock::now();
                float elapsed = std::chrono::nanoseconds(now - last_tick_time).count() / 1000000000.0;
                // float elapsed = 1.0 / 60;
                _wall.tick(elapsed);
                last_tick_time = std::chrono::steady_clock::now();
            });
            std::string out;
            if (_image)
                _profile.profile("df", std::to_string(_image->frame_count()) + "/" + std::to_string(_image->frame_count_total()));
            _profile.profile("d", [&] { _wall.draw_console(out); });
            _profile.profile("w", [&] {
                out.reserve(1024 * 1024 * 1024);
                if (_config.debug)
                    _report(out, _wall);
                fwrite(out.c_str(), out.size(), 1, stdout);
                fflush(stdout);
                _profile.profile("wb", format_bytes_size(out.size()));
            });

            auto now = std::chrono::steady_clock::now();
            auto sleep_time = std::chrono::nanoseconds(1000000000) / _config.max_fps - (now - last_time);
            _profile.profile("f", [&] {
                if (sleep_time.count() > 0)
                    std::this_thread::sleep_for(sleep_time);
            });
            last_time = std::chrono::steady_clock::now();
        });
    }
}

Config App::get_config()
{
    return _config;
}

bool App::update_config(Config config)
{
    auto old = _config;
    _config = config;
    if (not _load_image() or not _init_config()) {
        update_config(_config);
        return false;
    }
    return true;
}

void App::run()
{
    if (not _load_image() or not _init_config())
        return;
    _run();
}

std::pair<ColorFloat, ColorFloat> App::_pixel_mod(ColorFloat color, Vector2u pos, int frame)
{
    if (_image) {
        frame = frame * _image->fps() / 60;
        Vector2u real(pos.x * _image->size().x / _wall.map().size2().x, pos.y * _image->size().y / _wall.map().size2().y);
        auto pix = to_float(_image->get_pixel(real, (_config.realtime ? _image->frame_count() - 1 : frame)));
        pix = _blend(color, pix);
        return {pix, to_float(from_hex(_config.bg_color))};
    }
    else {
        return {color, to_float(from_hex(_config.bg_color))};
    }
}
