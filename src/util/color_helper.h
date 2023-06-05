#ifndef COLOR_HELPER_H
#define COLOR_HELPER_H

#include "const.h"
#include <string>

class ColorHelper
{
public:
    static const std::wstring ToString(Color color)
    {
        switch (color)
        {
        case Color::Black:
            return kBlackIcon;
        case Color::White:
            return kWhiteIcon;
        default:
            return kInvalidIcon;
        }
    }
};

#endif
