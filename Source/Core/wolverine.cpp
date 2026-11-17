#include "wolverine.hpp"
#include <cmath>
#include <algorithm>

namespace {
    // Complex objects: transform each primitive relative to its parent body/joint.
    // Translate to the pivot, rotate, offset to the part center, then scale its size.
    glm::mat4 makeLimb(const glm::mat4& parent, glm::vec2 pivot, float angle, glm::vec2 offset, glm::vec2 size) {
        glm::mat4 m = glm::translate(parent, glm::vec3(pivot, 0.0f));
        m = glm::rotate(m, angle, glm::vec3(0.0f, 0.0f, 1.0f));
        m = glm::translate(m, glm::vec3(offset, 0.0f));
        return glm::scale(m, glm::vec3(size, 1.0f));
    }

    // Build an articulated arm from circles (joints) and quads (arm segments/fist).
    // All parts inherit the shoulder transform, so they move together when it swings.
    void drawArm(
        const ShapeRenderer& renderer,
        const glm::mat4& root,
        float shoulderX,
        float swingAngle,
        const glm::vec4& skinColor,
        const glm::vec4& clawColor,
        float clawExtend
    ) {
        renderer.drawCircle(makeLimb(root, glm::vec2(shoulderX, -26.0f), 0.0f, glm::vec2(0.0f, 0.0f), glm::vec2(18.0f, 18.0f)), skinColor);

        glm::mat4 armJoint = glm::translate(root, glm::vec3(shoulderX, -28.0f, 0.0f));
        armJoint = glm::rotate(armJoint, swingAngle, glm::vec3(0.0f, 0.0f, 1.0f));

        renderer.drawQuad(makeLimb(armJoint, glm::vec2(0.0f, 0.0f), 0.0f, glm::vec2(0.0f, 16.0f), glm::vec2(14.0f, 18.0f)), skinColor);
        renderer.drawCircle(makeLimb(armJoint, glm::vec2(0.0f, 9.0f), 0.0f, glm::vec2(0.0f, 16.0f), glm::vec2(15.0f, 15.0f)), skinColor);
        renderer.drawQuad(makeLimb(armJoint, glm::vec2(-2.0f, 24.0f), -45.0f, glm::vec2(0.0f, 16.0f), glm::vec2(10.0f, 20.0f)), skinColor);
        renderer.drawQuad(makeLimb(armJoint, glm::vec2(6.0f, 32.0f), -45.0f, glm::vec2(0.0f, 16.0f), glm::vec2(14.0f, 8.0f)), skinColor);

        if (clawExtend > 0.01f) {
            glm::mat4 fistJoint = glm::translate(armJoint, glm::vec3(6.0f, 32.0f, 0.0f));
            fistJoint = glm::rotate(fistJoint, -45.0f, glm::vec3(0.0f, 0.0f, 1.0f));
            fistJoint = glm::translate(fistJoint, glm::vec3(0.0f, 16.0f, 0.0f));

            // Three narrow triangles attached to the fist form the retractable claws.
            // clawExtend scales their length as they extend or retract.
            float clawLen = 22.0f * clawExtend;
            for (int i = -1; i <= 1; ++i) {
                glm::mat4 claw = makeLimb(
                    fistJoint,
                    glm::vec2(static_cast<float>(i) * 4.0f, 3.0f),
                    180.0f,
                    glm::vec2(0.0f, -clawLen * 0.5f),
                    glm::vec2(2.6f, clawLen)
                );
                renderer.drawTriangle(claw, clawColor);
            }
        }
    }

