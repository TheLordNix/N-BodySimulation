#pragma once
#include <vector>
#include <raylib.h>

class Planet
{
public:
    Planet();
    void Draw() const;
    void Update(std::vector<Planet>& planets, double deltaTime);

    double GetX() const { return x; }
    double GetY() const { return y; }
    double GetMass() const { return mass; }
    double GetSpeedX() const { return speedX; }
    double GetSpeedY() const { return speedY; }
    double GetAccX() const { return accX; }
    double GetAccY() const { return accY; }
    double GetRadius() const { return radius; }
    
    void SetX(double newX) { x = newX; }
    void SetY(double newY) { y = newY; }
    void SetMass(double newMass) { mass = newMass; }
    void SetSpeedX(double newSpeedX) { speedX = newSpeedX; }
    void SetSpeedY(double newSpeedY) { speedY = newSpeedY; }
    void SetAccX(double newAccX) { accX = newAccX; }
    void SetAccY(double newAccY) { accY = newAccY; }
    void SetRadius(double newRadius) { radius = newRadius; }

private:
    double x;
    double y;
    double speedX;
    double speedY;
    double accX;
    double accY;
    double mass;
    double radius;
};

extern double scale;