#include "vao.hpp"
#include <utility>
#include <iostream>

VAO::VAO(VAO&& other) noexcept : ID(std::exchange(other.ID, 0)) {}
VAO& VAO::operator=(VAO&& other) noexcept {
	if (this != &other) {
		if (ID != 0) glDeleteVertexArrays(1, &ID);
		ID = std::exchange(other.ID, 0);
	}

	return *this;
}

void VAO::generate() {
	if (ID == 0) glGenVertexArrays(1, &ID);
}
void VAO::linkVBO(const VBO& vbo, unsigned int index, int size, GLenum type, GLboolean normalized, GLsizei stride, const void* offset) const{
	this->bind();
	vbo.bind();

	glVertexAttribPointer(index, size, type, normalized, stride, offset);
	glEnableVertexAttribArray(index);

	vbo.unbind();
	this->unbind();
}
void VAO::linkEBO(const EBO& ebo) const{
	this->bind();
	ebo.bind();

	ebo.unbind();
	this->unbind();
}
void VAO::bind() const{
	glBindVertexArray(ID);
}
void VAO::unbind() const{
	glBindVertexArray(0);
}

VAO::~VAO() {
	std::cout << "Destroying VAO: " << ID << "\n";
	if( ID != 0 ) glDeleteVertexArrays(1, &ID);
}