#include "shape_renderer.hpp"
#include <array>
#include <cmath>
#include <cstddef>

ShapeRenderer::ShapeRenderer(Shader& shader) : shader(shader) {
    initRenderData();
}

void ShapeRenderer::initRenderData() {
    // Primitive building blocks for the humanoid and drone: three vertices make a triangle.
    constexpr std::array<float, 6> triVertices = {
         0.0f, -0.5f,
        -0.5f,  0.5f,
         0.5f,  0.5f
    };

    triVAO.generate();
    triVBO.generate(triVertices.size() * sizeof(float), triVertices.data(), GL_STATIC_DRAW);
    triVAO.linkVBO(triVBO, 0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

    constexpr std::array<float, 8> quadVertices = {
        -0.5f,  0.5f,
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f
    };

    // Two indexed triangles share four corner vertices to form a rectangular quad.
    constexpr std::array<unsigned int, 6> quadIndices = {
        0, 1, 2,
        0, 2, 3
    };

    quadVAO.generate();
    quadVBO.generate(quadVertices.size() * sizeof(float), quadVertices.data(), GL_STATIC_DRAW);
    quadVAO.linkVBO(quadVBO, 0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

    quadVAO.bind();
    quadEBO.generate(quadIndices.size() * sizeof(unsigned int), quadIndices.data(), GL_STATIC_DRAW);
    quadEBO.bind();
    quadVAO.unbind();
    quadEBO.unbind();

    // Approximate a filled circle with a center and a closed ring of 32 segments.
    // drawCircle renders these vertices as a GL_TRIANGLE_FAN.
    constexpr int segments = 32;
    constexpr float twoPi = 6.28318530718f;
    std::array<float, (segments + 2) * 2> circleVertices{};

    circleVertices[0] = 0.0f;
    circleVertices[1] = 0.0f;

    for (int i = 0; i <= segments; ++i) {
        float angle = (static_cast<float>(i) / static_cast<float>(segments)) * twoPi;
        std::size_t idx = static_cast<std::size_t>((i + 1) * 2);
        circleVertices[idx] = 0.5f * std::cos(angle);
        circleVertices[idx + 1] = 0.5f * std::sin(angle);
    }

    circleVertexCount = segments + 2;

    circleVAO.generate();
    circleVBO.generate(circleVertices.size() * sizeof(float), circleVertices.data(), GL_STATIC_DRAW);
    circleVAO.linkVBO(circleVBO, 0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
}

glm::mat4 ShapeRenderer::composeModel(glm::vec2 position, glm::vec2 size, float rotate) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));
    return model;
}

void ShapeRenderer::drawTriangle(const glm::mat4& model, const glm::vec4& color) const {
    shader.use();
    shader.setMat4("model", model);
    shader.setVec4f("shapeColor", color);

    triVAO.bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    triVAO.unbind();
}

void ShapeRenderer::drawTriangle(glm::vec2 position, glm::vec2 size, float rotate, const glm::vec4& color) const {
    drawTriangle(composeModel(position, size, rotate), color);
}

void ShapeRenderer::drawQuad(const glm::mat4& model, const glm::vec4& color) const {
    shader.use();
    shader.setMat4("model", model);
    shader.setVec4f("shapeColor", color);

    quadVAO.bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    quadVAO.unbind();
}

void ShapeRenderer::drawQuad(glm::vec2 position, glm::vec2 size, float rotate, const glm::vec4& color) const {
    drawQuad(composeModel(position, size, rotate), color);
}

void ShapeRenderer::drawCircle(const glm::mat4& model, const glm::vec4& color) const {
    shader.use();
    shader.setMat4("model", model);
    shader.setVec4f("shapeColor", color);

    circleVAO.bind();
    glDrawArrays(GL_TRIANGLE_FAN, 0, circleVertexCount);
    circleVAO.unbind();
}

void ShapeRenderer::drawCircle(glm::vec2 position, glm::vec2 size, float rotate, const glm::vec4& color) const {
    drawCircle(composeModel(position, size, rotate), color);
}