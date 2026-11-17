#include "texture2D.hpp"
#include "stb/stb_image.h"
#include <iostream>
#include <utility>


Texture2D::Texture2D(Texture2D&& other) noexcept
	: ID(std::exchange(other.ID, 0)),
	width(std::exchange(other.width, 0)),
	height(std::exchange(other.height, 0)),
	internal_format(std::exchange(other.internal_format, 0)),
	image_format(std::exchange(other.image_format, 0)),
	wrap_s(std::exchange(other.wrap_s, 0)),
	wrap_t(std::exchange(other.wrap_t, 0)),
	filter_min(std::exchange(other.filter_min, 0)),
	filter_max(std::exchange(other.filter_max, 0)) { }
Texture2D& Texture2D::operator=(Texture2D&& other) noexcept {
	if (this != &other) {
		if (ID != 0) glDeleteTextures(1, &ID);
		ID = std::exchange(other.ID, 0);
		width = std::exchange(other.width, 0);
		height = std::exchange(other.height, 0);
		internal_format = std::exchange(other.internal_format, 0);
		image_format = std::exchange(other.image_format, 0);
		wrap_s = std::exchange(other.wrap_s, 0);
		wrap_t = std::exchange(other.wrap_t, 0);
		filter_min = std::exchange(other.filter_min, 0);
		filter_max = std::exchange(other.filter_max, 0);
	}

	return *this;
}

bool Texture2D::generate(
	const char* texturePath,
	bool flipVertically,
	unsigned int ws,
	unsigned int wt,
	unsigned int fmin,
	unsigned int fmax
) {
	stbi_set_flip_vertically_on_load(flipVertically);

	int w = 0;
	int h = 0;
	int nrChannels = 0;
	
	std::cerr << "Attempting to open: [" << texturePath << "]\n";

	unsigned char* data = stbi_load(texturePath, &w, &h, &nrChannels, 0);

	if (data == nullptr) {
		std::cerr << "ERROR::TEXTURE::FAILED_TO_LOAD: " << texturePath
			<< " | Reason: " << stbi_failure_reason() << '\n';
		return false;
	}

	GLenum format = GL_RGBA;
	switch (nrChannels)
	{
		case 1: format = GL_RED; break;
		case 3: format = GL_RGB; break;
		case 4: format = GL_RGBA; break;
		default:
			std::cerr << "ERROR::TEXTURE::UNSUPORTED_CHANNELS (" << nrChannels << "): " << texturePath << '\n';
			stbi_image_free(data);
			break;
	}
	generate(static_cast<unsigned int>(w), static_cast<unsigned int>(h), data, format, format, ws, wt, fmin, fmax);
	stbi_image_free(data);
	std::cerr << "Successfully opened and generated texture: [" << texturePath << "]\n";
	return true;
}
void Texture2D::generate(
		unsigned int w,
		unsigned int h,
		unsigned char* data,
		unsigned int int_format,
		unsigned int img_format,
		unsigned int ws,
		unsigned int wt,
		unsigned int fmin,
		unsigned int fmax
	) {
	if (w == 0 || h == 0) {
		std::cerr << "ERROR::TEXTURE::INVALID_DIMENSIONS (0x0)\n";
		return;
	}

	width = w;
	height = h;
	internal_format = int_format;
	image_format = img_format;
	wrap_s = ws;
	wrap_t = wt;
	filter_min = fmin;
	filter_max = fmax;

	if(ID == 0 ) glGenTextures(1, &ID);

	this->bind();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, w, h, 0, image_format, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_max);
	this->unbind();
}

void Texture2D::bind() const{
	glBindTexture(GL_TEXTURE_2D, ID);
}
void Texture2D::unbind() const{
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture2D::~Texture2D() {
	if (ID != 0 ) glDeleteTextures(1, &ID);
}