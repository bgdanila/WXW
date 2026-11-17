#pragma once

#include <glew/glew.h>
#include "glfw/glfw3.h"
#include <memory>
#include <array>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Graphics/sprite_renderer.hpp"
#include "../Graphics/shape_renderer.hpp"
#include "resource_manager.hpp"
#include "wolverine.hpp"
#include "drone.hpp"

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game
{
private:
    std::unique_ptr<SpriteRenderer> spriteRenderer;
    std::unique_ptr<ShapeRenderer> shapeRenderer;
    Wolverine player;
    std::vector<Drone> drones;
    float nextDroneSpawnX = 650.0f;
    float cameraX = 0.0f;
    std::vector<std::string> labBackgrounds;

public:
    GameState State;
    std::array<bool, 1024> Keys{};
    std::array<bool, 1024> KeysProcessed{};
    unsigned int width = 0;
    unsigned int height = 0;

    Game(unsigned int w, unsigned int h);
    ~Game();

    void init();

    void processInput(float dt);
    void update(float dt);
    void render();
};