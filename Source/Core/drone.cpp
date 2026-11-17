#include "drone.hpp"
#include <cmath>
#include <algorithm>

namespace {
    glm::mat4 makePart(const glm::mat4& parent, glm::vec2 pivot, float angle, glm::vec2 offset, glm::vec2 size) {
        glm::mat4 m = glm::translate(parent, glm::vec3(pivot, 0.0f));
        m = glm::rotate(m, angle, glm::vec3(0.0f, 0.0f, 1.0f));
        m = glm::translate(m, glm::vec3(offset, 0.0f));
        return glm::scale(m, glm::vec3(size, 1.0f));
    }
}

void Drone::update(float dt, glm::vec2 targetPos) {
    if (!alive) {
        return;
    }

    hoverTime += dt * 4.5f;
    bladeAngle += 540.0f * dt;
    if (bladeAngle >= 360.0f) {
        bladeAngle -= 360.0f;
    }

    if (hitFlash > 0.0f) {
        hitFlash = std::max(0.0f, hitFlash - dt * 4.0f);
    }
    if (attackCooldown > 0.0f) {
        attackCooldown = std::max(0.0f, attackCooldown - dt);
    }

    float dx = targetPos.x - position.x;
    float distX = std::abs(dx);

    float desiredVx = 0.0f;
    if (distX < 650.0f && distX > 45.0f) {
        desiredVx = (dx > 0.0f ? 1.0f : -1.0f) * 145.0f;
    }

    velocity.x += (desiredVx - velocity.x) * std::min(1.0f, 5.0f * dt);
    position.x += velocity.x * dt;

    float targetY = glm::clamp(targetPos.y - 35.0f, 220.0f, 430.0f);    baseY += (targetY - baseY) * std::min(1.0f, 2.5f * dt);
    position.y = baseY + std::sin(hoverTime) * 24.0f;
}

void Drone::draw(const ShapeRenderer& renderer) const {
    if (!alive) {
        return;
    }

    const glm::vec4 darkArmor(0.20f, 0.22f, 0.27f, 1.0f);
    const glm::vec4 midSteel(0.42f, 0.46f, 0.52f, 1.0f);
    const glm::vec4 bladeSilver(0.76f, 0.80f, 0.86f, 1.0f);
    const glm::vec4 sensorRed(0.95f, 0.16f, 0.18f, 1.0f);
    const glm::vec4 flashWhite(1.0f, 0.92f, 0.85f, 1.0f);

    glm::vec4 coreColor = (hitFlash > 0.0f) ? glm::mix(sensorRed, flashWhite, hitFlash) : sensorRed;
    float tiltAngle = velocity.x * 0.08f;

    glm::mat4 root = glm::mat4(1.0f);
    root = glm::translate(root, glm::vec3(position, 0.0f));
    root = glm::rotate(root, tiltAngle, glm::vec3(0.0f, 0.0f, 1.0f));

    // Complex objects: the drone is a second composite object built from primitives.
    // Four quad arms and triangular blades share the rotating rotorHub transform.
    glm::mat4 rotorHub = glm::rotate(root, bladeAngle, glm::vec3(0.0f, 0.0f, 1.0f));
    for (int i = 0; i < 4; ++i) {
        float armAngle = static_cast<float>(i) * 90.0f;
        renderer.drawQuad(makePart(rotorHub, glm::vec2(0.0f, 0.0f), armAngle, glm::vec2(0.0f, -26.0f), glm::vec2(5.0f, 22.0f)), midSteel);
        renderer.drawTriangle(makePart(rotorHub, glm::vec2(0.0f, 0.0f), armAngle, glm::vec2(0.0f, -40.0f), glm::vec2(14.0f, 12.0f)), bladeSilver);
    }

    // Triangular armor pieces and three concentric circles form the body and sensor.
    // They inherit root so the complete drone follows its position and tilt.
    renderer.drawTriangle(makePart(root, glm::vec2(0.0f, -22.0f), 0.0f, glm::vec2(0.0f, 0.0f), glm::vec2(26.0f, 14.0f)), darkArmor);
    renderer.drawTriangle(makePart(root, glm::vec2(0.0f, 22.0f), 180.0f, glm::vec2(0.0f, 0.0f), glm::vec2(26.0f, 14.0f)), darkArmor);

    renderer.drawCircle(makePart(root, glm::vec2(0.0f, 0.0f), 0.0f, glm::vec2(0.0f, 0.0f), glm::vec2(44.0f, 44.0f)), darkArmor);
    renderer.drawCircle(makePart(root, glm::vec2(0.0f, 0.0f), 0.0f, glm::vec2(0.0f, 0.0f), glm::vec2(30.0f, 30.0f)), midSteel);
    renderer.drawCircle(makePart(root, glm::vec2(0.0f, 0.0f), 0.0f, glm::vec2(0.0f, 0.0f), glm::vec2(14.0f, 14.0f)), coreColor);
}