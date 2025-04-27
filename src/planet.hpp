#pragma once
#include <vector>
#include <deque>
#include <raylib.h>

class Planet
{
public:
    Planet();
    void Draw() const;
    void Update(std::vector<Planet>& planets, double deltaTime);
    void UpdateTrail();

    double GetX() const { return x; }
    double GetY() const { return y; }
    double GetMass() const { return mass; }
    double GetSpeedX() const { return speedX; }
    double GetSpeedY() const { return speedY; }
    double GetAccX() const { return accX; }
    double GetAccY() const { return accY; }
    double GetRadius() const { return radius; }
    Color GetColor() const { return color; }
    
    void SetX(double newX) { x = newX; }
    void SetY(double newY) { y = newY; }
    void SetMass(double newMass) { mass = newMass; }
    void SetSpeedX(double newSpeedX) { speedX = newSpeedX; }
    void SetSpeedY(double newSpeedY) { speedY = newSpeedY; }
    void SetAccX(double newAccX) { accX = newAccX; }
    void SetAccY(double newAccY) { accY = newAccY; }
    void SetRadius(double newRadius) { radius = newRadius; }
    void SetColor(Color newColor) { color = newColor; }

    // Static feature toggles
    static bool showGlow;
    static bool showTrails;
    static bool showShadows;
    static bool showRings;

private:
    double x;
    double y;
    double speedX;
    double speedY;
    double accX;
    double accY;
    double mass;
    double radius;
    Color color;

    std::deque<Vector2> trail;
    static const int maxTrailLength = 100; 
};

extern double scale;