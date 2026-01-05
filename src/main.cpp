#include <imgui-SFML.h>

#include <SFML/Graphics.hpp>

#include "atmo/AtmoDebugControl.h"

int main() {
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;

    auto window = sf::RenderWindow(sf::VideoMode({2560, 1440u}), "CMake SFML Project", sf::Style::Default,
                                   sf::State::Windowed, settings);
    window.setFramerateLimit(120);
    if (!ImGui::SFML::Init(window)) {
        return -1;
    }

    sf::RenderTexture renderTexture{{2560u, 1440u}};
    renderTexture.setSmooth(true);

    sf::Font font("fonts/Cabin-VariableFont_wdth,wght.ttf");


    // cursor
    sf::RectangleShape cursor(sf::Vector2f(5.f, 5.f));
    cursor.setFillColor(sf::Color::Red);

    sf::CircleShape circle(5.f);
    circle.setFillColor(sf::Color::Red);

    atmosil::atmo::AtmoDebugControl control{20, 10, 100.f};

    sf::Clock deltaClock;
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);
            // mapModule.HandleInput(window, *event);
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto *mouseMove = event->getIf<sf::Event::MouseMoved>()) {
                sf::Vector2i mouse = sf::Mouse::getPosition(window);
                sf::Vector2f mouseWorld = window.mapPixelToCoords(mouse);
                cursor.setPosition(mouseWorld);
            }

            if (const auto *mouseClick = event->getIf<sf::Event::MouseButtonPressed>()) {
                sf::Vector2i mouse = sf::Mouse::getPosition(window);
                sf::Vector2f mouseWorld = window.mapPixelToCoords(mouse);
                control.UpdateWalls(mouseWorld);
            }
        }
        const auto delta = deltaClock.restart();
        ImGui::SFML::Update(window, delta);
        control.Update(delta);

        control.RenderDebugPanel();


        window.clear();
        renderTexture.clear();

        control.Render(renderTexture);

        // cursor
        renderTexture.draw(cursor);

        renderTexture.display();

        const sf::Texture &texture = renderTexture.getTexture();

        sf::Sprite sprite(texture);
        window.draw(sprite);


        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}
