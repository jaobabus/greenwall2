#ifndef APP_H
#define APP_H

#include <map>
#include <sstream>
#include <variant>

#include "console.h"
#include "config.h"
#include "wall.h"
#include "image_backend.h"
#include "profile.h"




class App
{
public:
    App(Config config)
        : _config(config) {}

    Config get_config();
    bool update_config(Config config);
    void run();

private:
    std::pair<ColorFloat, ColorFloat> _pixel_mod(ColorFloat color, Vector2u pos, int frame);
    void _report(std::string& out, Wall const& wall);
    ColorFloat _blend_middle(ColorFloat a, ColorFloat b);
    ColorFloat _blend_multiply(ColorFloat a, ColorFloat b);

private:
    bool _init_config();
    bool _load_image();
    void _run();

private:
    Config _config;
    ProfileMap _profile;
    Wall _wall;
    std::unique_ptr<AsyncSimpleImage> _image;
    std::function<ColorFloat(ColorFloat a, ColorFloat b)> _blend;

};



#endif // APP_H
