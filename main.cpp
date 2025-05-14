#include <iostream>
#include <fstream>
#include <cctype>
#include <vector>
#include <random>
#include <SFML/Graphics.hpp>

void setText(sf::Text &text, float x, float y) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f,
    textRect.top + textRect.height/2.0f);
    text.setPosition(sf::Vector2f(x, y));
}

int main() {
    sf::Clock clock;
    sf::Time pausedDuration = sf::Time::Zero;
    sf::Time pauseStartTime;
    sf::Time finalTime;
    sf::Font font;
    if (!font.loadFromFile("files/font.ttf")) {
        std::cout << "Error loading font" << std::endl;
        return -1;
    }
    int width = 800;
    int height = 600;
    float speed = 200.0f;

    sf::RenderWindow window(sf::VideoMode(width, height), "Welcome!", sf::Style::Close);
    sf::CircleShape prince(100.f);
    prince.setFillColor(sf::Color::Black);
    prince.setPosition(sf::Vector2f(width / 2 - 100, height / 2 - 100));

    while(window.isOpen()) {
        sf::Event event;
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                window.close();
            }
        }

        float deltaTime = clock.restart().asSeconds();

        // Handle player movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) // Move up
            prince.move(0, -speed * deltaTime);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) // Move down
            prince.move(0, speed * deltaTime);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) // Move left
            prince.move(-speed * deltaTime, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) // Move right
            prince.move(speed * deltaTime, 0);

        window.clear(sf::Color::White);
        window.draw(prince);
        window.display();
    }
    return 0;
}