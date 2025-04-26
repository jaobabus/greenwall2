#ifndef CONSOLE_H
#define CONSOLE_H

#include <string>
#include "utils.h"



namespace console {

enum class AsciiStyle {
    Normal = 0,
    Bold = 1,
    Faint = 2,
    Italic = 3,
    Underline = 4,
    Blink = 5,
    RapidBlink = 6,
    Reverse = 7,
    ConcealOrHide = 8,
    Strike = 9,
    Primary = 10,
    AltenativeFontStart = 11,
    AltenativeFontEnd = 19,
    Gothic = 20,
    StartInverse = 21,
    DoubleUnderlineOrNotBold = 21,
    NotFaint = 22,
    NotItalic = 23,
    NotUnderline = 24,
    NotBlinking = 25,
    ProportionalSpacing = 26,
    NotResersed = 27,
    Reveral = 28,
    NotStrike = 29,
    EndInverse = 29,
    Foregrount4StartLower = 30,
    Foreground8Or24 = 38,
    ResetForeground = 39,
    Background4StartLower = 40,
    Background8Or24 = 48,
    ResetBackground = 49,
    NotProportionalSpacing = 50,
    Framed = 51,
    Encirled = 52,
    Overlined = 53,
    NotFramedNotEncircled = 54,
    NotOverlined = 55,
    UnderlineColor = 58,
    ResetUnderlineColor = 59,
    IdeogramUnderline = 60,
    IdeogramDoubleUnderline = 61,
    IdeogramOverline = 62,
    IdeogramDoubleOverline = 63,
    IdeogramSterssMarking = 64,
    NotIdeogram = 65,
    Superscript = 73,
    Subscript = 74,
    NotSuperSubScript = 75,
    Foregrount4StartUpper = 90,
    Backgrount4StartUpper = 100,
};


class ConsoleCommandBuilder
{
public:
    ConsoleCommandBuilder(std::string* out, char mod, char term)
        : _out(out), _mod(mod), _term(term), _has_arg(false) {
        *_out += '\033';
        if (_mod)
            *_out += _mod;
    }

    void arg(int v) {
        char buf[16];
        sprintf(buf, "%d;", v);
        *_out += buf;
        _has_arg = true;
    }
    void arg(const char* v) {
        *_out += v;
        *_out += ';';
        _has_arg = true;
    }
    void end() const {
        if (_has_arg)
            (*_out)[_out->size() - 1] = _term;
        else
            *_out += _term;
    }

private:
    std::string* _out;
    char _mod;
    char _term;
    bool _has_arg;

};


void str_color24(std::string& out, ColorRGBA color);
void str_bg_color24(std::string& out, ColorRGBA color);
void str_position(std::string& out, Vector2i pos);
void str_style(std::string& out, AsciiStyle style);

Vector2u get_size();

}


#endif // CONSOLE_H
