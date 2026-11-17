#pragma once
#include "glew/glew.h"

class EBO {
	private:
		unsigned int ID = 0;
	public:
		EBO() = default;
		~EBO();

		// Copy Constructor
		EBO(const EBO&) = delete;
		EBO& operator=(const EBO&) = delete;

		// Move Constructor
		EBO(EBO&& other) noexcept;
		EBO& operator=(EBO&& other) noexcept;

		void generate(GLsizeiptr size, const void* data, GLenum usage);
		void bind() const;
		void unbind() const;

		inline unsigned int getID() const { return ID; }
};