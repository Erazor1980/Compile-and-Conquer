#include "Game.hpp"

Game::Game()
    : m_window(sf::VideoMode({ 1280u, 720u }), "Compile and Conquer")
{
    m_window.setFramerateLimit(60);
}

void Game::run()
{
    sf::Clock clock;

    while (m_window.isOpen())
    {
        const float deltaTime = clock.restart().asSeconds();

        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::processEvents()
{
    while (const std::optional event = m_window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            m_window.close();
        }
        else if (const auto* pMouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (pMouseButtonPressed->button == sf::Mouse::Button::Left)
            {
                const sf::Vector2i mousePixelPosition{ pMouseButtonPressed->position.x, pMouseButtonPressed->position.y };
                const sf::Vector2f worldPosition = m_window.mapPixelToCoords(mousePixelPosition);

                m_world.selectUnitAt(worldPosition);
            }
        }
    }
}

void Game::update(float deltaTime)
{
    m_world.update(deltaTime);
}

void Game::render()
{
    m_window.clear(sf::Color(30, 30, 30));
    m_world.render(m_window);
    m_window.display();
}