module;

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

export module pvz.game;

import std;
import pvz.sprites;
import pvz.game_types;

export namespace pvz {

    using PlantType = types::PlantType;
    using ZombieType = types::ZombieType;
    using Plant = types::Plant;
    using Zombie = types::Zombie;
    using Bullet = types::Bullet;
    using Lawnmower = types::Lawnmower;

    class Application {
    public:
        Application() {
            const auto desktopMode = sf::VideoMode::getDesktopMode();
            sf::RenderWindow window{
                sf::VideoMode{{desktopMode.size.x, desktopMode.size.y}},
                "Mini Plants vs Zombies (SFML)",
                sf::State::Fullscreen
            };
            window.setFramerateLimit(60);

            sf::View gameView{sf::FloatRect{{0.f, 0.f}, {800.f, 600.f}}};
            window.setView(gameView);

            sf::Font uiFont;
            bool uiFontLoaded = uiFont.openFromFile("/System/Library/Fonts/Supplemental/Arial.ttf");
            if (!uiFontLoaded) {
                std::cerr << "⚠️  Impossible de charger la police Arial\n";
            } else {
                std::cout << "✅ Police chargée avec succès\n";
            }
            sf::Texture shooterTex = pvz::sprites::createShooterSprite();
            sf::Texture sunflowerTex = pvz::sprites::createSunflowerSprite();
            sf::Texture potatoTex = pvz::sprites::createPotatoSprite();
            sf::Texture heavyTex = pvz::sprites::createHeavyShooterSprite();
            sf::Texture zombieTex = pvz::sprites::createZombieSprite();
            sf::Texture tankZombieTex = pvz::sprites::createTankZombieSprite();
            sf::Texture sprinterZombieTex = pvz::sprites::createSprinterZombieSprite();
            sf::Texture bossZombieTex = pvz::sprites::createBossZombieSprite();
            sf::Texture lawnmowerTex = pvz::sprites::createLawnmowerSprite();

            const std::array<float, 5> lanesY{220.f, 300.f, 380.f, 460.f, 540.f};
            const std::array<float, 7> slotXs{90.f, 160.f, 230.f, 300.f, 370.f, 440.f, 510.f};
            std::array<std::array<bool, 7>, 5> slotOccupied{};
            std::array<bool, 5> lawnmowersAvailable{true, true, true, true, true};

            std::vector<Plant>  plants;
            std::vector<Zombie> zombies;
            std::vector<Bullet> bullets;
            std::vector<Lawnmower> activeLawnmowers;

            int zombiesKilled = 0;
            bool gameOver = false;

            int   money             = 300;
            const int shooterCost   = 75;
            const int sunflowerCost = 50;
            const int potatoCost    = 150;
            const int heavyCost     = 175;
            const int rewardPerKill = 15;

            const float sunflowerInterval = 5.f;
            const int   sunflowerIncome   = 25;

PlantType selectedPlantType = PlantType::Sunflower;

            sf::Clock clock;
            float gameTime = 0.f;
            float alertTimer = 0.f;

            float baseZombieSpeed = 40.f;
            float baseZombieSpawnInterval = 3.f;
            int   baseZombieHealth = 5;

            float zombieSpawnTimer = 0.f;
            float zombieSpawnInterval = baseZombieSpawnInterval;

            sf::SoundBuffer shootBuffer = pvz::sprites::generateSoundBuffer(800.f, 0.1f);
            sf::SoundBuffer hitBuffer = pvz::sprites::generateSoundBuffer(400.f, 0.15f);
            sf::SoundBuffer killBuffer = pvz::sprites::generateSoundBuffer(200.f, 0.2f);
            sf::SoundBuffer plantDestroyedBuffer = pvz::sprites::generateSoundBuffer(300.f, 0.25f);
            sf::SoundBuffer zombieReachedEndBuffer = pvz::sprites::generateSoundBuffer(150.f, 0.3f);
            sf::SoundBuffer dangerZoneBuffer       = pvz::sprites::generateSoundBuffer(1000.f, 0.1f);

            sf::Sound shootSound{shootBuffer};
            sf::Sound hitSound{hitBuffer};
            sf::Sound killSound{killBuffer};
            sf::Sound plantDestroyedSound{plantDestroyedBuffer};
            sf::Sound zombieReachedEndSound{zombieReachedEndBuffer};
            sf::Sound dangerZoneSound{dangerZoneBuffer};
            shootSound.setVolume(30.f);
            hitSound.setVolume(40.f);
            killSound.setVolume(50.f);
            plantDestroyedSound.setVolume(40.f);
            zombieReachedEndSound.setVolume(60.f);
            dangerZoneSound.setVolume(50.f);

            while (window.isOpen()) {
                const float dt = clock.restart().asSeconds();
                
                if (gameOver) {
                    while (auto eventOpt = window.pollEvent()) {
                        const sf::Event& event = *eventOpt;
                        if (event.is<sf::Event::Closed>()) {
                            window.close();
                            break;
                        }
                        else if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
                            if (mousePressed->button == sf::Mouse::Button::Left) {
                                const sf::Vector2f mousePos = window.mapPixelToCoords(mousePressed->position, gameView);
                                const float mouseX = mousePos.x;
                                const float mouseY = mousePos.y;
                                
                                if (mouseX >= 250.f && mouseX <= 350.f && mouseY >= 400.f && mouseY <= 450.f) {
                                    gameOver = false;
                                    gameTime = 0.f;
                                    zombieSpawnTimer = 0.f;
                                    money = 300;
                                    zombiesKilled = 0;
                                    plants.clear();
                                    zombies.clear();
                                    bullets.clear();
                                    activeLawnmowers.clear();
                                    slotOccupied = {};
                                    lawnmowersAvailable = {true, true, true, true, true};
                                    clock.restart();
                                    continue;
                                }
                                
                                if (mouseX >= 450.f && mouseX <= 550.f && mouseY >= 400.f && mouseY <= 450.f) {
                                    window.close();
                                    break;
                                }
                            }
                        }
                    }
                    
                    window.clear(sf::Color(20, 20, 40));
                    
                    if (uiFontLoaded) {
                        sf::Text gameOverText{uiFont, "GAME OVER", 48};
                        gameOverText.setFillColor(sf::Color::Red);
                        gameOverText.setPosition({250.f, 150.f});
                        window.draw(gameOverText);
                        
                        const int minutes = static_cast<int>(gameTime) / 60;
                        const int seconds = static_cast<int>(gameTime) % 60;
                        sf::Text statsText{uiFont, 
                            std::format("Time Survived: {:02d}:{:02d}\nZombies Killed: {}\nFinal Money: {}", 
                                       minutes, seconds, zombiesKilled, money), 
                            24};
                        statsText.setFillColor(sf::Color::White);
                        statsText.setPosition({250.f, 250.f});
                        window.draw(statsText);
                        
                        sf::RectangleShape restartButton{{100.f, 50.f}};
                        restartButton.setPosition({250.f, 400.f});
                        restartButton.setFillColor(sf::Color(0, 150, 0));
                        restartButton.setOutlineColor(sf::Color::White);
                        restartButton.setOutlineThickness(2.f);
                        window.draw(restartButton);
                        
                        sf::Text restartText{uiFont, "Restart", 20};
                        restartText.setFillColor(sf::Color::White);
                        restartText.setPosition({270.f, 415.f});
                        window.draw(restartText);
                        
                        sf::RectangleShape quitButton{{100.f, 50.f}};
                        quitButton.setPosition({450.f, 400.f});
                        quitButton.setFillColor(sf::Color(150, 0, 0));
                        quitButton.setOutlineColor(sf::Color::White);
                        quitButton.setOutlineThickness(2.f);
                        window.draw(quitButton);
                        
                        sf::Text quitText{uiFont, "Quit", 20};
                        quitText.setFillColor(sf::Color::White);
                        quitText.setPosition({480.f, 415.f});
                        window.draw(quitText);
                    }
                    
                    window.display();
                    continue;
                }
                
                zombieSpawnTimer += dt;
                gameTime += dt;

                const float difficultyMultiplier = 1.f + (gameTime / 60.f);
                const float currentZombieSpeed = baseZombieSpeed * difficultyMultiplier;
                zombieSpawnInterval = std::max(0.5f, baseZombieSpawnInterval / difficultyMultiplier);
                const int currentZombieHealth = baseZombieHealth + static_cast<int>(gameTime / 30.f);

                while (auto eventOpt = window.pollEvent()) {
                    const sf::Event& event = *eventOpt;

                    if (event.is<sf::Event::Closed>()) {
                        window.close();
                    }
                    else if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
                        if (keyPressed->code == sf::Keyboard::Key::D) {
                            gameTime += 60.f;
                        }
                        else if (keyPressed->code == sf::Keyboard::Key::S) {
                            money += 1000;
                        }
                    }
                    else if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
                        const sf::Vector2f mousePos = window.mapPixelToCoords(mousePressed->position, gameView);
                        const float mouseX = mousePos.x;
                        const float mouseY = mousePos.y;

                        if (mousePressed->button == sf::Mouse::Button::Right) {
                            for (auto it = plants.begin(); it != plants.end(); ++it) {
                                const auto plantBounds = it->sprite->getGlobalBounds();
                                if (plantBounds.contains({mouseX, mouseY})) {
                                    int sellPrice = 0;
                                    switch (it->type) {
                                        case PlantType::Shooter:      sellPrice = static_cast<int>(shooterCost * 0.75f);   break;
                                        case PlantType::Sunflower:    sellPrice = static_cast<int>(sunflowerCost * 0.75f); break;
                                        case PlantType::Potato:       sellPrice = static_cast<int>(potatoCost * 0.75f);    break;
                                        case PlantType::HeavyShooter: sellPrice = static_cast<int>(heavyCost * 0.75f);     break;
                                    }
                                    
                                    money += sellPrice;
                                    slotOccupied[it->rowIndex][it->colIndex] = false;
                                    
                                    plants.erase(it);
                                    break;
                                }
                            }
                            continue;
                        }

                        if (mousePressed->button == sf::Mouse::Button::Left) {
                            const sf::FloatRect shooterCardArea{{450.f, 20.f}, {120.f, 35.f}};
                            const sf::FloatRect sunflowerCardArea{{580.f, 20.f}, {120.f, 35.f}};
                            const sf::FloatRect potatoCardArea{{450.f, 65.f}, {120.f, 35.f}};
                            const sf::FloatRect heavyCardArea{{580.f, 65.f}, {120.f, 35.f}};

                            if (shooterCardArea.contains({mouseX, mouseY})) {
                                selectedPlantType = PlantType::Shooter;
                                continue;
                            }
                            if (sunflowerCardArea.contains({mouseX, mouseY})) {
                                selectedPlantType = PlantType::Sunflower;
                                continue;
                            }
                            if (potatoCardArea.contains({mouseX, mouseY})) {
                                selectedPlantType = PlantType::Potato;
                                continue;
                            }
                            if (heavyCardArea.contains({mouseX, mouseY})) {
                                selectedPlantType = PlantType::HeavyShooter;
                                continue;
                            }

                            int currentCost = 0;
                            switch (selectedPlantType) {
                                case PlantType::Shooter:      currentCost = shooterCost;   break;
                                case PlantType::Sunflower:    currentCost = sunflowerCost; break;
                                case PlantType::Potato:       currentCost = potatoCost;    break;
                                case PlantType::HeavyShooter: currentCost = heavyCost;     break;
                            }
                            if (money < currentCost) {
                                continue;
                            }

                            int   selectedRow = -1;
                            int   selectedCol = -1;
                            const float cellHalfWidth  = 40.f;
                            const float cellHalfHeight = 35.f;

                            for (int r = 0; r < static_cast<int>(lanesY.size()); ++r) {
                                for (int c = 0; c < static_cast<int>(slotXs.size()); ++c) {
                                    const float centerX = slotXs[c];
                                    const float centerY = lanesY[r];
                                    if (std::abs(mouseX - centerX) <= cellHalfWidth &&
                                        std::abs(mouseY - centerY) <= cellHalfHeight) {
                                        selectedRow = r;
                                        selectedCol = c;
                                        break;
                                    }
                                }
                                if (selectedRow != -1) {
                            break;
                                }
                            }

                            if (selectedRow != -1 && selectedCol != -1 &&
                                !slotOccupied[selectedRow][selectedCol]) {
Plant plant;
                                switch (selectedPlantType) {
                                    case PlantType::Shooter:
                                        plant.sprite = sf::Sprite{shooterTex};
                                        break;
                                    case PlantType::Sunflower:
                                        plant.sprite = sf::Sprite{sunflowerTex};
                                        break;
                                    case PlantType::Potato:
                                        plant.sprite = sf::Sprite{potatoTex};
                                        break;
                                    case PlantType::HeavyShooter:
                                        plant.sprite = sf::Sprite{heavyTex};
                            break;
                        }
                                plant.sprite->setScale({1.5f, 1.5f});
                                const auto texSize = plant.sprite->getTexture().getSize();
                                plant.sprite->setOrigin({texSize.x / 2.f, texSize.y / 2.f});
                                const float centerX = slotXs[selectedCol];
                                const float centerY = lanesY[selectedRow];
                                plant.sprite->setPosition({centerX, centerY});
                                plant.rowIndex = selectedRow;
                                plant.colIndex = selectedCol;
                                plant.type     = selectedPlantType;
                                switch (selectedPlantType) {
                                    case PlantType::Shooter:
                                        plant.maxHealth    = 3.f;
                                        plant.health       = 3.f;
                                        plant.shootCooldown = 0.f;
                                        break;
                                    case PlantType::Sunflower:
                                        plant.maxHealth    = 2.5f;
                                        plant.health       = 2.5f;
                                        plant.shootCooldown = 0.f;
                                        plant.sunCooldown   = sunflowerInterval;
                                        break;
                                    case PlantType::Potato:
                                        plant.maxHealth    = 10.f;
                                        plant.health       = 10.f;
                                        plant.shootCooldown = 0.f;
                                        break;
                                    case PlantType::HeavyShooter:
                                        plant.maxHealth    = 3.5f;
                                        plant.health       = 3.5f;
                                        plant.shootCooldown = 0.f;
                                        break;
                                }
                                plants.push_back(plant);

                                slotOccupied[selectedRow][selectedCol] = true;

                                money -= currentCost;
                            }
                        }
                    }
                }

