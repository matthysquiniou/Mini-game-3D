#pragma once

#include <string_view>
#include <RenderWindow.h>
#include <define.h>

#include "Renderer.h"
#undef DrawText

namespace sr
{
    class Text;
    
    class Window
    {
    public:
        Window() = default;
        Window(std::wstring_view title, uint32 width, uint32 height);
        Window(Window const& other) = delete;
        Window(Window&& other) = delete;

        Window& operator=(Window const& other) = delete;
        Window& operator=(Window&& other) = delete;

        ~Window(); 

       void Create(std::wstring_view title, uint32 width, uint32 height);

        void Begin(Camera& camera);
        void Draw(Geometry& geo);
        void DrawText(Text& text);
        void End();

        void Display();
        
        bool IsOpen() const { return m_window.IsOpen(); }

        uint32 getWidth() { return m_width; }
        uint32 getHeight() { return m_height; }

    private:
        void RenderShadowPass();
        void RenderMainPass();
        void Render2DPass();
        Renderer& GetRenderer() { return m_renderer; }
        
    private:
        gce::RenderWindow m_window;
        Renderer m_renderer;
        
        uint32 m_width = 0;
        uint32 m_height = 0;

        std::vector<Geometry*> m_geoToDraw;
        std::vector<Text*> m_textToDraw;

        friend class Renderer;
        friend class ShadowMap;
    };   
}

