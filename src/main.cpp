#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>
#include "planet.hpp"

const double G = 6.674e-11;

float DrawCustomSlider(Rectangle sliderBounds, float value, float minValue, float maxValue, bool& isSliding) {
    DrawRectangleRec(sliderBounds, LIGHTGRAY);

    float sliderWidth = (value - minValue) / (maxValue - minValue) * sliderBounds.width;
    Rectangle sliderHandle = { sliderBounds.x + sliderWidth - 5, sliderBounds.y, 10, sliderBounds.height };

    DrawRectangleRec(sliderHandle, DARKGRAY);

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), sliderBounds)) {
        isSliding = true;
        float mouseX = GetMousePosition().x;
        value = minValue + (mouseX - sliderBounds.x) / sliderBounds.width * (maxValue - minValue);
        if (value < minValue) value = minValue;
        if (value > maxValue) value = maxValue;
    } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        isSliding = false;
    }

    return value;
}

int main() 
{
    const Color darkGreen = {20, 160, 133, 255};

    constexpr int screenWidth = 1400;
    constexpr int screenHeight = 850;

    InitWindow(screenWidth, screenHeight, "N-Body Simulation");
    SetTargetFPS(60);

    Camera2D camera = { 0 };
    camera.target = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    Vector2 lastMousePosition = { 0 };

    std::vector<Planet> planets;

    Planet sun;
    sun.SetMass(1.989e30);
    sun.SetRadius(50);
    sun.SetX(0);
    sun.SetY(0);
    sun.SetSpeedY(0);
    sun.SetColor(YELLOW);
    planets.push_back(sun);

    // Lambda to add other planets
    auto addPlanet = [&](double distance, double mass, double radius, double speedY, Color color) {
        Planet p;
        p.SetMass(mass);
        p.SetRadius(radius);
        p.SetX(distance);
        p.SetY(0);
        p.SetSpeedY(speedY);
        p.SetColor(color);
        planets.push_back(p);
    };

    addPlanet(5.79e10, 3.30e23, 6, 4.79e4, GRAY);    // Mercury
    addPlanet(1.08e11, 4.87e24, 10, 3.50e4, ORANGE); // Venus
    addPlanet(1.50e11, 5.97e24, 12, 2.98e4, GREEN);   // Earth
    addPlanet(2.28e11, 6.42e23, 10, 2.41e4, RED);    // Mars
    addPlanet(7.78e11, 1.90e27, 25, 1.31e4, BROWN);  // Jupiter
    addPlanet(1.43e12, 5.68e26, 22, 9.7e3, BEIGE);   // Saturn
    addPlanet(2.87e12, 8.68e25, 18, 6.8e3, BLUE); // Uranus
    addPlanet(4.50e12, 1.02e26, 18, 5.4e3, DARKBLUE);  // Neptune

    float deltaTime = 3600 * 10;
    float simulationSpeed = deltaTime / 3600.0f;

    bool isDragging = false;
    bool showAddPlanetUI = true;
    Planet newPlanet;
    newPlanet.SetMass(1e24);
    newPlanet.SetRadius(10);
    bool giveOrbitalSpeed = false;
    bool isSliding = false;

    while (!WindowShouldClose())
    {
        Vector2 mousePosition = GetMousePosition();

        if (IsKeyPressed(KEY_G)) Planet::showGlow = !Planet::showGlow;
        if (IsKeyPressed(KEY_T)) Planet::showTrails = !Planet::showTrails;
        if (IsKeyPressed(KEY_S)) Planet::showShadows = !Planet::showShadows;
        if (IsKeyPressed(KEY_R)) Planet::showRings = !Planet::showRings;

        Rectangle speedUpButton = { screenWidth - 140, screenHeight - 50, 60, 40 };
        Rectangle speedDownButton = { screenWidth - 70, screenHeight - 50, 60, 40 };
        Rectangle toggleAddPlanetUIBox = { screenWidth - 300, 10, 280, 40 };
        Rectangle planetDisplayBox = { screenWidth - 300, 60, 280, 280 };
        Rectangle orbitalSpeedButton = { planetDisplayBox.x, planetDisplayBox.y + planetDisplayBox.height + 110, 150, 40 };

        // Input

        if (!isSliding && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (CheckCollisionPointRec(mousePosition, speedUpButton)) {
                deltaTime *= 10;
            }
            if (CheckCollisionPointRec(mousePosition, speedDownButton)) {
                deltaTime /= 10;
                if (deltaTime < 0.000001 * 3600) deltaTime = 0.000001 * 3600; 
            }
            if (CheckCollisionPointRec(mousePosition, orbitalSpeedButton)) {
                giveOrbitalSpeed = !giveOrbitalSpeed;
            }
            if (CheckCollisionPointRec(mousePosition, toggleAddPlanetUIBox)) {
                showAddPlanetUI = !showAddPlanetUI;
            }
        }

        simulationSpeed = deltaTime / 3600.0f;

        // Camera movement

        if (!isDragging && !isSliding && (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) || IsMouseButtonDown(MOUSE_BUTTON_LEFT))) {
            Vector2 delta = Vector2Subtract(mousePosition, lastMousePosition);
            camera.target = Vector2Subtract(camera.target, Vector2Scale(delta, 1.0f / camera.zoom));
        }

        lastMousePosition = mousePosition;

        // Camera zoom

        float scroll = GetMouseWheelMove();
        if (scroll != 0) {
            Vector2 mouseWorldPos = GetScreenToWorld2D(mousePosition, camera);

            float zoomFactor = (scroll > 0) ? 1.1f : 0.9f;
            camera.zoom *= zoomFactor;

            Vector2 afterZoomWorldPos = GetScreenToWorld2D(mousePosition, camera);
            Vector2 zoomDelta = Vector2Subtract(mouseWorldPos, afterZoomWorldPos);
            camera.target = Vector2Add(camera.target, zoomDelta);
        }

        // Add planet UI

        if (showAddPlanetUI && !isSliding) {
            if (!isDragging && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (CheckCollisionPointRec(mousePosition, planetDisplayBox)) {
                    isDragging = true;
                    Vector2 worldPos = GetScreenToWorld2D(mousePosition, camera);

                    newPlanet.SetX((worldPos.x - GetScreenWidth() / 2) * scale);
                    newPlanet.SetY((worldPos.y - GetScreenHeight() / 2) * scale);
                }
            }

            if (isDragging && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                Vector2 worldPos = GetScreenToWorld2D(mousePosition, camera);

                newPlanet.SetX((worldPos.x - GetScreenWidth() / 2) * scale);
                newPlanet.SetY((worldPos.y - GetScreenHeight() / 2) * scale);
            }

            if (isDragging && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                isDragging = false;

                if (giveOrbitalSpeed) {
                    Planet& sun = planets[0];
                    double dx = newPlanet.GetX() - sun.GetX();
                    double dy = newPlanet.GetY() - sun.GetY();
                    double distance = std::sqrt(dx * dx + dy * dy);
                    double orbitalVelocity = std::sqrt(G * sun.GetMass() / distance);
                    newPlanet.SetSpeedX(-orbitalVelocity * (dy / distance));
                    newPlanet.SetSpeedY(orbitalVelocity * (dx / distance));
                }
                planets.push_back(newPlanet);
            }
        }

        // Planet update

        for (auto& planet : planets) {
            planet.Update(planets, deltaTime);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode2D(camera);

        // GRID
        float baseGridSpacing = 100.0f; 
        float gridSpacing = baseGridSpacing;

        while (gridSpacing * camera.zoom < baseGridSpacing) {
            gridSpacing *= 2.0f; 
        }
        while (gridSpacing * camera.zoom > baseGridSpacing * 2.0f) {
            gridSpacing /= 2.0f; 
        }

        Vector2 centerWorld = GetScreenToWorld2D({ screenWidth / 2.0f, screenHeight / 2.0f }, camera);

        float startX = std::floor(centerWorld.x / gridSpacing) * gridSpacing;
        float startY = std::floor(centerWorld.y / gridSpacing) * gridSpacing;

        for (float x = startX; x <= centerWorld.x + (screenWidth / 2.0f) / camera.zoom; x += gridSpacing) {
            DrawLine(x, centerWorld.y - (screenHeight / 2.0f) / camera.zoom, x, centerWorld.y + (screenHeight / 2.0f) / camera.zoom, LIGHTGRAY);
        }
        for (float x = startX; x >= centerWorld.x - (screenWidth / 2.0f) / camera.zoom; x -= gridSpacing) {
            DrawLine(x, centerWorld.y - (screenHeight / 2.0f) / camera.zoom, x, centerWorld.y + (screenHeight / 2.0f) / camera.zoom, LIGHTGRAY);
        }

        for (float y = startY; y <= centerWorld.y + (screenHeight / 2.0f) / camera.zoom; y += gridSpacing) {
            DrawLine(centerWorld.x - (screenWidth / 2.0f) / camera.zoom, y, centerWorld.x + (screenWidth / 2.0f) / camera.zoom, y, LIGHTGRAY);
        }
        for (float y = startY; y >= centerWorld.y - (screenHeight / 2.0f) / camera.zoom; y -= gridSpacing) {
            DrawLine(centerWorld.x - (screenWidth / 2.0f) / camera.zoom, y, centerWorld.x + (screenWidth / 2.0f) / camera.zoom, y, LIGHTGRAY);
        }

        // Planet Draw
        ClearBackground(darkGreen);
        for (const auto& planet : planets) {
            planet.Draw();
        }

        if (isDragging) {
            DrawCircleV(GetWorldToScreen2D({ (float)newPlanet.GetX(), (float)newPlanet.GetY() }, camera), newPlanet.GetRadius(), BLUE);
        }

        EndMode2D();

        // UI TEXT

        DrawText(TextFormat("Zoom: %.2fx", camera.zoom), 10, 10, 20, DARKGRAY);

        DrawRectangleRec(speedUpButton, LIGHTGRAY);
        DrawRectangleRec(speedDownButton, LIGHTGRAY);
        DrawText("FAST", speedUpButton.x + 2, speedUpButton.y + 10, 20, BLACK);
        DrawText("SLOW", speedDownButton.x + 2, speedDownButton.y + 10, 20, BLACK);

        DrawText(TextFormat("Simulation Speed: %.6f hours/frame", simulationSpeed), 10, 30, 20, DARKGRAY);

        DrawRectangleRec(toggleAddPlanetUIBox, LIGHTGRAY);
        DrawText(showAddPlanetUI ? "HIDE ADD PLANET UI" : "SHOW ADD PLANET UI", toggleAddPlanetUIBox.x + 10, toggleAddPlanetUIBox.y + 10, 20, BLACK);

        if (showAddPlanetUI) {
            DrawRectangleRec(planetDisplayBox, LIGHTGRAY);
            DrawCircle(planetDisplayBox.x + planetDisplayBox.width / 2, planetDisplayBox.y + planetDisplayBox.height / 2, newPlanet.GetRadius(), BLUE);
        
            DrawText("Mass", planetDisplayBox.x + 10, planetDisplayBox.y + planetDisplayBox.height + 10, 20, DARKGRAY);
            newPlanet.SetMass(DrawCustomSlider({ planetDisplayBox.x + 100, planetDisplayBox.y + planetDisplayBox.height + 10, 150, 20 }, newPlanet.GetMass(), 1e22, 1e30, isSliding));
            DrawText(TextFormat("%.2e kg", newPlanet.GetMass()), planetDisplayBox.x + 100, planetDisplayBox.y + planetDisplayBox.height + 35, 20, DARKGRAY);
        
            DrawText("Radius", planetDisplayBox.x + 10, planetDisplayBox.y + planetDisplayBox.height + 60, 20, DARKGRAY);
            newPlanet.SetRadius(DrawCustomSlider({ planetDisplayBox.x + 100, planetDisplayBox.y + planetDisplayBox.height + 60, 150, 20 }, newPlanet.GetRadius(), 1, 50, isSliding));
            DrawText(TextFormat("%.2f units", newPlanet.GetRadius()), planetDisplayBox.x + 100, planetDisplayBox.y + planetDisplayBox.height + 85, 20, DARKGRAY);
        
            DrawRectangleRec(orbitalSpeedButton, LIGHTGRAY);
            DrawText(giveOrbitalSpeed ? "Orbital Speed: ON" : "Orbital Speed: OFF", orbitalSpeedButton.x + 5, orbitalSpeedButton.y + 10, 20, DARKGRAY);
        }

        EndDrawing();
    }

    CloseWindow();
}