#pragma once
#include "glew/glew.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

class Shader {
	private:
		enum class Type {
			PROGRAM, VERTEX, FRAGMENT, GEOMETRY
		};
		unsigned int ID = 0;

		void checkErrors(unsigned int id, Type type);
	public:
		Shader() = default;
		~Shader();

		// Copy Constructor
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		// Move Constructor
		Shader(Shader&& other) noexcept;
		Shader& operator=(Shader&& other) noexcept;

		void compile(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
		void use() const;
		void setBool(const std::string& name, bool value) const;
		void setInt(const std::string& name, int value) const;
		void setFloat(const std::string& name, float value) const;
		void setVec3f(const std::string& name, const glm::vec3& vec) const;
		void setVec3f(const std::string& name, float x, float y, float z) const;
		void setMat4(const std::string& name, const glm::mat4& mat) const;
		void setVec4f(const std::string& name, const glm::vec4& vec) const;
		void setVec4f(const std::string& name, float x, float y, float z, float w) const;

		inline unsigned int getID() const { return ID; }
};