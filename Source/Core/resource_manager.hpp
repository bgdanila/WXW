#pragma once

#include <map>
#include <string>

#include <glew/glew.h>

#include "../Graphics/texture2D.hpp"
#include "../Graphics/shader.hpp"

class ResourceManager {
	private:
		ResourceManager() = default;

	public:
		// Resource Storage
		static std::map<std::string, Shader> Shaders;
		static std::map<std::string, Texture2D> Textures;

		static Shader& loadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name);
		static Shader& getShader(std::string name);

		static Texture2D& loadTexture(const char* file, std::string name);
		static Texture2D& getTexture(std::string name);

		static void clear();
};