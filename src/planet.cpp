#include "planet.hpp"
#include <raylib.h>
#include <cmath>
#include <vector>
#include <string>

const double G = 6.674e-11;
double scale = 5e8;

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
}

void Planet::Draw() const {
    int screenCenterX = GetScreenWidth() / 2;
    int screenCenterY = GetScreenHeight() / 2;

    double scaledX = screenCenterX + x / scale;
    double scaledY = screenCenterY + y / scale;

    DrawCircle(
        static_cast<int>(scaledX),
        static_cast<int>(scaledY),
        static_cast<int>(radius * (5e8 / scale)),
        RED
    );
}