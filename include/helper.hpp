#pragma once

#include <random>

// Returns random angle in degrees in range [0, 360)
inline float getRandomAngle()
{
    static std::mt19937 rng{ std::random_device{}() };
    static std::uniform_real_distribution<float> dist(0.0f, 360.0f);
    return dist(rng);
}