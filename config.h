#ifndef CONFIG_H
#define CONFIG_H

#include <minijson/json.h>
#include <string>




class Config
{
public:
    std::string image_path  = "image.png";
    std::string random_type = "default"; // one of default,steady_clock,crand,cpprandom
    std::string blend_type  = "default"; // one of default,middle,multiply
    std::string letter_mode = "default"; // one of default,random,one,file:<path>
    std::string alphabet = "0123456789qwertyuiop[]asdfghjkl;'zxcvbnm,./!@#$%^&*()_~`=+-QWERTYUIOP{}\\|ASDFGHJKL:\"ZXCVBNM<>?";
    std::string fg_color = "00FF00";
    std::string bg_color = "000000";
    int linear_mode = 0;
    int max_fps = 60;
    int debug = 1;
    float speed_factor = 0.2;
    float fade_factor = 1.0;
    int guaranted_spawn = 0;
    int spawn_rate = 3000;
    int realtime = 0;
    std::string schema = _schema();


    static constexpr auto json_properties() {
        return std::make_tuple(mini_json::property(&Config::image_path, "image_path"),
                               mini_json::property(&Config::random_type, "random_type"),
                               mini_json::property(&Config::blend_type, "blend_type"),
                               mini_json::property(&Config::alphabet, "alphabet"),
                               mini_json::property(&Config::linear_mode, "linear_mode"),
                               mini_json::property(&Config::max_fps, "max_fps"),
                               mini_json::property(&Config::debug, "debug"),
                               mini_json::property(&Config::speed_factor, "speed_factor"),
                               mini_json::property(&Config::fade_factor, "fade_factor"),
                               mini_json::property(&Config::guaranted_spawn, "guaranted_spawn"),
                               mini_json::property(&Config::spawn_rate, "spawn_rate"),
                               mini_json::property(&Config::fg_color, "fg_color"),
                               mini_json::property(&Config::bg_color, "bg_color"),
                               mini_json::property(&Config::realtime, "realtime")
                               );
    }

    static std::string _schema() {
return R"({
    "image_path": {
      "type": "string",
      "title": "Layer image/video path"
    },
    "random_type": {
      "type": "string",
      "title": "Random reference",
      "enum": ["default", "steady_clock", "crand", "cpprandom"]
    },
    "blend_type": {
      "type": "string",
      "title": "Pixels blend type",
      "enum": ["default", "middle", "multiply"]
    },
    "letter_mode": {
      "type": "string",
      "title": "Letter genereating mode",
      "enum": ["default", "random", "one", "reference"]
    },
    "alphabet": {
      "type": "string",
      "title": "alphabet for random in letter_mode:random or file path for letter_mode:reference"
    },
    "linear_mode": {
      "type": "string",
      "title": "Optimize console set_position option",
      "enum": [0, 1]
    },
    "max_fps": {
      "type": "integer",
      "title": "Max FPS"
    },
    "debug": {
      "type": "integer",
      "title": "Show debug information in top window",
      "enum": [0, 1]
    },
    "speed_factor": {
      "type": "number",
      "title": "Speed letters moving down"
    },
    "fade_factor": {
      "type": "number",
      "title": "Fade letters speed"
    },
    "guaranted_spawn": {
      "type": "integer",
      "title": "Guarante spawn for entity (very slow) ",
      "enum": [0, 1]
    },
    "spawn_rate": {
      "type": "integer",
      "title": "Entity spawn per frame"
    },
    "fg_color": {
      "type": "string",
      "title": "Color random spawn letters"
    },
    "bg_color": {
      "type": "string",
      "title": "Color background"
    },
    "realtime": {
      "type": "integer",
      "title": "Use only last frame (for streaming protocols) ",
      "enum": [0, 1]
    }
})";
    }

};


#endif // CONFIG_H
