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
    int width = 1000;
    int height = 900;
    float speed = 200.0f;
    bool gameStart = false;

    sf::Texture introTex;
    if (!introTex.loadFromFile("files/images/LittlePrinceOnMoon.png")) {
        std::cout << "Error loading intro image" << std::endl;
    }
    sf::Sprite intro;
    intro.setTexture(introTex);

    sf::RectangleShape button(sf::Vector2f(500.0f, 100.0f));
    button.setFillColor(sf::Color(171, 222, 105));
    button.setPosition(sf::Vector2f(width / 2 - 250, height - 150));

    sf::CircleShape prince(100.f);
    prince.setFillColor(sf::Color::Black);
    prince.setPosition(sf::Vector2f(width / 2 - 100, height / 2 - 100));

    sf::RenderWindow start(sf::VideoMode(width, height), "Welcome!", sf::Style::Close);
    while(start.isOpen()) {
        sf::Event event;
        while(start.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                start.close();
            }
        }
        start.clear();
        start.draw(intro);
        start.draw(button);
        start.display();
    }

    if (gameStart) {
        sf::RenderWindow game(sf::VideoMode(width, height), "The Little Prince", sf::Style::Close);
        while(game.isOpen()) {
            sf::Event event;
            while(game.pollEvent(event)) {
                if(event.type == sf::Event::Closed) {
                    game.close();
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

            game.clear(sf::Color::White);
            game.draw(prince);
            game.display();
        }
    }
    return 0;
}