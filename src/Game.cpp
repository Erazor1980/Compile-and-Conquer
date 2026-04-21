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
                m_selectionStart = m_window.mapPixelToCoords(mousePixelPosition);
                m_selectionCurrent = m_selectionStart;
                m_bIsSelecting = true;
                m_bHasDragged = false;
            }
        }
        else if (const auto* pMouseMoved = event->getIf<sf::Event::MouseMoved>())
        {
            if (m_bIsSelecting)
            {
                const sf::Vector2i mousePixelPosition{ pMouseMoved->position.x, pMouseMoved->position.y };
                m_selectionCurrent = m_window.mapPixelToCoords(mousePixelPosition);

                const float dragThreshold = 5.0f;

                if (std::abs(m_selectionStart.x - m_selectionCurrent.x) > dragThreshold ||
                    std::abs(m_selectionStart.y - m_selectionCurrent.y) > dragThreshold)
                {
                    m_bHasDragged = true;
                }
            }
        }
        else if (const auto* pMouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>())
        {
            if (pMouseButtonReleased->button == sf::Mouse::Button::Left && m_bIsSelecting)
            {
                m_bIsSelecting = false;

                const sf::Vector2f min{
                    std::min(m_selectionStart.x, m_selectionCurrent.x),
                    std::min(m_selectionStart.y, m_selectionCurrent.y)
                };

                const sf::Vector2f max{
                    std::max(m_selectionStart.x, m_selectionCurrent.x),
                    std::max(m_selectionStart.y, m_selectionCurrent.y)
                };

                const sf::FloatRect selectionRect(min, max - min);


                handleSelection(selectionRect, !m_bHasDragged);
            }
            else if (pMouseButtonReleased->button == sf::Mouse::Button::Right)
            {
                const sf::Vector2i mousePixelPosition{ pMouseButtonReleased->position.x, pMouseButtonReleased->position.y };
                const sf::Vector2f worldPosition = m_window.mapPixelToCoords(mousePixelPosition);
                m_world.moveSelectedUnitsTo(worldPosition);
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

    if (m_bIsSelecting)
    {
        const sf::Vector2f min{
            std::min(m_selectionStart.x, m_selectionCurrent.x),
            std::min(m_selectionStart.y, m_selectionCurrent.y)
        };

        const sf::Vector2f size{
            std::abs(m_selectionStart.x - m_selectionCurrent.x),
            std::abs(m_selectionStart.y - m_selectionCurrent.y)
        };

        sf::RectangleShape rect;
        rect.setPosition(min);
        rect.setSize(size);
        rect.setFillColor(sf::Color(0, 0, 255, 50));
        rect.setOutlineColor(sf::Color::Blue);
        rect.setOutlineThickness(1.0f);

        m_window.draw(rect);
    }

    m_window.display();
}

void Game::handleSelection(const sf::FloatRect& selectionRect, bool bIsClick)
{
    const bool bCtrlPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);

    if (bIsClick)
    {
        if (bCtrlPressed)
        {
            m_world.toggleUnitAt(m_selectionStart);
        }
        else
        {
            m_world.selectUnitAt(m_selectionStart);
        }
    }
    else
    {
        if (bCtrlPressed)
        {
            m_world.toggleUnitsInRect(selectionRect);
        }
        else
        {
            m_world.selectUnitsInRect(selectionRect);
        }
    }
}