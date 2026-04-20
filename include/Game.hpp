#pragma once

#include <SFML/Graphics.hpp>
#include "World.hpp"

class Game
{
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float deltaTime);
    void render();

private:
    sf::RenderWindow m_window;
    World m_world;
};