    // Build a leg from circular joints and rectangular thigh, shin and foot pieces.
    // The knee transform inherits the hip transform, keeping the lower leg attached.
    void drawLeg(
        const ShapeRenderer& renderer,
        const glm::mat4& root,
        float hipX,
        float thighAngle,
        float kneeAngle,
        const glm::vec4& thighColor,
        const glm::vec4& bootColor
    ) {
        renderer.drawCircle(makeLimb(root, glm::vec2(hipX, 18.0f), 0.0f, glm::vec2(0.0f, 0.0f), glm::vec2(15.0f, 15.0f)), thighColor);

        glm::mat4 hipJoint = glm::translate(root, glm::vec3(hipX, 18.0f, 0.0f));
        hipJoint = glm::rotate(hipJoint, thighAngle, glm::vec3(0.0f, 0.0f, 1.0f));

        renderer.drawQuad(makeLimb(hipJoint, glm::vec2(0.0f, 0.0f), 0.0f, glm::vec2(0.0f, 10.0f), glm::vec2(14.0f, 20.0f)), thighColor);
        renderer.drawCircle(makeLimb(hipJoint, glm::vec2(0.0f, 20.0f), 0.0f, glm::vec2(0.0f, 0.0f), glm::vec2(14.0f, 14.0f)), bootColor);

        glm::mat4 kneeJoint = glm::translate(hipJoint, glm::vec3(0.0f, 20.0f, 0.0f));
        kneeJoint = glm::rotate(kneeJoint, kneeAngle, glm::vec3(0.0f, 0.0f, 1.0f));

        renderer.drawQuad(makeLimb(kneeJoint, glm::vec2(0.0f, 0.0f), 0.0f, glm::vec2(0.0f, 10.0f), glm::vec2(13.0f, 20.0f)), bootColor);
        renderer.drawQuad(makeLimb(kneeJoint, glm::vec2(3.0f, 19.0f), 0.0f, glm::vec2(0.0f, 0.0f), glm::vec2(18.0f, 7.0f)), bootColor);
    }
}

void Wolverine::update(float dt) {
    position.x += velocityX * dt;

    if (velocityX != 0.0f) {
        facing = (velocityX > 0.0f) ? 1.0f : -1.0f;
        if (!isSpinning) {
            runTime += dt * 12.0f;
        }
    }
    else if (!isSpinning) {
        runTime = 0.0f;
    }

    float targetClaw = clawsExtended ? 1.0f : 0.0f;
    clawExtend += glm::clamp(targetClaw - clawExtend, -6.0f * dt, 6.0f * dt);

    if (isSpinning) {
        velocityY += 1400.0f * dt;
        position.y += velocityY * dt;
        spinAngle += 420.0f * dt;

        if (position.y >= groundY) {
            position.y = groundY;
            velocityY = 0.0f;
            isSpinning = false;
            spinAngle = 0.0f;
        }
    }
    timeSinceHit += dt;
    if (slashCooldown > 0.0f) {
        slashCooldown = std::max(0.0f, slashCooldown - dt);
    }
    if (hurtFlash > 0.0f) {
        hurtFlash = std::max(0.0f, hurtFlash - dt * 4.0f);
    }

    if (hp > 0.0f && hp < maxHp) {
        const float k = 0.45f;
        const float tau = 1.8f;
        float ramp = 1.0f - std::exp(-timeSinceHit / tau);
        float dH = k * (maxHp - hp) * ramp * dt;
        hp = std::min(maxHp, hp + dH);
    }
}

