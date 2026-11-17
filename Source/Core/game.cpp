#include "game.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <algorithm>
#include <iostream>

Game::Game(unsigned int w, unsigned int h)
    : State(GAME_ACTIVE), Keys(), width(w), height(h)
{
}

void Game::init()
{
    Shader& spriteShader = ResourceManager::loadShader(
        "Source/Shaders/vshader.vert",
        "Source/Shaders/fshader.frag",
        nullptr,
        "sprite"
    );

    glm::mat4 projection = glm::ortho(
        0.0f, static_cast<float>(width),
        static_cast<float>(height), 0.0f,
        -1.0f, 1.0f
    );

    spriteShader.use();
    spriteShader.setInt("image", 0);
    spriteShader.setMat4("projection", projection);

    Shader& shapeShader = ResourceManager::loadShader(
        "Source/Shaders/shape.vert",
        "Source/Shaders/shape.frag",
        nullptr,
        "shape"
    );
    shapeShader.use();
    shapeShader.setMat4("projection", projection);

    spriteRenderer = std::make_unique<SpriteRenderer>(spriteShader);
    shapeRenderer = std::make_unique<ShapeRenderer>(shapeShader);
    ResourceManager::loadTexture("Source/Assets/Textures/wx.png", "player");

    ResourceManager::loadTexture("Source/Assets/Textures/floor.jpg", "floor");
    ResourceManager::loadTexture("Source/Assets/Textures/lab_tank.jpg", "lab_tank");
    ResourceManager::loadTexture("Source/Assets/Textures/lab_monitors.jpg", "lab_monitors");
    ResourceManager::loadTexture("Source/Assets/Textures/lab_corridor.jpg", "lab_corridor");

    labBackgrounds = { "lab_tank", "lab_monitors", "lab_corridor" };
}

void Game::processInput(float dt)
{
    // User input: after defeat or victory, R resets the player, enemies, camera and game state.
    if (player.hp <= 0.0f || State == GAME_WIN) {
        if (Keys[GLFW_KEY_R]) {
            player = Wolverine();
            drones.clear();
            cameraX = 0.0f;
            nextDroneSpawnX = 650.0f;
            State = GAME_ACTIVE;
        }
        return;
    }

    // A/D set horizontal velocity while held; resetting it stops movement when released.
    // Wolverine::update multiplies this velocity by frame time to move the player.
    player.velocityX = 0.0f;
    if (Keys[GLFW_KEY_A]) {
        player.velocityX = -260.0f;
    }
    if (Keys[GLFW_KEY_D]) {
        player.velocityX = 260.0f;
    }

    // E toggles claws once per press. KeysProcessed blocks repeated toggles while held;
    // releasing E clears the latch so the next press can toggle again.
    if (Keys[GLFW_KEY_E] && !KeysProcessed[GLFW_KEY_E]) {
        player.clawsExtended = !player.clawsExtended;
        KeysProcessed[GLFW_KEY_E] = true;
    }
    else if (!Keys[GLFW_KEY_E]) {
        KeysProcessed[GLFW_KEY_E] = false;
    }

    // Space starts a spinning jump with upward velocity and extended claws.
    // The isSpinning guard prevents restarting the move while it is already active.
    if (Keys[GLFW_KEY_SPACE] && !player.isSpinning) {
        player.isSpinning = true;
        player.velocityY = -600.0f;
        player.clawsExtended = true;
    }
}

void Game::update(float dt)
{
    if (player.hp <= 0.0f || State == GAME_WIN) {
        return;
    }

    const float escapeDistance = 4500.0f;

    player.update(dt);

    if (player.position.x < 40.0f) {
        player.position.x = 40.0f;
    }

    if (player.position.x >= escapeDistance) {
        player.position.x = escapeDistance;
        player.velocityX = 0.0f;
        State = GAME_WIN;
        drones.clear();
        return;
    }

    float targetCameraX = glm::max(0.0f, player.position.x - 240.0f);
    cameraX += (targetCameraX - cameraX) * glm::min(1.0f, 10.0f * dt);

    while (nextDroneSpawnX < cameraX + static_cast<float>(width) + 200.0f && nextDroneSpawnX < escapeDistance - 250.0f) {
        Drone d;
        d.position = glm::vec2(nextDroneSpawnX, 320.0f);
        d.baseY = 320.0f;
        drones.push_back(d);
        nextDroneSpawnX += 520.0f;
    }

    for (Drone& d : drones) {
        d.update(dt, player.position);
    }

    glm::vec2 chestCenter = player.position + glm::vec2(0.0f, -40.0f);
    glm::vec2 facingDir(player.facing, 0.0f);
    float slashReach = player.isSpinning ? 115.0f : 105.0f;
    float minCosTheta = player.isSpinning ? -1.0f : std::cos(glm::radians(55.0f));

    for (Drone& d : drones) {
        if (!d.alive) {
            continue;
        }

        glm::vec2 toDrone = d.position - chestCenter;
        float dist = glm::length(toDrone);

        if (player.clawExtend > 0.5f && player.slashCooldown <= 0.0f && dist > 0.001f && dist <= slashReach + d.radius) {
            glm::vec2 dir = toDrone / dist;
            float alignment = glm::dot(facingDir, dir);

            if (alignment >= minCosTheta) {
                float baseDmg = player.isSpinning ? 36.0f : 24.0f;
                d.hp -= player.computeBerserkerDamage(baseDmg);
                d.hitFlash = 1.0f;
                d.velocity += dir * (420.0f * player.clawExtend);
                d.attackCooldown = 0.65f;
                player.slashCooldown = 0.28f;

                if (d.hp <= 0.0f) {
                    d.hp = 0.0f;
                    d.alive = false;
                }
            }
        }

        float bodyRadius = 32.0f;
        if (d.alive && dist <= bodyRadius + d.radius && d.attackCooldown <= 0.0f) {
            player.takeDamage(18.0f);
            d.attackCooldown = 1.1f;
            if (dist > 0.001f) {
                d.velocity -= (toDrone / dist) * 220.0f;
            }
        }
    }
}

