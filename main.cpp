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
    int height = 1100;
    float speed = 200.0f;
    float gravity = 500.0f;
    float Yvelocity = 0.0f;
    bool running = true;
    bool inAir = false;
    bool gameStart = false;

    sf::Texture introTex;
    if (!introTex.loadFromFile("files/images/LittlePrinceOnMoon.png")) {
        std::cout << "Error loading intro image" << std::endl;
    }
    sf::Sprite intro;
    intro.setTexture(introTex);
    intro.setPosition(sf::Vector2f(0, 200));

    sf::Text welcome("The Little Prince", font, 75);
    welcome.setFillColor(sf::Color::White);
    welcome.setStyle(sf::Text::Bold);
    setText(welcome, width/2, 100);

    sf::RectangleShape button(sf::Vector2f(500.0f, 100.0f));
    button.setFillColor(sf::Color(171, 222, 105));
    button.setPosition(sf::Vector2f(width / 2 - 250, height - 150));

    sf::Text startText("Start", font, 50);
    startText.setFillColor(sf::Color::White);
    startText.setStyle(sf::Text::Bold);
    setText(startText, width/2, height - 100);

    sf::RectangleShape prince(sf::Vector2f(50.0f, 100.0f));
    prince.setFillColor(sf::Color::Green);
    prince.setOrigin(prince.getSize().x / 2, prince.getSize().y);
    prince.setPosition(sf::Vector2f(width / 2, height - 300.0f));

    sf::CircleShape planet(200.0f);
    planet.setFillColor(sf::Color(169, 169, 169));
    planet.setOrigin(200.0f, 200.0f);
    planet.setPosition(width/2, height/2 + 100);

    sf::RenderWindow start(sf::VideoMode(width, height), "Welcome!", sf::Style::Close);
    while(start.isOpen()) {
        sf::Event event;
        while(start.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                start.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    int x = event.mouseButton.x;
                    int y = event.mouseButton.y;
                    if ((x > width / 2 - 250 && x < width / 2 + 250) && (y > height - 150 && y < height - 50)) {
                        start.close();
                        gameStart = true;
                        height = 900;
                    }
                }
            }
        }
        start.clear(sf::Color(29, 62, 143));
        start.draw(intro);
        start.draw(welcome);
        start.draw(button);
        start.draw(startText);
        start.display();
    }

    if (gameStart) {
        sf::RenderWindow game(sf::VideoMode(width, height), "The Little Prince", sf::Style::Close);
        sf::Vector2f position = prince.getPosition();
        while(game.isOpen()) {
            sf::Event event;
            while(game.pollEvent(event)) {
                if(event.type == sf::Event::Closed) {
                    game.close();
                }
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Escape) {
                        game.close();
                    }
                    if (event.key.code == sf::Keyboard::Down) {
                        std::cout << "Down" << std::endl;
                    }
                    if (event.key.code == sf::Keyboard::R) {
                        position.x = planet.getPosition().x;
                        position.y = planet.getPosition().y - planet.getRadius();
                        Yvelocity = 0.0f;
                        inAir = false;
                    }
                }
            }

            float deltaTime = clock.restart().asSeconds();

            // Handle player movement
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !inAir) { // Jump
                Yvelocity = -600.0f;
                inAir = true;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { // Move left
                position.x -= speed * deltaTime;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { // Move right
                position.x += speed * deltaTime;
            }

            //Gravity
            Yvelocity += gravity * deltaTime;
            position.y += Yvelocity * deltaTime;
            sf::Vector2f bottomLeft(position.x - prince.getSize().x / 2, position.y);
            sf::Vector2f bottomRight(position.x + prince.getSize().x / 2, position.y);

            // Planet collision check
            auto checkCornerContact = [&](sf::Vector2f corner) {
                float dx = corner.x - planet.getPosition().x;
                if (std::abs(dx) >= planet.getRadius()) {
                    return false;
                }
                float dy = std::sqrt(std::max(0.f, planet.getRadius() * planet.getRadius() - dx * dx));
                float groundY = planet.getPosition().y - dy;
                return corner.y >= groundY;
            };
            bool leftOnPlanet = checkCornerContact(bottomLeft);
            bool rightOnPlanet = checkCornerContact(bottomRight);
            if ((leftOnPlanet || rightOnPlanet) && Yvelocity >= 0.0f) {
                float dxL = bottomLeft.x - planet.getPosition().x;
                float dxR = bottomRight.x - planet.getPosition().x;
                float dyL = std::sqrt(std::max(0.f, planet.getRadius() * planet.getRadius() - dxL * dxL));
                float dyR = std::sqrt(std::max(0.f, planet.getRadius() * planet.getRadius() - dxR * dxR));
                float groundYL = planet.getPosition().y - dyL;
                float groundYR = planet.getPosition().y - dyR;
                float groundY = std::min(groundYL, groundYR);
                const float epsilon = 5.0f;
                if (position.y >= groundY - epsilon) {
                    position.y = groundY;
                    Yvelocity = 0.0f;
                    inAir = false;
                } else {
                    inAir = true;
                }
            } else {
                inAir = true;
            }


            // Respawn if prince falls out of view
            if (position.y > height + 100) {
                position.x = planet.getPosition().x;
                position.y = planet.getPosition().y - planet.getRadius();
                Yvelocity = 0.0f;
                inAir = false;
            }

            prince.setPosition(position);

            game.clear(sf::Color(29, 62, 143));
            game.draw(prince);
            game.draw(planet);
            game.display();
        }
    }
    return 0;
}