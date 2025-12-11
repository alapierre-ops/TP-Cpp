module;

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

export module pvz.sprites;

import std;

export namespace pvz::sprites {

    inline sf::Texture createPixelArtTexture(int width, int height, std::function<void(sf::Image&)> drawFunc) {
        sf::Image img{{static_cast<unsigned int>(width), static_cast<unsigned int>(height)}, sf::Color::Transparent};
        drawFunc(img);
        sf::Texture tex;
        if (!tex.loadFromImage(img)) {
            std::cerr << "Erreur lors du chargement de la texture\n";
        }
        tex.setSmooth(false);
        return tex;
    }

    inline sf::Texture createShooterSprite() {
        return createPixelArtTexture(32, 32, [](sf::Image& img) {
            for (int y = 20; y < 32; ++y) {
                for (int x = 14; x < 18; ++x) {
                    img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(0, 150, 0));
                }
            }
            for (int y = 8; y < 20; ++y) {
                for (int x = 10; x < 22; ++x) {
                    if ((x - 16) * (x - 16) + (y - 14) * (y - 14) < 50) {
                        img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(0, 200, 0));
                    }
                }
            }
            for (int x = 14; x < 18; ++x) {
                img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(12)}, sf::Color(30, 30, 30));
            }
        });
    }

    inline sf::Texture createSunflowerSprite() {
        return createPixelArtTexture(32, 32, [](sf::Image& img) {
            for (int y = 20; y < 32; ++y) {
                for (int x = 14; x < 18; ++x) {
                    img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(0, 150, 0));
                }
            }
            for (int x = 8; x < 12; ++x) {
                img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(18)}, sf::Color(0, 180, 0));
            }
            for (int x = 20; x < 24; ++x) {
                img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(18)}, sf::Color(0, 180, 0));
            }
            for (int y = 10; y < 18; ++y) {
                for (int x = 12; x < 20; ++x) {
                    if ((x - 16) * (x - 16) + (y - 14) * (y - 14) < 25) {
                        img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(139, 90, 43));
                    }
                }
            }
            for (int angle = 0; angle < 360; angle += 45) {
                const float rad = angle * 3.14159f / 180.f;
                const int px = 16 + static_cast<int>(10 * std::cos(rad));
                const int py = 14 + static_cast<int>(10 * std::sin(rad));
                if (px >= 0 && px < 32 && py >= 0 && py < 32) {
                    img.setPixel({static_cast<unsigned int>(px), static_cast<unsigned int>(py)}, sf::Color(255, 215, 0));
                    if (px > 0) img.setPixel({static_cast<unsigned int>(px - 1), static_cast<unsigned int>(py)}, sf::Color(255, 215, 0));
                    if (px < 31) img.setPixel({static_cast<unsigned int>(px + 1), static_cast<unsigned int>(py)}, sf::Color(255, 215, 0));
                }
            }
        });
    }

    inline sf::Texture createPotatoSprite() {
        return createPixelArtTexture(32, 32, [](sf::Image& img) {
            for (int y = 8; y < 28; ++y) {
                for (int x = 8; x < 24; ++x) {
                    const float dx = (x - 16.f) / 8.f;
                    const float dy = (y - 18.f) / 10.f;
                    if (dx * dx + dy * dy < 1.f) {
                        img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(139, 69, 19));
                    }
                }
            }
            img.setPixel({12u, 14u}, sf::Color(30, 30, 30));
            img.setPixel({20u, 14u}, sf::Color(30, 30, 30));
            for (int x = 14; x < 18; ++x) {
                img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(18)}, sf::Color(30, 30, 30));
            }
        });
    }

    inline sf::Texture createHeavyShooterSprite() {
        return createPixelArtTexture(32, 32, [](sf::Image& img) {
            for (int y = 20; y < 32; ++y) {
                for (int x = 12; x < 20; ++x) {
                    img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(0, 150, 0));
                }
            }
            for (int y = 6; y < 20; ++y) {
                for (int x = 8; x < 24; ++x) {
                    if ((x - 16) * (x - 16) + (y - 13) * (y - 13) < 80) {
                        img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(0, 180, 180));
                    }
                }
            }
            for (int x = 14; x < 18; ++x) {
                img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(10)}, sf::Color(30, 30, 30));
                img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(12)}, sf::Color(30, 30, 30));
            }
        });
    }

    inline sf::Texture createZombieSprite() {
        return createPixelArtTexture(32, 32, [](sf::Image& img) {
            for (int y = 12; y < 28; ++y) {
                for (int x = 10; x < 22; ++x) {
                    if ((x - 16) * (x - 16) + (y - 20) * (y - 20) < 50) {
                        img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(100, 120, 100));
                    }
                }
            }
            for (int y = 6; y < 14; ++y) {
                for (int x = 12; x < 20; ++x) {
                    if ((x - 16) * (x - 16) + (y - 10) * (y - 10) < 20) {
                        img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(150, 170, 150));
                    }
                }
            }
            img.setPixel({13u, 9u}, sf::Color(200, 0, 0));
            img.setPixel({19u, 9u}, sf::Color(200, 0, 0));
            for (int y = 14; y < 20; ++y) {
                img.setPixel({8u, static_cast<unsigned int>(y)}, sf::Color(100, 120, 100));
                img.setPixel({24u, static_cast<unsigned int>(y)}, sf::Color(100, 120, 100));
            }
        });
    }

    inline sf::Texture createTankZombieSprite() {
        return createPixelArtTexture(32, 32, [](sf::Image& img) {
            for (int y = 12; y < 28; ++y) {
                for (int x = 8; x < 24; ++x) {
                    if ((x - 16) * (x - 16) + (y - 20) * (y - 20) < 70) {
                        img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(100, 120, 100));
                    }
                }
            }
            for (int y = 4; y < 12; ++y) {
                for (int x = 12; x < 20; ++x) {
                    const int width = 8 - (y - 4) / 2;
                    if (std::abs(x - 16) < width) {
                        img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(255, 140, 0));
                    }
                }
            }
            for (int y = 8; y < 14; ++y) {
                for (int x = 12; x < 20; ++x) {
                    if ((x - 16) * (x - 16) + (y - 11) * (y - 11) < 20) {
                        img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(150, 170, 150));
                    }
                }
            }
            img.setPixel({13u, 11u}, sf::Color(200, 0, 0));
            img.setPixel({19u, 11u}, sf::Color(200, 0, 0));
        });
    }

    inline sf::Texture createSprinterZombieSprite() {
        return createPixelArtTexture(32, 32, [](sf::Image& img) {
            for (int y = 14; y < 28; ++y) {
                for (int x = 12; x < 20; ++x) {
                    if ((x - 16) * (x - 16) + (y - 21) * (y - 21) < 30) {
                        img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(100, 120, 100));
                    }
                }
            }
            for (int y = 6; y < 14; ++y) {
                for (int x = 13; x < 19; ++x) {
                    if ((x - 16) * (x - 16) + (y - 10) * (y - 10) < 15) {
                        img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(150, 170, 150));
                    }
                }
            }
            img.setPixel({14u, 9u}, sf::Color(200, 0, 0));
            img.setPixel({18u, 9u}, sf::Color(200, 0, 0));
            for (int y = 14; y < 20; ++y) {
                img.setPixel({9u, static_cast<unsigned int>(y)}, sf::Color(100, 120, 100));
                img.setPixel({23u, static_cast<unsigned int>(y)}, sf::Color(100, 120, 100));
            }
            for (int x = 10; x < 22; ++x) {
                img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(26)}, sf::Color(255, 100, 100));
            }
        });
    }

    inline sf::Texture createBossZombieSprite() {
        return createPixelArtTexture(32, 32, [](sf::Image& img) {
            for (int y = 8; y < 30; ++y) {
                for (int x = 6; x < 26; ++x) {
                    const float dx = (x - 16.f) / 10.f;
                    const float dy = (y - 19.f) / 11.f;
                    if (dx * dx + dy * dy < 1.f) {
                        img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(60, 80, 60));
                    }
                }
            }
            for (int y = 4; y < 12; ++y) {
                for (int x = 10; x < 22; ++x) {
                    if ((x - 16) * (x - 16) + (y - 8) * (y - 8) < 30) {
                        img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(120, 140, 120));
                    }
                }
            }
            for (int y = 6; y < 9; ++y) {
                for (int x = 12; x < 15; ++x) {
                    img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(255, 0, 0));
                }
                for (int x = 17; x < 20; ++x) {
                    img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(255, 0, 0));
                }
            }
            for (int y = 2; y < 6; ++y) {
                for (int x = 12; x < 20; ++x) {
                    if (y == 2 || y == 5 || x == 12 || x == 19) {
                        img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(100, 100, 120));
                    }
                }
            }
            for (int y = 12; y < 22; ++y) {
                for (int x = 2; x < 6; ++x) {
                    img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(60, 80, 60));
                }
                for (int x = 26; x < 30; ++x) {
                    img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(60, 80, 60));
                }
            }
            for (int angle = 0; angle < 360; angle += 30) {
                const float rad = angle * 3.14159f / 180.f;
                const int px = 16 + static_cast<int>(14 * std::cos(rad));
                const int py = 16 + static_cast<int>(14 * std::sin(rad));
                if (px >= 0 && px < 32 && py >= 0 && py < 32) {
                    img.setPixel({static_cast<unsigned int>(px), static_cast<unsigned int>(py)}, sf::Color(150, 0, 200));
                }
            }
        });
    }

    inline sf::Texture createLawnmowerSprite() {
        return createPixelArtTexture(32, 32, [](sf::Image& img) {
            for (int y = 12; y < 24; ++y) {
                for (int x = 4; x < 28; ++x) {
                    if (y >= 16 && y < 20) {
                        img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(200, 50, 50));
                    } else if ((y >= 12 && y < 16) || (y >= 20 && y < 24)) {
                        const float dx = (x - 16.f) / 12.f;
                        const float dy = (y < 16 ? (y - 14.f) / 2.f : (22.f - y) / 2.f);
                        if (dx * dx + dy * dy < 1.f) {
                            img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(200, 50, 50));
                        }
                    }
                }
            }
            for (int x = 8; x < 24; ++x) {
                if ((x % 4) < 2) {
                    img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(18)}, sf::Color(255, 255, 200));
                }
            }
            for (int y = 8; y < 12; ++y) {
                for (int x = 20; x < 26; ++x) {
                    img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(139, 90, 43));
                }
            }
            for (int y = 22; y < 26; ++y) {
                for (int x = 8; x < 12; ++x) {
                    if ((x - 10) * (x - 10) + (y - 24) * (y - 24) < 4) {
                        img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(30, 30, 30));
                    }
                }
                for (int x = 20; x < 24; ++x) {
                    if ((x - 22) * (x - 22) + (y - 24) * (y - 24) < 4) {
                        img.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color(30, 30, 30));
                    }
                }
            }
        });
    }

    inline sf::SoundBuffer generateSoundBuffer(float frequency, float duration, float sampleRate = 44100.f) {
        sf::SoundBuffer buffer;
        const std::size_t sampleCount = static_cast<std::size_t>(sampleRate * duration);
        std::vector<std::int16_t> samples(sampleCount);

        for (std::size_t i = 0; i < sampleCount; ++i) {
            const float t = static_cast<float>(i) / sampleRate;
            float envelope = 1.f;
            if (t < duration * 0.1f) {
                envelope = t / (duration * 0.1f);
            } else if (t > duration * 0.8f) {
                envelope = 1.f - (t - duration * 0.8f) / (duration * 0.2f);
            }
            const float sample = std::sin(2.f * 3.14159f * frequency * t) * envelope;
            samples[i] = static_cast<std::int16_t>(sample * 16000.f);
        }

        const bool success = buffer.loadFromSamples(samples.data(), sampleCount, 1, static_cast<unsigned int>(sampleRate),
                                                    {sf::SoundChannel::Mono});
        if (!success) {
            std::cerr << "Erreur lors de la génération du son\n";
        }
        return buffer;
    }

}