                if (zombieSpawnTimer >= zombieSpawnInterval) {
                    zombieSpawnTimer = 0.f;

                    static std::mt19937 rng{std::random_device{}()};
                    std::uniform_int_distribution<std::size_t> laneDist(0, lanesY.size() - 1);
                    const float laneYForZombie = lanesY[laneDist(rng)];

ZombieType zombieType = ZombieType::Normal;
                    std::uniform_real_distribution<float> typeDist(0.f, 1.f);
                    const float typeRoll = typeDist(rng);

                    if (difficultyMultiplier >= 5.0f && typeRoll < 0.01f) {
                        zombieType = ZombieType::Boss;
                    }
                    else if (difficultyMultiplier >= 2.0f) {
                        const float tankChance = std::min(0.5f, 0.3f + (difficultyMultiplier - 2.0f) * 0.1f);
                        if (typeRoll < tankChance) {
                            zombieType = ZombieType::Tank;
                        }
                    }
                    if (zombieType == ZombieType::Normal && difficultyMultiplier >= 3.0f) {
                        const float sprinterChance = std::min(0.4f, 0.2f + (difficultyMultiplier - 3.0f) * 0.1f);
                        if (typeRoll < sprinterChance) {
                            zombieType = ZombieType::Sprinter;
                        }
                    }

                    Zombie z;
                    z.type = zombieType;

                    switch (zombieType) {
                        case ZombieType::Normal:
                            z.sprite = sf::Sprite{zombieTex};
                            z.speed = currentZombieSpeed;
                            z.maxHealth = currentZombieHealth;
                            z.health = currentZombieHealth;
                            break;

                        case ZombieType::Tank:
                            z.sprite = sf::Sprite{tankZombieTex};
                            z.speed = currentZombieSpeed;
                            z.maxHealth = currentZombieHealth * 3;
                            z.health = currentZombieHealth * 3;
                            break;

                        case ZombieType::Sprinter:
                            z.sprite = sf::Sprite{sprinterZombieTex};
                            z.speed = currentZombieSpeed * 2.f;
                            z.maxHealth = std::max(2, (currentZombieHealth + 1) / 2);
                            z.health = z.maxHealth;
                            break;

                        case ZombieType::Boss:
                            z.sprite = sf::Sprite{bossZombieTex};
                            z.speed = currentZombieSpeed * 0.7f;
                            z.maxHealth = currentZombieHealth * 20 + static_cast<int>(difficultyMultiplier * 10);
                            z.health = z.maxHealth;
                            z.sprite->setScale({2.0f, 2.0f});
                            break;
                    }

                    if (zombieType != ZombieType::Boss) {
                        z.sprite->setScale({1.5f, 1.5f});
                    }
                    const auto zTexSize = z.sprite->getTexture().getSize();
                    z.sprite->setOrigin({zTexSize.x / 2.f, zTexSize.y / 2.f});
                    z.sprite->setPosition({780.f, laneYForZombie});
                    zombies.push_back(z);
                }

