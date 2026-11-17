#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Graphics/shape_renderer.hpp"

struct Drone {
    glm::vec2 position = glm::vec2(700.0f, 340.0f);
    float baseY = 340.0f;
    glm::vec2 velocity = glm::vec2(0.0f, 0.0f);

    float radius = 26.0f;
    float hp = 60.0f;
    float maxHp = 60.0f;
    bool alive = true;

    float hoverTime = 0.0f;
    float bladeAngle = 0.0f;
    float hitFlash = 0.0f;
    float attackCooldown = 0.0f;

    void update(float dt, glm::vec2 targetPos);
    void draw(const ShapeRenderer& renderer) const;
};