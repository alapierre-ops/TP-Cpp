module;

#include <SFML/Graphics.hpp>

export module pvz.game_types;

import std;

export namespace pvz::types {

    enum class PlantType {
        Shooter,
        Sunflower,
        Potato,
        HeavyShooter
    };

    enum class ZombieType {
        Normal,
        Tank,
        Sprinter,
        Boss
    };

    struct Plant {
        std::optional<sf::Sprite> sprite;
        float shootCooldown{0.f};
        float maxHealth{3.f};
        float health{3.f};
        int   rowIndex{0};
        int   colIndex{0};
        PlantType type{PlantType::Shooter};
        float sunCooldown{0.f};
    };

    struct Zombie {
        std::optional<sf::Sprite> sprite;
        float speed{40.f};
        int   maxHealth{5};
        int   health{5};
        ZombieType type{ZombieType::Normal};
    };

    struct Bullet {
        sf::CircleShape shape;
        float speed{200.f};
        int   damage{1};
    };

    struct Lawnmower {
        std::optional<sf::Sprite> sprite;
        float x{0.f};
        int   lane{0};
        float speed{400.f};
    };

}