                for (auto &plant : plants) {
                    switch (plant.type) {
                        case PlantType::Sunflower:
                            plant.sunCooldown -= dt;
                            if (plant.sunCooldown <= 0.f) {
                                money += sunflowerIncome;
                                plant.sunCooldown = sunflowerInterval;
                            }
                            break;

                        case PlantType::Potato:
                            break;

                        case PlantType::Shooter:
                        case PlantType::HeavyShooter: {
                            plant.shootCooldown -= dt;
                            const float shootInterval =
                                (plant.type == PlantType::Shooter) ? 1.5f : 0.9f;

                            if (plant.shootCooldown <= 0.f) {
                                bool hasTarget = false;
                                for (const auto &z : zombies) {
                                    if (z.sprite->getPosition().x > plant.sprite->getPosition().x) {
                                        hasTarget = true;
                            break;
                        }
                    }
                                if (hasTarget) {
Bullet b;
                                    b.shape.setRadius(8.f);
                                    b.shape.setFillColor(plant.type == PlantType::Shooter
                                                         ? sf::Color::Yellow
                                                         : sf::Color(255, 100, 0));
                                    b.shape.setOrigin({b.shape.getRadius(), b.shape.getRadius()});
                                    b.shape.setPosition(plant.sprite->getPosition());
                                    b.damage = (plant.type == PlantType::Shooter) ? 1 : 3;
                                    bullets.push_back(b);
                                    plant.shootCooldown = shootInterval;
                                    shootSound.play();
                                }
                            }
                            break;
                        }
                    }
                }