void Game::render()
{
    glm::mat4 projection = glm::ortho(
        cameraX,
        cameraX + static_cast<float>(width),
        static_cast<float>(height),
        0.0f,
        -1.0f,
        1.0f
    );

    Shader& spriteShader = ResourceManager::getShader("sprite");
    spriteShader.use();
    spriteShader.setMat4("projection", projection);

    Shader& shapeShader = ResourceManager::getShader("shape");
    shapeShader.use();
    shapeShader.setMat4("projection", projection);

    if (!labBackgrounds.empty()) {
        float segmentWidth = static_cast<float>(width);
        int firstSeg = static_cast<int>(std::floor(cameraX / segmentWidth));
        int lastSeg = static_cast<int>(std::floor((cameraX + segmentWidth) / segmentWidth));

        for (int seg = firstSeg; seg <= lastSeg; ++seg) {
            const std::string& texKey = labBackgrounds[seg % labBackgrounds.size()];
            spriteRenderer->drawSprite(
                ResourceManager::getTexture(texKey),
                glm::vec2(seg * segmentWidth, 0.0f),
                glm::vec2(segmentWidth, 520.0f),
                0.0f,
                glm::vec3(1.0f)
            );
        }
    }

    Texture2D& floorTex = ResourceManager::getTexture("floor");
    float tileWidth = 128.0f;
    float firstTileX = std::floor(cameraX / tileWidth) * tileWidth;
    float viewRight = cameraX + static_cast<float>(width);

    for (float x = firstTileX; x < viewRight; x += tileWidth) {
        spriteRenderer->drawSprite(
            floorTex,
            glm::vec2(x, 520.0f),
            glm::vec2(tileWidth, 80.0f),
            0.0f,
            glm::vec3(1.0f)
        );
    }

    shapeRenderer->drawQuad(glm::vec2(4520.0f, 260.0f), glm::vec2(40.0f, 520.0f), 0.0f, glm::vec4(0.15f, 0.85f, 0.45f, 0.35f));

    for (const Drone& d : drones) {
        d.draw(*shapeRenderer);
    }

    for (const Drone& d : drones) {
        if (!d.alive) {
            continue;
        }
        float ratio = d.hp / d.maxHp;
        shapeRenderer->drawQuad(d.position + glm::vec2(0.0f, -46.0f), glm::vec2(44.0f, 6.0f), 0.0f, glm::vec4(0.12f, 0.14f, 0.18f, 0.9f));
        shapeRenderer->drawQuad(d.position + glm::vec2(-22.0f * (1.0f - ratio), -46.0f), glm::vec2(44.0f * ratio, 6.0f), 0.0f, glm::vec4(0.92f, 0.18f, 0.20f, 1.0f));
    }

    player.draw(*shapeRenderer);

    glm::mat4 hudProj = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);
    shapeShader.use();
    shapeShader.setMat4("projection", hudProj);

    float hpRatio = glm::clamp(player.hp / player.maxHp, 0.0f, 1.0f);
    float rageRatio = glm::clamp((player.computeBerserkerDamage(1.0f) - 1.0f) / 1.8f, 0.0f, 1.0f);
    float escapeRatio = glm::clamp((player.position.x - 40.0f) / (4500.0f - 40.0f), 0.0f, 1.0f);

    shapeRenderer->drawQuad(glm::vec2(130.0f, 28.0f), glm::vec2(224.0f, 18.0f), 0.0f, glm::vec4(0.10f, 0.12f, 0.16f, 0.95f));
    shapeRenderer->drawQuad(glm::vec2(20.0f + 110.0f * hpRatio, 28.0f), glm::vec2(220.0f * hpRatio, 14.0f), 0.0f, glm::vec4(0.18f, 0.82f, 0.36f, 1.0f));

    shapeRenderer->drawQuad(glm::vec2(130.0f, 50.0f), glm::vec2(224.0f, 12.0f), 0.0f, glm::vec4(0.10f, 0.12f, 0.16f, 0.95f));
    shapeRenderer->drawQuad(glm::vec2(20.0f + 110.0f * rageRatio, 50.0f), glm::vec2(220.0f * rageRatio, 8.0f), 0.0f, glm::vec4(0.95f, 0.45f, 0.12f, 1.0f));

    float barRight = static_cast<float>(width) - 130.0f;
    float barStart = static_cast<float>(width) - 240.0f;
    shapeRenderer->drawQuad(glm::vec2(barRight, 28.0f), glm::vec2(224.0f, 16.0f), 0.0f, glm::vec4(0.10f, 0.12f, 0.16f, 0.95f));
    shapeRenderer->drawQuad(glm::vec2(barStart + 110.0f * escapeRatio, 28.0f), glm::vec2(220.0f * escapeRatio, 12.0f), 0.0f, glm::vec4(0.25f, 0.72f, 0.95f, 1.0f));

    if (player.hp <= 0.0f) {
        shapeRenderer->drawQuad(
            glm::vec2(static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f),
            glm::vec2(static_cast<float>(width), static_cast<float>(height)),
            0.0f,
            glm::vec4(0.55f, 0.05f, 0.06f, 0.45f)
        );
    }
    else if (State == GAME_WIN) {
        shapeRenderer->drawQuad(
            glm::vec2(static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f),
            glm::vec2(static_cast<float>(width), static_cast<float>(height)),
            0.0f,
            glm::vec4(0.10f, 0.55f, 0.28f, 0.35f)
        );
    }
}

Game::~Game()
{
}