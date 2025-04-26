#include "console.h"

#include <termio.h>




void str_color24(std::string& out, ColorRGBA color, bool fg)
{
    console::ConsoleCommandBuilder builder(&out, '[', 'm');
    builder.arg(fg ? 38 : 48);
    builder.arg(2);
    builder.arg(color.r);
    builder.arg(color.g);
    builder.arg(color.b);
    builder.end();
}

void console::str_color24(std::string& out, ColorRGBA color)
{
    str_color24(out, color, true);
}

void console::str_bg_color24(std::string& out, ColorRGBA color)
{
    str_color24(out, color, false);
}

void console::str_position(std::string& out, Vector2i pos)
{
    console::ConsoleCommandBuilder builder(&out, '[', 'H');
    builder.arg(pos.y + 1);
    builder.arg(pos.x + 1);
    builder.end();
}

Vector2u console::get_size()
{
    struct winsize w;
    if (ioctl(1, TIOCGWINSZ, &w)) {
        w.ws_col = 80;
        w.ws_row = 20;
    }
    return Vector2u(w.ws_col, w.ws_row);
}

void console::str_style(std::string& out, AsciiStyle style)
{
    console::ConsoleCommandBuilder builder(&out, '[', 'm');
    builder.arg(int(style));
    builder.end();
}