                for (auto &z : zombies) {
                    bool isChewingPlant = false;

                    for (auto &p : plants) {
                        const auto zombieBounds = z.sprite->getGlobalBounds();
                        const auto plantBounds  = p.sprite->getGlobalBounds();
                        const bool intersects   = zombieBounds.findIntersection(plantBounds).has_value();

                        if (intersects) {
                            isChewingPlant = true;
                            p.health -= 1.5f * dt;
                        }
                    }

                    if (!isChewingPlant) {
                        z.sprite->move({-z.speed * dt, 0.f});
                    }

                    if (z.sprite->getPosition().x < -10.f && z.health > 0) {
                        int zombieLane = -1;
                        for (int i = 0; i < static_cast<int>(lanesY.size()); ++i) {
                            if (std::abs(z.sprite->getPosition().y - lanesY[i]) < 50.f) {
                                zombieLane = i;
                                break;
                            }
                        }

                        if (zombieLane >= 0 && zombieLane < static_cast<int>(lawnmowersAvailable.size())) {
                            if (lawnmowersAvailable[zombieLane]) {
                                lawnmowersAvailable[zombieLane] = false;
                                Lawnmower lawnmower;
                                lawnmower.sprite = sf::Sprite{lawnmowerTex};
                                lawnmower.sprite->setScale({1.5f, 1.5f});
                                const auto lmTexSize = lawnmower.sprite->getTexture().getSize();
                                lawnmower.sprite->setOrigin({lmTexSize.x / 2.f, lmTexSize.y / 2.f});
                                lawnmower.x = -30.f;
                                lawnmower.lane = zombieLane;
                                lawnmower.sprite->setPosition({lawnmower.x, lanesY[zombieLane]});
                                activeLawnmowers.push_back(lawnmower);
                                
                                killSound.play();
                                z.sprite->setPosition({-100.f, z.sprite->getPosition().y});
            } else {
                                gameOver = true;
                                break;
                            }
            } else {
                            z.sprite->setPosition({-100.f, z.sprite->getPosition().y});
                        }
                    }
                }

