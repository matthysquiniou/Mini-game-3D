#pragma once
#include <string>

#include "Font.h"
#include "Window.h"

namespace sr
{
    class Text
    {
    public:
        Text() = default;
        Text(std::wstring const& text) {m_text = text; m_font.Load(L"Calibri");}
        Text(Text const& other) = delete; 
        Text(Text&& other) noexcept = delete;

        Text& operator=(Text const& other) = delete;
        Text& operator=(Text &&) = delete;

        ~Text() = default;

        void SetText(std::wstring const& text) { m_text = text; }
        void SetColor(gce::Color const& color) { m_brush.SetColor(color); }
        void SetOpacity(float32 opacity) { m_brush.SetOpacity(opacity); }
        void SetPosition(gce::Vector2f32 const& pos) { m_position = pos; };

    private:
        gce::Font m_font; 
        gce::ColorBrush m_brush = gce::ColorBrush(gce::Color::Red, 1.0f);
        std::wstring m_text;

        gce::Vector2f32 m_position {0.0f, 0.0f};

        friend class Window;
    };
}

