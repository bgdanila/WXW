#include "resource_manager.hpp"

#include <iostream>
#include <sstream>
#include <fstream>

#include "stb/stb_image.h"

std::map<std::string, Texture2D>    ResourceManager::Textures;
std::map<std::string, Shader>       ResourceManager::Shaders;

Shader& ResourceManager::loadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name)
{
    Shaders[name].compile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
}
Shader& ResourceManager::getShader(std::string name)
{
    return Shaders.at(name);
}

Texture2D& ResourceManager::loadTexture(const char* file, std::string name)
{
    Texture2D texture;
    if(!texture.generate(file)) std::cerr << "WARNING::RESOURCE_MANAGER: Failed to load texture '" << name << "'\n";
    Textures[name] = std::move(texture);
    return Textures[name];
}
Texture2D& ResourceManager::getTexture(std::string name)
{
    return Textures.at(name);
}

void ResourceManager::clear()
{
    Shaders.clear();
    Textures.clear();
}