                bool dangerZoneActive = false;
                if (!zombies.empty()) {
                    const float minDangerX = slotXs.front() - 60.f;
                    const float maxDangerX = slotXs.back() + 60.f;
                    for (const auto& z : zombies) {
                        const float zx = z.sprite->getPosition().x;
                        if (zx >= minDangerX && zx <= maxDangerX) {
                            dangerZoneActive = true;
                            break;
                        }
                    }
                }

                if (dangerZoneActive) {
                    alertTimer += dt;
                    if (alertTimer >= 0.8f) {
                        dangerZoneSound.play();
                        alertTimer = 0.f;
                    }
            } else {
                    alertTimer = 0.f;
                }

                const std::size_t plantsBefore = plants.size();
                plants.erase(
                    std::remove_if(plants.begin(), plants.end(),
                                   [&slotOccupied](const Plant& p) {
                                       if (p.health <= 0.f) {
                                           if (p.rowIndex >= 0 && p.rowIndex < 5 &&
                                               p.colIndex >= 0 && p.colIndex < 7) {
                                               slotOccupied[p.rowIndex][p.colIndex] = false;
                                           }
                                           return true;
                                       }
                                       return false;
                                   }),
                    plants.end());
                if (plants.size() < plantsBefore) {
                    plantDestroyedSound.play();
                }

