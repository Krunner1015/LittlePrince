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

bool checkPlanetCollision(
    const sf::CircleShape& planet,
    sf::Vector2f& princePosition,
    sf::Vector2f bottomLeft,
    sf::Vector2f bottomRight,
    float& Yvelocity,
    bool& inAir
) {
    float dxL = bottomLeft.x - planet.getPosition().x;
    float dxR = bottomRight.x - planet.getPosition().x;

    if (std::abs(dxL) > planet.getRadius() && std::abs(dxR) > planet.getRadius())
        return false; // too far away

    float dyL = std::sqrt(std::max(0.f, planet.getRadius() * planet.getRadius() - dxL * dxL));
    float dyR = std::sqrt(std::max(0.f, planet.getRadius() * planet.getRadius() - dxR * dxR));
    float groundYL = planet.getPosition().y - dyL;
    float groundYR = planet.getPosition().y - dyR;
    float groundY = std::min(groundYL, groundYR);

    const float epsilon = 5.0f;
    if ((bottomLeft.y >= groundYL || bottomRight.y >= groundYR) && Yvelocity >= 0.0f && princePosition.y >= groundY - epsilon) {
        princePosition.y = groundY;
        Yvelocity = 0.0f;
        inAir = false;
        return true;
    }
    return false;
}

