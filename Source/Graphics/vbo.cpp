#include "vbo.hpp"
#include <utility>
#include <iostream>

VBO::VBO(VBO&& other) noexcept: ID(std::exchange(other.ID, 0)) {}
VBO& VBO::operator=(VBO&& other) noexcept {
	if (this != &other) {
		if (ID != 0) glDeleteBuffers(1, &ID);
		ID = std::exchange(other.ID, 0);
	}

	return *this;
}

void VBO::generate(GLsizeiptr size, const void* data, GLenum usage) {
	if (ID == 0) glGenBuffers(1, &ID);	
	this->bind();

	glBufferData(GL_ARRAY_BUFFER, size, data, usage);
	this->unbind();
};
void VBO::bind() const{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}
void VBO::unbind() const{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VBO::~VBO(){
	std::cout << "Destroying VBO: " << ID << "\n";
	if(ID != 0) glDeleteBuffers(1, &ID);
};