                zombies.erase(
                    std::remove_if(zombies.begin(), zombies.end(),
                                   [](const Zombie &z) { return z.sprite->getPosition().x < -50.f; }),
                    zombies.end());

                for (auto &lm : activeLawnmowers) {
                    lm.x += lm.speed * dt;
                    lm.sprite->setPosition({lm.x, lanesY[lm.lane]});
                    
                    for (auto &z : zombies) {
                        if (z.health > 0) {
                            int zLane = -1;
                            for (int i = 0; i < static_cast<int>(lanesY.size()); ++i) {
                                if (std::abs(z.sprite->getPosition().y - lanesY[i]) < 50.f) {
                                    zLane = i;
                                    break;
                                }
                            }
                            
                            if (zLane == lm.lane) {
                                const auto lmBounds = lm.sprite->getGlobalBounds();
                                const auto zBounds = z.sprite->getGlobalBounds();
                                if (lmBounds.findIntersection(zBounds).has_value()) {
                                    z.health = 0;
                                    z.sprite->setPosition({-1000.f, z.sprite->getPosition().y});
                                    zombiesKilled++;
                                    money += rewardPerKill;
                                }
                            }
                        }
                    }
                }
                
                activeLawnmowers.erase(
                    std::remove_if(activeLawnmowers.begin(), activeLawnmowers.end(),
                                   [](const Lawnmower &lm) { return lm.x > 850.f; }),
                    activeLawnmowers.end());

                for (auto &b : bullets) {
                    b.shape.move({b.speed * dt, 0.f});
                }
                bullets.erase(
                    std::remove_if(bullets.begin(), bullets.end(),
                                   [](const Bullet &b) { return b.shape.getPosition().x > 850.f; }),
                    bullets.end());

                for (auto &b : bullets) {
                    for (auto &z : zombies) {
                        const auto bulletBounds  = b.shape.getGlobalBounds();
                        const auto zombieBounds  = z.sprite->getGlobalBounds();
                        const bool  intersects   = bulletBounds.findIntersection(zombieBounds).has_value();

                        if (intersects) {
                            z.health -= b.damage;

                            b.shape.setPosition({900.f, b.shape.getPosition().y});

                            if (z.health <= 0) {
                                money += rewardPerKill;
                                zombiesKilled++;
                                z.sprite->setPosition({-1000.f, z.sprite->getPosition().y});
                                killSound.play();
            } else {
                                hitSound.play();
                            }
                        }
                    }
                }
                zombies.erase(
                    std::remove_if(zombies.begin(), zombies.end(),
                                   [](const Zombie &z) { 
                                       return z.sprite->getPosition().x < -50.f || z.health <= 0; 
                                   }),
                    zombies.end());
                bullets.erase(
                    std::remove_if(bullets.begin(), bullets.end(),
                                   [](const Bullet &b) { return b.shape.getPosition().x > 850.f; }),
                    bullets.end());

                window.clear(sf::Color(30, 30, 60));

                sf::RectangleShape laneLine{{800.f, 5.f}};
                laneLine.setFillColor(sf::Color(0, 150, 0));
                for (float laneYValue : lanesY) {
                    laneLine.setPosition({0.f, laneYValue});
                    window.draw(laneLine);
                }

                for (int i = 0; i < static_cast<int>(lanesY.size()); ++i) {
                    sf::RectangleShape lawnmowerIcon{{40.f, 50.f}};
                    lawnmowerIcon.setPosition({10.f, lanesY[i] - 25.f});
                    
                    if (lawnmowersAvailable[i]) {
                        lawnmowerIcon.setFillColor(sf::Color(0, 200, 0));
                        lawnmowerIcon.setOutlineColor(sf::Color(0, 255, 0));
                    } else {
                        lawnmowerIcon.setFillColor(sf::Color(60, 60, 60));
                        lawnmowerIcon.setOutlineColor(sf::Color(100, 100, 100));
                    }
                    lawnmowerIcon.setOutlineThickness(2.f);
                    window.draw(lawnmowerIcon);
                    
                    if (lawnmowersAvailable[i]) {
                        sf::RectangleShape blade1{{25.f, 3.f}};
                        blade1.setFillColor(sf::Color(200, 200, 0));
                        blade1.setPosition({15.f, lanesY[i] - 10.f});
                        blade1.setRotation(sf::degrees(45.f));
                        window.draw(blade1);
                        
                        sf::RectangleShape blade2{{25.f, 3.f}};
                        blade2.setFillColor(sf::Color(200, 200, 0));
                        blade2.setPosition({15.f, lanesY[i] + 10.f});
                        blade2.setRotation(sf::degrees(-45.f));
                        window.draw(blade2);
                    }
                }

