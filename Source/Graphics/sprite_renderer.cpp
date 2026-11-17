#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "sprite_renderer.hpp"
#include <array>

SpriteRenderer::SpriteRenderer(Shader& shader)
    : shader(shader)
{
    initRenderData();
}

void SpriteRenderer::initRenderData()
{
    // configure VAO/VBO
    std::array<float, 24> vertices = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    
    quadVAO.generate();
    quadVBO.generate(vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    quadVAO.linkVBO(quadVBO, 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
}

void SpriteRenderer::drawSprite(
    const Texture2D& texture,
    glm::vec2 position,
    glm::vec2 size,
    float rotate,
    glm::vec3 color
) {
    shader.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    model = glm::scale(model, glm::vec3(size, 1.0f));

    shader.setMat4("model", model);
    shader.setVec3f("spriteColor", color);

    glActiveTexture(GL_TEXTURE0);
    texture.bind();

    quadVAO.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    quadVAO.unbind();
}