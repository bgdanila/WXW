#pragma once
#include "shader.hpp"
#include "texture2D.hpp"
#include "vao.hpp"
#include "vbo.hpp"

class SpriteRenderer
{
    private:
        Shader& shader;
        VAO quadVAO;
        VBO quadVBO;

        void initRenderData();

    public:
        explicit SpriteRenderer(Shader& shader);
        ~SpriteRenderer() = default;

        SpriteRenderer(const SpriteRenderer&) = delete;
        SpriteRenderer& operator=(const SpriteRenderer&) = delete;

        void drawSprite(
            const Texture2D& texture, 
            glm::vec2 position,
            glm::vec2 size = glm::vec2(10.0f, 10.0f), 
            float rotate = 0.0f,
            glm::vec3 color = glm::vec3(1.0f)
        );

};