                sf::RectangleShape slotCell{{50.f, 60.f}};
                const bool flashRed = dangerZoneActive && std::fmod(gameTime, 0.4f) < 0.2f;
                if (flashRed) {
                    slotCell.setFillColor(sf::Color(150, 0, 0, 100));
                    slotCell.setOutlineColor(sf::Color(255, 50, 50));
                } else {
                    slotCell.setFillColor(sf::Color(0, 100, 0, 60));
                    slotCell.setOutlineColor(sf::Color(0, 180, 0));
                }
                slotCell.setOutlineThickness(2.f);
                slotCell.setOrigin(slotCell.getSize() / 2.f);
                for (int r = 0; r < static_cast<int>(lanesY.size()); ++r) {
                    for (int c = 0; c < static_cast<int>(slotXs.size()); ++c) {
                        slotCell.setPosition({slotXs[c], lanesY[r]});
                        window.draw(slotCell);
                    }
                }

                for (const auto &plant : plants) {
                    window.draw(*plant.sprite);

                    const auto pos       = plant.sprite->getPosition();
                    const auto texSize   = plant.sprite->getTexture().getSize();
                    const auto scale     = plant.sprite->getScale();
                    const sf::Vector2f size{texSize.x * scale.x, texSize.y * scale.y};
                    const float barWidth  = size.x;
                    const float barHeight = 5.f;
                    const float clampedHealth =
                        std::clamp(plant.health, 0.f, plant.maxHealth);
                    const float currentRatio =
                        plant.maxHealth > 0.f ? clampedHealth / plant.maxHealth : 0.f;

                    sf::RectangleShape hpBack{{barWidth, barHeight}};
                    hpBack.setFillColor(sf::Color(60, 60, 60));
                    hpBack.setOrigin({barWidth / 2.f, barHeight / 2.f});
                    hpBack.setPosition({pos.x, pos.y - size.y / 2.f - 6.f});

                    sf::RectangleShape hpFront{{barWidth * currentRatio, barHeight}};
                    hpFront.setFillColor(sf::Color(0, 220, 0));
                    hpFront.setOrigin({0.f, barHeight / 2.f});
                    hpFront.setPosition({pos.x - barWidth / 2.f, pos.y - size.y / 2.f - 6.f});

                    window.draw(hpBack);
                    window.draw(hpFront);
                }
                for (const auto &lm : activeLawnmowers) {
                    window.draw(*lm.sprite);
                }

                for (const auto &z : zombies) {
                    window.draw(*z.sprite);

                    const auto pos       = z.sprite->getPosition();
                    const auto texSize   = z.sprite->getTexture().getSize();
                    const auto scale     = z.sprite->getScale();
                    const sf::Vector2f size{texSize.x * scale.x, texSize.y * scale.y};
                    const float barWidth  = size.x;
                    const float barHeight = 6.f;
                    const int   clampedHealth = std::clamp(z.health, 0, z.maxHealth);
                    const float currentRatio =
                        z.maxHealth > 0 ? static_cast<float>(clampedHealth) / static_cast<float>(z.maxHealth) : 0.f;

                    sf::RectangleShape hpBack{{barWidth, barHeight}};
                    hpBack.setFillColor(sf::Color(80, 0, 0));
                    hpBack.setOrigin({barWidth / 2.f, barHeight / 2.f});
                    hpBack.setPosition({pos.x, pos.y - size.y / 2.f - 8.f});

                    sf::RectangleShape hpFront{{barWidth * currentRatio, barHeight}};
                    hpFront.setFillColor(sf::Color(0, 200, 0));
                    hpFront.setOrigin({0.f, barHeight / 2.f});
                    hpFront.setPosition({pos.x - barWidth / 2.f, pos.y - size.y / 2.f - 8.f});

                    window.draw(hpBack);
                    window.draw(hpFront);
                }
                for (const auto &b : bullets) {
                    window.draw(b.shape);
                }

