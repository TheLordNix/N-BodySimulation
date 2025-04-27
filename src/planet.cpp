#include "planet.hpp"
#include <raylib.h>
#include <cmath>
#include <vector>
#include <string>

const double G = 6.674e-11;
double scale = 5e8;

// Initialize static members
bool Planet::showGlow = true;
bool Planet::showTrails = true;
bool Planet::showShadows = false;
bool Planet::showRings = false;

Planet::Planet() 
    : x(0), y(0), speedX(0), speedY(0), accX(0), accY(0), mass(10), radius(100) 
{
}

std::pair<double, double> CalculateGravitationalForce(const Planet& a, const Planet& b) {
    double dx = b.GetX() - a.GetX();
    double dy = b.GetY() - a.GetY();
    double distanceSquared = dx * dx + dy * dy;
    double distance = std::sqrt(distanceSquared);

    if (distanceSquared < 1e-5) return {0, 0};

    double forceMagnitude = (G * a.GetMass() * b.GetMass()) / distanceSquared;
    return {
        forceMagnitude * (dx / distance),
        forceMagnitude * (dy / distance)
    };
}

void Planet::UpdateTrail() {
    trail.push_back({ (float)x, (float)y });
    if (trail.size() > maxTrailLength) {
        trail.pop_front();
    }
}

void Planet::Update(std::vector<Planet>& planets, double deltaTime) {
    double netForceX = 0;
    double netForceY = 0;

    for (const auto& other : planets) {
        if (&other == this) continue;

        std::pair<double, double> force = CalculateGravitationalForce(*this, other);
        double fx = force.first;
        double fy = force.second;
        netForceX += fx;
        netForceY += fy;
    }

    accX = netForceX / mass;
    accY = netForceY / mass;

    speedX += accX * deltaTime;
    speedY += accY * deltaTime;

    x += speedX * deltaTime;
    y += speedY * deltaTime;
    
    UpdateTrail();
}

void Planet::Draw() const {
    int screenCenterX = GetScreenWidth() / 2;
    int screenCenterY = GetScreenHeight() / 2;

    double scaledX = screenCenterX + x / scale;
    double scaledY = screenCenterY + y / scale;

    // Draw glow
    if (showGlow) {
        DrawCircle(
            static_cast<int>(scaledX),
            static_cast<int>(scaledY),
            static_cast<int>(radius * 1.5 * (5e8 / scale)),
            Fade(color, 0.3f) // Use the planet's base color with reduced alpha
        );
    }

    // Draw planet
    DrawCircle(
        static_cast<int>(scaledX),
        static_cast<int>(scaledY),
        static_cast<int>(radius * (5e8 / scale)),
        color
    );

    // Draw orbital trail
    if (showTrails && trail.size() > 1) {
        std::vector<Vector2> scaledTrail(trail.size());
        for (size_t i = 0; i < trail.size(); ++i) {
            scaledTrail[i] = { 
                (float)(screenCenterX + trail[i].x / scale), 
                (float)(screenCenterY + trail[i].y / scale) 
            };
        }
    
        for (size_t i = 1; i < scaledTrail.size(); ++i) {
            DrawLineEx(
                scaledTrail[i - 1], 
                scaledTrail[i], 
                4.0f, // Thickness
                Fade(color, (float)i / trail.size())
            );
        }
    }
}