void Wolverine::draw(const ShapeRenderer& renderer) const {
    float scale = 2.0f;

    const glm::vec4 skinHighlight(0.93f, 0.74f, 0.60f, 1.0f);
    const glm::vec4 skinBase(0.88f, 0.68f, 0.54f, 1.0f);
    const glm::vec4 skinMid(0.83f, 0.62f, 0.48f, 1.0f);
    const glm::vec4 skinShadow(0.76f, 0.55f, 0.42f, 1.0f);
    const glm::vec4 skinBack(0.66f, 0.47f, 0.36f, 1.0f);

    const glm::vec4 visorRed(0.92f, 0.14f, 0.16f, 1.0f);
    const glm::vec4 darkGray(0.24f, 0.26f, 0.30f, 1.0f);
    const glm::vec4 steelGray(0.48f, 0.51f, 0.56f, 1.0f);
    const glm::vec4 clawSilver(0.78f, 0.82f, 0.88f, 1.0f);

    float bobY = (!isSpinning && velocityX != 0.0f) ? -std::abs(std::sin(runTime)) * 5.0f : 0.0f;
    float armSwing = isSpinning ? -20.0f : (velocityX != 0.0f ? -std::sin(runTime) * 45.0f : 10.0f);
    float legSwing = isSpinning ? -35.0f : (velocityX != 0.0f ? std::sin(runTime) * 42.0f : 0.0f);
    float frontKnee = isSpinning ? 65.0f : (velocityX != 0.0f ? std::max(0.0f, -std::cos(runTime)) * 50.0f : 0.0f);
    float backKnee = isSpinning ? 45.0f : (velocityX != 0.0f ? std::max(0.0f, std::cos(runTime)) * 50.0f : 0.0f);

    // A shared root positions, mirrors, scales and spins the complete humanoid.
    // Every body part below inherits this transform.
    glm::vec2 spinCenter(0.0f, -22.0f);
    glm::mat4 root = glm::mat4(1.0f);
    root = glm::translate(root, glm::vec3(position.x, position.y + bobY, 0.0f));
    root = glm::scale(root, glm::vec3(facing * scale, scale, 1.0f));
    root = glm::translate(root, glm::vec3(spinCenter, 0.0f));
    root = glm::rotate(root, -spinAngle, glm::vec3(0.0f, 0.0f, 1.0f));
    root = glm::translate(root, glm::vec3(-spinCenter, 0.0f));

    // Assemble Wolverine from back limbs, torso quads, a circular head, equipment,
    // then front limbs. Drawing in this order makes the front parts overlap the back.
    drawLeg(renderer, root, 4.0f, -legSwing, backKnee, skinBack, skinBack);
    drawArm(renderer, root, 6.0f, isSpinning ? -20.0f : -armSwing, skinBack, clawSilver, clawExtend);

    renderer.drawQuad(makeLimb(root, glm::vec2(0.0f, 0.0f), 0.0f, glm::vec2(0.0f, 0.0f), glm::vec2(14.0f, 12.0f)), skinShadow);
    renderer.drawQuad(makeLimb(root, glm::vec2(1.0f, -13.0f), 0.0f, glm::vec2(0.0f, 0.0f), glm::vec2(24.0f, 14.0f)), skinMid);
    renderer.drawQuad(makeLimb(root, glm::vec2(0.0f, -27.0f), 5.0f, glm::vec2(0.0f, 0.0f), glm::vec2(32.0f, 16.0f)), skinHighlight);
    renderer.drawQuad(makeLimb(root, glm::vec2(0.0f, 12.0f), 0.0f, glm::vec2(0.0f, 0.0f), glm::vec2(20.0f, 12.0f)), darkGray);

    renderer.drawQuad(makeLimb(root, glm::vec2(2.0f, -24.0f), 5.0f, glm::vec2(0.0f, 0.0f), glm::vec2(28.0f, 3.0f)), darkGray);
    renderer.drawCircle(makeLimb(root, glm::vec2(7.0f, -24.0f), 0.0f, glm::vec2(0.0f, 0.0f), glm::vec2(7.0f, 7.0f)), steelGray);
    renderer.drawQuad(makeLimb(root, glm::vec2(-4.0f, 11.0f), 0.0f, glm::vec2(0.0f, 0.0f), glm::vec2(8.0f, 10.0f)), steelGray);

    renderer.drawQuad(makeLimb(root, glm::vec2(0.0f, -39.0f), 5.0f, glm::vec2(0.0f, 0.0f), glm::vec2(14.0f, 8.0f)), skinShadow);
    renderer.drawCircle(makeLimb(root, glm::vec2(0.0f, -50.0f), 0.0f, glm::vec2(0.0f, 0.0f), glm::vec2(22.0f, 22.0f)), skinBase);
    renderer.drawQuad(makeLimb(root, glm::vec2(1.0f, -55.0f), 5.0f, glm::vec2(0.0f, 0.0f), glm::vec2(26.0f, 14.0f)), steelGray);
    renderer.drawQuad(makeLimb(root, glm::vec2(9.0f, -52.5f), 5.0f, glm::vec2(0.0f, 0.0f), glm::vec2(11.0f, 4.0f)), visorRed);
    renderer.drawQuad(makeLimb(root, glm::vec2(-8.0f, -56.0f), 5.0f, glm::vec2(0.0f, 0.0f), glm::vec2(8.0f, 10.0f)), darkGray);

    drawLeg(renderer, root, -3.0f, legSwing, frontKnee, skinBase, skinMid);
    drawArm(renderer, root, -4.0f, armSwing, skinBase, clawSilver, clawExtend);
}

float Wolverine::computeBerserkerDamage(float baseDamage) const {
    float missingRatio = glm::clamp(1.0f - (hp / maxHp), 0.0f, 1.0f);
    const float alpha = 1.8f;
    return baseDamage * (1.0f + alpha * missingRatio * missingRatio);
}

void Wolverine::takeDamage(float amount) {
    hp = std::max(0.0f, hp - amount);
    timeSinceHit = 0.0f;
    hurtFlash = 1.0f;
}