int main() {
    sf::Clock clock;
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
    bool runL = false;
    bool runR = false;
    bool inAir = false;
    bool onPlanet = true;
    bool seenKing = false;
    bool seenVainMan = false;
    bool seenDrunkard = false;
    bool seenBusinessman = false;
    bool seenLamplighter = false;
    bool seenGeographer = false;
    bool gameStart = false;
    bool onEarth = false;

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

    sf::Texture starsTex;
    if (!starsTex.loadFromFile("files/images/stars.png")) {
        std::cout << "Error loading stars image" << std::endl;
    }
    starsTex.setRepeated(true);
    sf::RectangleShape stars(sf::Vector2f(width*10, height));
    stars.setTexture(&starsTex);
    stars.setTextureRect(sf::IntRect(0, 0, width*10, height));

    std::vector<sf::CircleShape> planets;
    std::vector<sf::Sprite> planetsTex;

    sf::Texture PrincePlanetTex;
    if (!PrincePlanetTex.loadFromFile("files/images/PrincePlanet.png")) {
        std::cout << "Error loading Prince Planet image" << std::endl;
    }
    sf::Sprite PrincePlanet;
    PrincePlanet.setTexture(PrincePlanetTex);
    PrincePlanet.setPosition(sf::Vector2f(0, 160));
    planetsTex.push_back(PrincePlanet);

    sf::CircleShape planet(200.0f); //prince
    planet.setFillColor(sf::Color(251, 239, 212));
    planet.setOrigin(200.0f, 200.0f);
    planet.setPosition(width/2, height/2 + 100);
    planets.push_back(planet);

    sf::Texture KingPlanetTex;
    if (!KingPlanetTex.loadFromFile("files/images/KingPlanet.png")) {
        std::cout << "Error loading King Planet image" << std::endl;
    }
    sf::Sprite KingPlanet;
    KingPlanet.setTexture(KingPlanetTex);
    KingPlanet.setPosition(sf::Vector2f(875, -25));
    planetsTex.push_back(KingPlanet);

    sf::CircleShape planet2(200.0f); //king
    planet2.setFillColor(sf::Color(253, 211, 90));
    planet2.setOrigin(200.0f, 200.0f);
    planet2.setPosition(width / 2 + 750, height / 2);
    planets.push_back(planet2);

    sf::Texture VainManPlanetTex;
    if (!VainManPlanetTex.loadFromFile("files/images/VainManPlanet.png")) {
        std::cout << "Error loading Vain Man Planet image" << std::endl;
    }
    sf::Sprite VainManPlanet;
    VainManPlanet.setTexture(VainManPlanetTex);
    VainManPlanet.setPosition(sf::Vector2f(1615, 15));
    planetsTex.push_back(VainManPlanet);

    sf::CircleShape planet3(200.0f); //vain man
    planet3.setFillColor(sf::Color(137, 164, 123));
    planet3.setOrigin(200.0f, 200.0f);
    planet3.setPosition(width / 2 + 1600, height / 2 + 200);
    planets.push_back(planet3);

    sf::Texture DrunkardPlanetTex;
    if (!DrunkardPlanetTex.loadFromFile("files/images/DrunkardPlanet.png")) {
        std::cout << "Error loading Drunkard Planet image" << std::endl;
    }
    sf::Sprite DrunkardPlanet;
    DrunkardPlanet.setTexture(DrunkardPlanetTex);
    DrunkardPlanet.setPosition(sf::Vector2f(2400, 245));
    planetsTex.push_back(DrunkardPlanet);

    sf::CircleShape planet4(200.0f); //drunkard
    planet4.setFillColor(sf::Color(169, 114, 42));
    planet4.setOrigin(200.0f, 200.0f);
    planet4.setPosition(width / 2 + 2400, height / 2 + 200);
    planets.push_back(planet4);

    sf::Texture BusinessmanPlanetTex;
    if (!BusinessmanPlanetTex.loadFromFile("files/images/BusinessmanPlanet.png")) {
        std::cout << "Error loading Businessman Planet image" << std::endl;
    }
    sf::Sprite BusinessmanPlanet;
    BusinessmanPlanet.setTexture(BusinessmanPlanetTex);
    BusinessmanPlanet.setPosition(sf::Vector2f(3195, 150));
    planetsTex.push_back(BusinessmanPlanet);

    sf::CircleShape planet5(200.0f); //businessman
    planet5.setFillColor(sf::Color(236, 180, 151));
    planet5.setOrigin(200.0f, 200.0f);
    planet5.setPosition(width / 2 + 3200, height / 2 + 100);
    planets.push_back(planet5);

    sf::Texture LamplighterPlanetTex;
    if (!LamplighterPlanetTex.loadFromFile("files/images/LamplighterPlanet.png")) {
        std::cout << "Error loading Lamplighter Planet image" << std::endl;
    }
    sf::Sprite LamplighterPlanet;
    LamplighterPlanet.setTexture(LamplighterPlanetTex);
    LamplighterPlanet.setPosition(sf::Vector2f(4000, 60));
    planetsTex.push_back(LamplighterPlanet);

    sf::CircleShape planet6(200.0f); //lamplighter
    planet6.setFillColor(sf::Color(186, 113, 203));
    planet6.setOrigin(200.0f, 200.0f);
    planet6.setPosition(width / 2 + 4000, height / 2 + 210);
    planets.push_back(planet6);

    sf::Texture GeographerPlanetTex;
    if (!GeographerPlanetTex.loadFromFile("files/images/GeographerPlanet.png")) {
        std::cout << "Error loading Geographer Planet image" << std::endl;
    }
    sf::Sprite GeographerPlanet;
    GeographerPlanet.setTexture(GeographerPlanetTex);
    GeographerPlanet.setPosition(sf::Vector2f(4800, 160));
    planetsTex.push_back(GeographerPlanet);

    sf::CircleShape planet7(200.0f); //geographer
    planet7.setFillColor(sf::Color(105, 68, 243));
    planet7.setOrigin(200.0f, 200.0f);
    planet7.setPosition(width / 2 + 4800, height / 2 + 110);
    planets.push_back(planet7);

    sf::Texture arrowTex;
    if (!arrowTex.loadFromFile("files/images/Arrow.png")) {
        std::cout << "Error loading arrow image" << std::endl;
    }
    sf::Sprite arrow;
    arrow.setTexture(arrowTex);
    arrow.setPosition(sf::Vector2f(5500, 0));

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
        sf::View view(sf::FloatRect(0, 0, width, height));
        sf::Vector2f position = prince.getPosition();
        while(game.isOpen()) {
            sf::Event event;
            while(game.pollEvent(event)) {
                if(event.type == sf::Event::Closed) {
                    game.close();
                    gameStart = false;
                }
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Escape) {
                        game.close();
                        gameStart = false;
                    }
                    if (event.key.code == sf::Keyboard::Down) {
                        std::cout << "Down" << std::endl;
                        seenKing = true;
                        seenVainMan = true;
                        seenDrunkard = true;
                        seenBusinessman = true;
                        seenLamplighter = true;
                        seenGeographer = true;
                    }
                    if (event.key.code == sf::Keyboard::R) {
                        position.x = planet.getPosition().x;
                        position.y = planet.getPosition().y - planet.getRadius();
                        Yvelocity = 0.0f;
                        inAir = false;
                        view.setCenter(position);
                    }
                    if (event.key.code == sf::Keyboard::E) {
                        position.x = planet7.getPosition().x;
                        position.y = planet7.getPosition().y - planet7.getRadius();
                        Yvelocity = 0.0f;
                        inAir = false;
                        view.setCenter(position);
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
                runL = true;
                runR = false;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { // Move right
                position.x += speed * deltaTime;
                runL = false;
                runR = true;
            }

            //Gravity
            Yvelocity += gravity * deltaTime;
            position.y += Yvelocity * deltaTime;
            sf::Vector2f bottomLeft(position.x - prince.getSize().x / 2, position.y);
            sf::Vector2f bottomRight(position.x + prince.getSize().x / 2, position.y);

            // Planet collision check
            onPlanet = false;
            for (auto& p : planets) {
                onPlanet |= checkPlanetCollision(p, position, bottomLeft, bottomRight, Yvelocity, inAir);
            }
            if (!onPlanet) {
                inAir = true;
            }

            // Respawn if prince falls out of view
            if (position.y > height + 400) {
                if (position.x > 5600 && seenKing && seenVainMan && seenDrunkard && seenBusinessman && seenLamplighter && seenGeographer) {
                    game.close();
                    gameStart = false;
                    onEarth = true;
                }
                float minDist = std::numeric_limits<float>::max();
                sf::Vector2f bestPos;
                for (auto& p : planets) {
                    float dx = position.x - p.getPosition().x;
                    float dist = std::abs(dx); // or use sqrt(dx*dx + dy*dy) for full 2D distance
                    if (dist < minDist) {
                        minDist = dist;
                        bestPos.x = p.getPosition().x;
                        bestPos.y = p.getPosition().y - p.getRadius(); // just above the surface
                    }
                }
                position = bestPos;
                Yvelocity = 0.0f;
                inAir = false;
                view.setCenter(position);
            }

            prince.setPosition(position);
            view.setCenter(position);
            game.setView(view);

            game.clear(sf::Color(29, 62, 143));
            sf::Vector2f viewCenter = view.getCenter() *0.8f;
            stars.setPosition(viewCenter.x - width/2, viewCenter.y - height/2);
            game.draw(stars);
            game.draw(arrow);
            if (runL && !runR) {
                game.draw(prince);
            } else if (runR && !runL) {
                game.draw(prince);
            } else {
                game.draw(prince);
            }
            runL = false;
            runR = false;
            for (auto& p : planets) {
                game.draw(p);
            }
            for (auto& p : planetsTex) {
                game.draw(p);
            }
            game.display();
        }
    }
    return 0;
}