#include "pch.h"

#include "World.hpp"

int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    World world = World();

    world.loop();

    return 0;
}