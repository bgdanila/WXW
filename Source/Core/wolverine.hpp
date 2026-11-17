#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Graphics/shape_renderer.hpp"

struct Wolverine {
    glm::vec2 position = glm::vec2(200.0f, 400.0f);
    float groundY = 400.0f;
    float facing = 1.0f;

    float velocityX = 0.0f;
    float velocityY = 0.0f;
    float runTime = 0.0f;

    bool clawsExtended = true;
    float clawExtend = 1.0f;

    bool isSpinning = false;
    float spinAngle = 0.0f;

    float hp = 100.0f;
    float maxHp = 100.0f;
    float timeSinceHit = 5.0f;
    float slashCooldown = 0.0f;
    float hurtFlash = 0.0f;

    void update(float dt);
    void draw(const ShapeRenderer& renderer) const;
    float computeBerserkerDamage(float baseDamage) const;
    void takeDamage(float amount);
};