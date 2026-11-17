#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.hpp"
#include "vao.hpp"
#include "vbo.hpp"
#include "ebo.hpp"

class ShapeRenderer {
public:
    explicit ShapeRenderer(Shader& shader);
    ~ShapeRenderer() = default;

    ShapeRenderer(const ShapeRenderer&) = delete;
    ShapeRenderer& operator=(const ShapeRenderer&) = delete;

    void drawTriangle(const glm::mat4& model, const glm::vec4& color) const;
    void drawTriangle(glm::vec2 position, glm::vec2 size, float rotate, const glm::vec4& color) const;

    void drawQuad(const glm::mat4& model, const glm::vec4& color) const;
    void drawQuad(glm::vec2 position, glm::vec2 size, float rotate, const glm::vec4& color) const;

    void drawCircle(const glm::mat4& model, const glm::vec4& color) const;
    void drawCircle(glm::vec2 position, glm::vec2 size, float rotate, const glm::vec4& color) const;

private:
    Shader& shader;

    VAO triVAO;
    VBO triVBO;

    VAO quadVAO;
    VBO quadVBO;
    EBO quadEBO;

    VAO circleVAO;
    VBO circleVBO;
    int circleVertexCount = 0;

    void initRenderData();
    static glm::mat4 composeModel(glm::vec2 position, glm::vec2 size, float rotate);
};