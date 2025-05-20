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

void updateDialogueDisplay(
    sf::Text &text,
    sf::RectangleShape &box,
    std::vector<std::string> &lines,
    const sf::Font &font,
    unsigned int characterSize,
    int windowWidth
) {
    std::string allText;
    for (const auto& line : lines)
        allText += line + "\n";

    text.setString(allText);

    sf::FloatRect bounds = text.getLocalBounds();
    box.setSize(sf::Vector2f(windowWidth - 100, bounds.height + 40));
    box.setOrigin(box.getSize().x / 2, box.getSize().y / 2);
    box.setPosition(windowWidth / 2.0f, 800); // adjust as needed
    text.setPosition(windowWidth / 2.0f, 800 - bounds.height / 2 + 10);
}

int main() {
    sf::Clock clock;
    sf::Font font;
    if (!font.loadFromFile("files/font.ttf")) {
        std::cout << "Error loading font" << std::endl;
        return -1;
    }
    sf::Clock dialogueClock;
    float dialogueDelay = 2.0f;
    int currentLineIndex = 0;
    std::vector<std::string>* currentConvo = nullptr;
    std::vector<std::string> visibleLines;
    int width = 1000;
    int height = 1100;
    float speed = 200.0f;
    float gravity = 500.0f;
    float Yvelocity = 0.0f;
    bool runL = false;
    bool runR = false;
    bool inAir = false;
    bool onPlanet = true;
    bool seenFlower = false;
    bool inFlowerConvo = false;
    bool seenKing = false;
    bool inKingConvo = false;
    bool seenVainMan = false;
    bool inVainConvo = false;
    bool seenDrunkard = false;
    bool inDrunkardConvo = false;
    bool seenBusinessman = false;
    bool inBusinessmanConvo = false;
    bool seenLamplighter = false;
    bool inLamplighterConvo = false;
    bool seenGeographer = false;
    bool inGeographerConvo = false;
    bool seenFlowers = false;
    bool inFlowersConvo = false;
    bool seenFox = false;
    bool inFoxConvo = false;
    bool gameStart = false;
    bool onEarth = false;

    std::vector<std::string> flowerConvo = {
        "Prince: Good-bye",
        "Flower: ...",
        "Prince: Good-bye",
        "Flower: 'cough'",
        "Flower: I've been silly, I ask your forgiveness.",
        "        Try to be happy.",
        "Prince: ...",
        "Flower: Of course I love you, ",
        "Flower: It was my fault you never knew. It doesn't matter.",
        "        But you were just as silly as I was. Try to be happy",
        "        Put that glass thing down. I don't want it anymore.",
        "Prince: But the wind . . .",
        "Flower: My cold isn't that bad . . . The night air will do me",
        "        good. I'm a flower.",
        "Prince: But the animals . . .",
        "Flower: I need to put up with two or three caterpillars if I",
        "        want to get to know the butterflies. Apparently",
        "        they're very beautiful. Otherwise who will visit me?",
        "        You'll be far away. As for the big animals, I'm not",
        "        afraid of them. I have my own claws.",
        "Flower: Don't hang around like this; it's irritating. You",
        "        made up your mind to leave. Now go."
    };
    std::vector<std::string> kingConvo = {
        "King: Ah! A subject!",
        "Prince: How do you know me?",
        "King: Come closer. Let me see you.",
        "Prince: 'yawn'",
        "King: No yawning before a king!",
        "Prince: Sorry... I'm tired.",
        "King: Then I order you to yawn.",
        "Prince: I can't now...",
        "King: Hmm. May I remind you I must be obeyed.",
        "Prince: May I sit?",
        "King: I order you to sit.",
        "Prince: What do you rule over?",
        "King: Everything.",
        "King: this planet, others, the stars.",
        "Prince: And they obey?",
        "King: Of course. I don't allow disobedience.",
        "Prince: Can you order a sunset? I miss mine.",
        "King: I will, when conditions are right. Around 7:40 tonight.",
        "Prince: I'm bored. I'll go now.",
        "King: Don't! I'll make you Minister of Justice!",
        "Prince: But there's no one to judge.",
        "King: Then judge yourself. that's hardest of all.",
        "Prince: I can do that anywhere. Goodbye.",
        "King: Wait! I name you my Ambassador!",
        "Prince (to himself): Grown-ups are so strange"
    };
    std::vector<std::string> vainConvo = {
        "Vain Man: Ah! A visit from an admirer!",
        "Prince: Hello. That's a funny hat you're wearing.",
        "Vain Man: It's for answering acclamations.",
        "Vain Man: Unfortunately, no one ever comes this way.",
        "Prince: Oh...",
        "Vain Man: Clap your hands!",
        "Prince: 'claps hands'",
        "Vain Man: 'tips hat'",
        "Prince: And what would make the hat fall off?",
        "Vain Man: . . .",
        "Vain Man: Do you admire me?",
        "Prince: What does admire mean?",
        "Vain Man: It means to acknowledge that I am the handsomest,",
        "          the best-dresed, the richest, and the most",
        "          inteligent man on the planet",
        "Prince: But you're the only man on your planet!",
        "Vain Man: Do me this favor. Admire me all the same.",
        "Prince: I admire you ... but what does it matter?",
        "Prince (to himself): Grown-ups are certainly very strange"
    };
    std::vector<std::string> drunkardConvo = {
        "Prince: What are you doing there?",
        "Drunkard: Drinking,",
        "Prince: Why are you drinking?",
        "Drunkard: To forget,",
        "Prince: To forget what?",
        "Drunkard: To forget that I'm ashamed,",
        "Prince: What are you ashamed of?",
        "Drunkard: Of drinking!",
        "Prince (to himself): Grown-ups are certainly very, very strange"
    };
    std::vector<std::string> businessmanConvo = {
        "Prince: Hello. Your cigarette's gone out.",
        "Businessman: Three and two is five. Five and seven, twelve.",
        "             Twelve and three, fifteen. Hello. Fifteen and",
        "             seven, twenty-two. Twenty-two and six, twenty-",
        "             eight. No time to light it again. Twenty-six and",
        "             five, thirty-one. Whew! That amounts to five-",
        "             hundred-and-one million, six-hundred-twenty-two",
        "             thousand, seven hundred thirty-one.",
        "Prince: Five hundred million what?",
        "Businessman: What? Oh, litle glittering things in the sky.",
        "Prince: Flies? Bees?",
        "Businessman: No! Stars. I own them.",
        "Prince: You own the stars?",
        "Businessman: Yes. I'm rich. I can buy more stars if discovered.",
        "Prince: But what do you do with them?",
        "Businessman: I count them, write the number down, lock it in a",
        "             drawer. That's enough.",
        "Prince: That's not like owning a flower or a scarf, you can't",
        "        use or care for stars.",
        "Businessman: But I own them.",
        "Prince: I water my flower and clean my volcanoes. I benefit",
        "        them. But your stars, do not benefit from you.",
        "Businessman: . . .",
        "Prince (to himself): Grown-ups are certainly quite",
        "                     extraordinary."
    };
    std::vector<std::string> lamplighterConvo = {
        "Prince: Good morning. Why did you just put out your lamp?",
        "Lamplighter: Orders. Good morning. (lights it again)",
        "Prince: And now you've lit it?",
        "Lamplighter: Orders again. Good evening.",
        "Prince: I don't understand.",
        "Lamplighter: There's nothing to understand. Orders are",
        "             orders. In the old days, I had time to rest. Now",
        "             the planet spins once every minute, so I light",
        "             and extinguish the lamp every 60 seconds.",
        "Prince: So your days last one minute?",
        "Lamplighter: Yes. While we've talked, a month has passed.",
        "Prince: I like you. You're faithful to your work. I can help",
        "        you rest. Just walk slowly and you'll stay in the",
        "        sunlight and avoid lighting the lamp.",
        "Lamplighter: But I love to sleep.",
        "Prince: Then you're unlucky.",
        "Lamplighter: I am. Good morning. (puts out the lamp)",
        "Prince (to himself): The others would laugh at him. But he's",
        "                     the only one who thinks beyond himself. I",
        "                     could have made him a friend...But his",
        "                     planet is too small. And what I'll miss",
        "                     most... is all those sunsets."
    };
    std::vector<std::string> geographerConvo = {
        "Geographer: Oh! An explorer!",
        "Prince: What's that big book? What do you do?",
        "Geographer: I'm a geographer. I know where all the seas,",
        "            mountains, rivers, and deserts are.",
        "Prince: Finally, someone with a real job! Is your planet",
        "        beautiful? Does it have oceans?",
        "Geographer: I couldn't say.",
        "Prince: Mountains?",
        "Geographer: I couldn't say.",
        "Prince: But... you're a geographer!",
        "Geographer: Yes, but I don't explore. I collect reports from",
        "            explorers. I ask questions and write down what",
        "            they tell me, if they're trustworthy.",
        "Prince: Why check if they're trustworthy?",
        "Geographer: A liar or a drunk might give false information.",
        "            A drunk might see double mountains.",
        "Prince: I know someone who'd be a bad explorer...",
        "Geographer: But you, you’re an explorer!",
        "            Tell me about your planet!",
        "Prince: It’s small. I have three volcanoes, two active, one",
        "        maybe extinct. And a flower.",
        "Geographer: We don't record flowers. They are ephemeral.",
        "Prince: What does ephemeral mean?",
        "Geographer: It means: in danger of disappearing.",
        "Prince (to himself): My flower is ephemeral...",
        "                     And I left her alone on my planet...",
        "Prince: Where should I go next?",
        "Geographer: Try the planet Earth. It has a good reputation."
    };

    std::vector<std::string> flowersConvo = {"..."};

    std::vector<std::string> foxConvo = {"..."};

    sf::Text dialogueText;
    dialogueText.setFont(font);
    dialogueText.setCharacterSize(24);
    dialogueText.setFillColor(sf::Color::White);

    sf::RectangleShape dialogueBox(sf::Vector2f(width - 100, 100));
    dialogueBox.setFillColor(sf::Color(0, 0, 0, 150));

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

    sf::Texture flowersTex;
    if (!flowersTex.loadFromFile("files/images/Fox.png")) {
        std::cout << "Error loading fox image" << std::endl;
    }
    sf::Sprite flowers;
    flowers.setTexture(flowersTex);
    flowers.setPosition(sf::Vector2f(600, 200));

    sf::Texture foxTex;
    if (!foxTex.loadFromFile("files/images/Fox.png")) {
        std::cout << "Error loading fox image" << std::endl;
    }
    sf::Sprite fox;
    fox.setTexture(foxTex);
    fox.setPosition(sf::Vector2f(1000, height - 500));

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
                        onEarth = true;
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
                        if (position.x > 350 && position.x < 650 && position.y > 300 && position.y < 500 && !seenFlower) {
                            currentConvo = &flowerConvo;
                            visibleLines.clear();
                            currentLineIndex = 0;
                            visibleLines.push_back((*currentConvo)[currentLineIndex]);
                            updateDialogueDisplay(dialogueText, dialogueBox, visibleLines, font, 24, width);
                            dialogueClock.restart();
                            inFlowerConvo = true;
                            inKingConvo = false;
                            inVainConvo = false;
                            inDrunkardConvo = false;
                            inBusinessmanConvo = false;
                            inLamplighterConvo = false;
                            inGeographerConvo = false;
                            seenFlower = true;
                            std::cout << "Flower seen" << std::endl;
                        } else if (position.x > 1100 && position.x < 1400 && position.y > 200 && position.y < 400 && !seenKing) {
                            currentConvo = &kingConvo;
                            visibleLines.clear();
                            currentLineIndex = 0;
                            visibleLines.push_back((*currentConvo)[currentLineIndex]);
                            updateDialogueDisplay(dialogueText, dialogueBox, visibleLines, font, 24, width);
                            dialogueClock.restart();
                            inFlowerConvo = false;
                            inKingConvo = true;
                            inVainConvo = false;
                            inDrunkardConvo = false;
                            inBusinessmanConvo = false;
                            inLamplighterConvo = false;
                            inGeographerConvo = false;
                            seenKing = true;
                            std::cout << "King seen" << std::endl;
                        } else if (position.x > 1950 && position.x < 2250 && position.y > 400 && position.y < 600 && !seenVainMan) {
                            currentConvo = &vainConvo;
                            visibleLines.clear();
                            currentLineIndex = 0;
                            visibleLines.push_back((*currentConvo)[currentLineIndex]);
                            updateDialogueDisplay(dialogueText, dialogueBox, visibleLines, font, 24, width);
                            dialogueClock.restart();
                            inFlowerConvo = false;
                            inKingConvo = false;
                            inVainConvo = true;
                            inDrunkardConvo = false;
                            inBusinessmanConvo = false;
                            inLamplighterConvo = false;
                            inGeographerConvo = false;
                            seenVainMan = true;
                            std::cout << "Vain Man seen" << std::endl;
                        } else if (position.x > 2750 && position.x < 3050 && position.y > 400 && position.y < 600 && !seenDrunkard) {
                            currentConvo = &drunkardConvo;
                            visibleLines.clear();
                            currentLineIndex = 0;
                            visibleLines.push_back((*currentConvo)[currentLineIndex]);
                            updateDialogueDisplay(dialogueText, dialogueBox, visibleLines, font, 24, width);
                            dialogueClock.restart();
                            inFlowerConvo = false;
                            inKingConvo = false;
                            inVainConvo = false;
                            inDrunkardConvo = true;
                            inBusinessmanConvo = false;
                            inLamplighterConvo = false;
                            inGeographerConvo = false;
                            seenDrunkard = true;
                            std::cout << "Drunkard seen" << std::endl;
                        } else if (position.x > 3550 && position.x < 3850 && position.y > 300 && position.y < 500 && !seenBusinessman) {
                            currentConvo = &businessmanConvo;
                            visibleLines.clear();
                            currentLineIndex = 0;
                            visibleLines.push_back((*currentConvo)[currentLineIndex]);
                            updateDialogueDisplay(dialogueText, dialogueBox, visibleLines, font, 24, width);
                            dialogueClock.restart();
                            inFlowerConvo = false;
                            inKingConvo = false;
                            inVainConvo = false;
                            inDrunkardConvo = false;
                            inBusinessmanConvo = true;
                            inLamplighterConvo = false;
                            inGeographerConvo = false;
                            seenBusinessman = true;
                            std::cout << "Businessman seen" << std::endl;
                        } else if (position.x > 4350 && position.x < 4650 && position.y > 410 && position.y < 610 && !seenLamplighter) {
                            currentConvo = &lamplighterConvo;
                            visibleLines.clear();
                            currentLineIndex = 0;
                            visibleLines.push_back((*currentConvo)[currentLineIndex]);
                            updateDialogueDisplay(dialogueText, dialogueBox, visibleLines, font, 24, width);
                            dialogueClock.restart();
                            inFlowerConvo = false;
                            inKingConvo = false;
                            inVainConvo = false;
                            inDrunkardConvo = false;
                            inBusinessmanConvo = false;
                            inLamplighterConvo = true;
                            inGeographerConvo = false;
                            seenLamplighter = true;
                            std::cout << "Lamplighter seen" << std::endl;
                        } else if (position.x > 5150 && position.x < 5450 && position.y > 310 && position.y < 510 && !seenGeographer) {
                            currentConvo = &geographerConvo;
                            visibleLines.clear();
                            currentLineIndex = 0;
                            visibleLines.push_back((*currentConvo)[currentLineIndex]);
                            updateDialogueDisplay(dialogueText, dialogueBox, visibleLines, font, 24, width);
                            dialogueClock.restart();
                            inFlowerConvo = false;
                            inKingConvo = false;
                            inVainConvo = false;
                            inDrunkardConvo = false;
                            inBusinessmanConvo = false;
                            inLamplighterConvo = false;
                            inGeographerConvo = true;
                            seenGeographer = true;
                            std::cout << "Geographer seen" << std::endl;
                        }
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
                if (position.x > 5600 && seenFlower && seenKing && seenVainMan && seenDrunkard && seenBusinessman && seenLamplighter && seenGeographer) {
                    game.close();
                    gameStart = false;
                    onEarth = true;
                    std::cout << "Going to Earth" << std::endl;
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

            if (currentConvo && dialogueClock.getElapsedTime().asSeconds() >= dialogueDelay) {
                currentLineIndex++;
                if (currentLineIndex < currentConvo->size()) {
                    visibleLines.push_back((*currentConvo)[currentLineIndex]);
                    if (visibleLines.size() > 8) {
                        visibleLines.erase(visibleLines.begin());
                    }
                    updateDialogueDisplay(dialogueText, dialogueBox, visibleLines, font, 24, width);
                    dialogueClock.restart();
                }
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
            if (inFlowerConvo) {
                dialogueBox.setPosition(width / 2, height / 2 - 300);
            } else if (inKingConvo) {
                dialogueBox.setPosition(width / 2 + 750, height / 2 - 400);
            } else if (inVainConvo) {
                dialogueBox.setPosition(width / 2 + 1600, height / 2 - 200);
            } else if (inDrunkardConvo) {
                dialogueBox.setPosition(width / 2 + 2400, height / 2 - 200);
            } else if (inBusinessmanConvo) {
                dialogueBox.setPosition(width / 2 + 3200, height / 2 - 300);
            } else if (inLamplighterConvo) {
                dialogueBox.setPosition(width / 2 + 4000, height / 2 - 190);
            } else if (inGeographerConvo) {
                dialogueBox.setPosition(width / 2 + 4800, height / 2 - 290);
            }
            if (inFlowerConvo || inKingConvo || inVainConvo || inDrunkardConvo || inBusinessmanConvo || inLamplighterConvo || inGeographerConvo) {
                dialogueText.setPosition(dialogueBox.getPosition().x - dialogueBox.getSize().x / 2 + 20, dialogueBox.getPosition().y - dialogueBox.getSize().y / 2 + 20);
                game.draw(dialogueBox);
                game.draw(dialogueText);
            }
            game.display();
        }
    }
    if (onEarth) {
        sf::RenderWindow earth(sf::VideoMode(width, height), "Earth", sf::Style::Close);
        sf::Vector2f position = prince.getPosition();
        gravity = 980.0f;

        sf::Texture grassTex;
        if (!grassTex.loadFromFile("files/images/grass.png")) {
            std::cout << "Error loading grass image" << std::endl;
        }
        std::vector<sf::Sprite> grassTiles;
        for (float x = -84.0f; x < 10000; x += 1623) {
            sf::Sprite grass;
            grass.setTexture(grassTex);
            grass.setPosition(sf::Vector2f(x, height - 332.0f));
            grassTiles.push_back(grass);
        }

        sf::View view(sf::FloatRect(0, 0, width, height));
        view.setCenter(prince.getPosition().x, height/2);
        clock.restart();
        while(earth.isOpen()) {
            sf::Event event;
            while(earth.pollEvent(event)) {
                if(event.type == sf::Event::Closed) {
                    earth.close();
                    onEarth = false;
                }
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Escape) {
                        earth.close();
                        onEarth = false;
                    }
                    if (event.key.code == sf::Keyboard::Down) {
                        if (position.x > 500 && position.x < 800 && position.y > 0 && position.y < 1000 && !seenFlowers) {
                            currentConvo = &flowersConvo;
                            visibleLines.clear();
                            currentLineIndex = 0;
                            visibleLines.push_back((*currentConvo)[currentLineIndex]);
                            updateDialogueDisplay(dialogueText, dialogueBox, visibleLines, font, 24, width);
                            dialogueClock.restart();
                            inFlowersConvo = true;
                            seenFlowers = true;
                            std::cout << "Flowers seen" << std::endl;
                        } else if (position.x > 900 && position.x < 1200 && position.y > 0 && position.y < 1000 && !seenFox) {
                            currentConvo = &foxConvo;
                            visibleLines.clear();
                            currentLineIndex = 0;
                            visibleLines.push_back((*currentConvo)[currentLineIndex]);
                            updateDialogueDisplay(dialogueText, dialogueBox, visibleLines, font, 24, width);
                            dialogueClock.restart();
                            inFoxConvo = true;
                            seenFox = true;
                            std::cout << "Fox seen" << std::endl;
                        }
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

            //landing
            if (position.y >= height - 200) {
                position.y = height - 200;
                Yvelocity = 0.0f;
                inAir = false;
            }

            if (position.x < prince.getSize().x / 2) {
                position.x = prince.getSize().x / 2;
            }

            if (currentConvo && dialogueClock.getElapsedTime().asSeconds() >= dialogueDelay) {
                currentLineIndex++;
                if (currentLineIndex < currentConvo->size()) {
                    visibleLines.push_back((*currentConvo)[currentLineIndex]);
                    if (visibleLines.size() > 8) {
                        visibleLines.erase(visibleLines.begin());
                    }
                    updateDialogueDisplay(dialogueText, dialogueBox, visibleLines, font, 24, width);
                    dialogueClock.restart();
                }
            }

            prince.setPosition(position);

            float camX = prince.getPosition().x;
            camX = std::max(width / 2.0f, camX);
            camX = std::min(10000.0f - width / 2.0f, camX);
            view.setCenter(camX, height / 2);

            earth.clear(sf::Color(255, 238, 198));
            earth.setView(view);
            if (runL && !runR) {
                earth.draw(prince);
            } else if (runR && !runL) {
                earth.draw(prince);
            } else {
                earth.draw(prince);
            }
            runL = false;
            runR = false;
            if (seenFox) {
                for (auto& tile : grassTiles) {
                    earth.draw(tile);
                }
                earth.draw(fox);
            } else {
                earth.draw(fox);
                for (auto& tile : grassTiles) {
                    earth.draw(tile);
                }
            }
            earth.draw(flowers);
            if (inFlowersConvo) {
                dialogueBox.setPosition(875, height / 2 - 300);
            } else if (inFoxConvo) {
                dialogueBox.setPosition(875, height / 2 - 300);
            }
            if (inFlowersConvo || inFoxConvo) {
                dialogueText.setPosition(dialogueBox.getPosition().x - dialogueBox.getSize().x / 2 + 20, dialogueBox.getPosition().y - dialogueBox.getSize().y / 2 + 20);
                earth.draw(dialogueBox);
                earth.draw(dialogueText);
            }
            earth.display();
        }
    }
    return 0;
}