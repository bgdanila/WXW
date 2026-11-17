#pragma once
#include "glew/glew.h"
#include "vbo.hpp"
#include "ebo.hpp"

class VAO {
	private:
		unsigned int ID = 0;
	public:
		VAO() = default;
		~VAO();

		// Copy Constructor
		VAO(const VAO&) = delete;
		VAO& operator=(const VAO&) = delete;

		// Move Constructor
		VAO(VAO&& other) noexcept;
		VAO& operator=(VAO&& other) noexcept;

		void generate();
		void linkVBO(const VBO& vbo, unsigned int index, int size, GLenum type, GLboolean normalized, GLsizei stride, const void* offset) const;
		void linkEBO(const EBO& ebo) const;
		void bind() const;
		void unbind() const;
		
		inline unsigned int getID() const { return ID; }
};