                if (uiFontLoaded) {
                    sf::Text moneyText{uiFont, "Suns: " + std::to_string(money), 18};
                    moneyText.setFillColor(sf::Color::White);
                    moneyText.setPosition({10.f, 10.f});

                    int sunflowerCount = 0;
                    for (const auto& plant : plants) {
                        if (plant.type == PlantType::Sunflower) {
                            sunflowerCount++;
                        }
                    }
                    const float actualIncomePerSec = static_cast<float>(sunflowerCount) * (sunflowerIncome / sunflowerInterval);
                    sf::Text incomeText{
                        uiFont,
                        std::format("Income/sec: {:.1f} ({} sunflowers)", actualIncomePerSec, sunflowerCount),
                        18};
                    incomeText.setFillColor(sf::Color::Yellow);
                    incomeText.setPosition({10.f, 32.f});

                    const int minutes = static_cast<int>(gameTime) / 60;
                    const int seconds = static_cast<int>(gameTime) % 60;
                    sf::Text timeText{uiFont, std::format("Time: {:02d}:{:02d}", minutes, seconds), 18};
                    timeText.setFillColor(sf::Color::Cyan);
                    timeText.setPosition({10.f, 54.f});

                    const int difficultyPercent = static_cast<int>(difficultyMultiplier * 100.f);
                    sf::Text difficultyText{uiFont, std::format("Difficulty: {}%", difficultyPercent), 18};
                    difficultyText.setFillColor(sf::Color(255, 165, 0));
                    difficultyText.setPosition({10.f, 76.f});

                    window.draw(moneyText);
                    window.draw(incomeText);
                    window.draw(timeText);
                    window.draw(difficultyText);

                    sf::RectangleShape shooterCard{{120.f, 35.f}};
                    shooterCard.setPosition({450.f, 20.f});
                    shooterCard.setFillColor(selectedPlantType == PlantType::Shooter
                                              ? sf::Color(0, 120, 0)
                                              : sf::Color(0, 80, 0));
                    shooterCard.setOutlineColor(sf::Color::White);
                    shooterCard.setOutlineThickness(2.f);

                    sf::RectangleShape sunflowerCard{{120.f, 35.f}};
                    sunflowerCard.setPosition({580.f, 20.f});
                    sunflowerCard.setFillColor(selectedPlantType == PlantType::Sunflower
                                                ? sf::Color(160, 130, 0)
                                                : sf::Color(110, 90, 0));
                    sunflowerCard.setOutlineColor(sf::Color::White);
                    sunflowerCard.setOutlineThickness(2.f);

                    sf::RectangleShape potatoCard{{120.f, 35.f}};
                    potatoCard.setPosition({450.f, 65.f});
                    potatoCard.setFillColor(selectedPlantType == PlantType::Potato
                                             ? sf::Color(139, 69, 19)
                                             : sf::Color(100, 50, 10));
                    potatoCard.setOutlineColor(sf::Color::White);
                    potatoCard.setOutlineThickness(2.f);

                    sf::RectangleShape heavyCard{{120.f, 35.f}};
                    heavyCard.setPosition({580.f, 65.f});
                    heavyCard.setFillColor(selectedPlantType == PlantType::HeavyShooter
                                            ? sf::Color(0, 150, 150)
                                            : sf::Color(0, 100, 100));
                    heavyCard.setOutlineColor(sf::Color::White);
                    heavyCard.setOutlineThickness(2.f);

                    sf::Text shooterText{uiFont, "Shooter (" + std::to_string(shooterCost) + ")", 12};
                    shooterText.setFillColor(sf::Color::White);
                    shooterText.setPosition({460.f, 28.f});

                    sf::Text sunflowerText{uiFont, "Sunflower (" + std::to_string(sunflowerCost) + ")", 12};
                    sunflowerText.setFillColor(sf::Color::White);
                    sunflowerText.setPosition({590.f, 28.f});

                    sf::Text potatoText{uiFont, "Potato (" + std::to_string(potatoCost) + ")", 12};
                    potatoText.setFillColor(sf::Color::White);
                    potatoText.setPosition({460.f, 73.f});

                    sf::Text heavyText{uiFont, "Heavy (" + std::to_string(heavyCost) + ")", 12};
                    heavyText.setFillColor(sf::Color::White);
                    heavyText.setPosition({590.f, 73.f});

                    window.draw(shooterCard);
                    window.draw(sunflowerCard);
                    window.draw(potatoCard);
                    window.draw(heavyCard);
                    window.draw(shooterText);
                    window.draw(sunflowerText);
                    window.draw(potatoText);
                    window.draw(heavyText);
                }

                window.display();
            }
        }
    };

}
