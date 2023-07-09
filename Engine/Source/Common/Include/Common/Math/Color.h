//
// Created by johnk on 2023/7/7.
//

#pragma once

#include <cstdint>
#include <sstream>

#include <Common/Debug.h>

namespace Common {
    struct Color;
    struct LinearColor;

    struct Color {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;

        inline Color();
        inline Color(uint8_t inR, uint8_t inG, uint8_t inB);
        inline Color(uint8_t inR, uint8_t inG, uint8_t inB, uint8_t inA);
        inline explicit Color(const LinearColor& inColor);
        inline Color(const Color& inOther);
        inline Color(Color&& inOther) noexcept;
        inline Color& operator=(const Color& inOther);

        inline LinearColor ToLinearColor() const;
        std::string ToHexString() const;
    };

    struct LinearColor {
        float r;
        float g;
        float b;
        float a;

        inline LinearColor();
        inline LinearColor(float inR, float inG, float inB);
        inline LinearColor(float inR, float inG, float inB, float inA);
        inline explicit LinearColor(const Color& inColor);
        inline LinearColor(const LinearColor& inOther);
        inline LinearColor(LinearColor&& inOther) noexcept;
        inline LinearColor& operator=(const LinearColor& inOther);

        inline void CheckValid() const;
        inline Color ToColor() const;
    };

    struct ColorConsts {
        static const Color red;
        static const Color blue;
        // TODO more color here
    };
}

namespace Common {
    Color::Color()
        : r(0), g(0), b(0), a(0)
    {
    }

    Color::Color(uint8_t inR, uint8_t inG, uint8_t inB)
        : r(inR), g(inG), b(inB), a(255)
    {
    }

    Color::Color(uint8_t inR, uint8_t inG, uint8_t inB, uint8_t inA)
        : r(inR), g(inG), b(inB), a(inA)
    {
    }

    Color::Color(const LinearColor& inColor)
    {
        *this = inColor.ToColor();
    }

    Color::Color(const Color& inOther) = default;

    Color::Color(Color&& inOther) noexcept
        : r(inOther.r), g(inOther.g), b(inOther.b), a(inOther.a)
    {
    }

    Color& Color::operator=(const Color& inOther) = default;

    LinearColor Color::ToLinearColor() const
    {
        return {
            static_cast<float>(r) / 255.0f,
            static_cast<float>(g) / 255.0f,
            static_cast<float>(b) / 255.0f,
            static_cast<float>(a) / 255.0f
        };
    }

    std::string Color::ToHexString() const
    {
        std::stringstream result;
        result << "0x" << std::hex << r << std::hex << g << std::hex << b << std::hex << a;
        return result.str();
    }

    LinearColor::LinearColor()
        : r(0), g(0), b(0), a(0)
    {
        CheckValid();
    }

    LinearColor::LinearColor(float inR, float inG, float inB)
        : r(inR), g(inG), b(inB), a(1.0f)
    {
        CheckValid();
    }

    LinearColor::LinearColor(float inR, float inG, float inB, float inA)
        : r(inR), g(inG), b(inB), a(inA)
    {
        CheckValid();
    }

    LinearColor::LinearColor(const Color& inColor)
    {
        *this = inColor.ToLinearColor();
        CheckValid();
    }

    LinearColor::LinearColor(const LinearColor& inOther)
        : r(inOther.r), g(inOther.g), b(inOther.b), a(inOther.a)
    {
        CheckValid();
    }

    LinearColor::LinearColor(LinearColor&& inOther) noexcept
        : r(inOther.r), g(inOther.g), b(inOther.b), a(inOther.a)
    {
        CheckValid();
    }

    LinearColor& LinearColor::operator=(const LinearColor& inOther)
    {
        r = inOther.r;
        g = inOther.g;
        b = inOther.b;
        a = inOther.a;
        CheckValid();
        return *this;
    }

    Color LinearColor::ToColor() const
    {
        return {
            static_cast<uint8_t>(std::round(r * 255)),
            static_cast<uint8_t>(std::round(g * 255)),
            static_cast<uint8_t>(std::round(b * 255)),
            static_cast<uint8_t>(std::round(a * 255))
        };
    }

    void LinearColor::CheckValid() const
    {
        Assert(r >= 0.0f && r <= 1.0f);
        Assert(g >= 0.0f && g <= 1.0f);
        Assert(b >= 0.0f && b <= 1.0f);
        Assert(a >= 0.0f && a <= 1.0f);
    }
}
