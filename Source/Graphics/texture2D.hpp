#pragma once
#include "glew/glew.h"

class Texture2D {
	private:
		unsigned int ID = 0;
		unsigned int width = 0;
		unsigned int height = 0;
		unsigned int internal_format = 0; // Format of texture object
		unsigned int image_format = 0; // Format of loaded image

		// Texture Configuration
		unsigned int wrap_s = 0; // Wrapping mode on S axis
		unsigned int wrap_t = 0; // Wrapping mode on T axis
		unsigned int filter_min = 0; // Filtering mode if texture pixels < screen pixels
		unsigned int filter_max = 0; // Filteriing mode if texture pixels > screen pixels
	public:
		Texture2D() = default;
		~Texture2D();

		// Copy Constructor
		Texture2D(const Texture2D&) = delete;
		Texture2D& operator=(const Texture2D&) = delete;

		// Move Constructor
		Texture2D(Texture2D&& other) noexcept;
		Texture2D& operator=(Texture2D&& other) noexcept;

		bool generate(
			const char* texturePath,
			bool flipVertically = false,
			unsigned int ws = GL_REPEAT,
			unsigned int wt = GL_REPEAT,
			unsigned int fmin = GL_LINEAR,
			unsigned int fmax = GL_LINEAR
		);
		void generate(
			unsigned int w,
			unsigned int h,
			unsigned char* data,
			unsigned int int_format = GL_RGBA,
			unsigned int img_format = GL_RGBA,
			unsigned int ws = GL_REPEAT,
			unsigned int wt = GL_REPEAT,
			unsigned int fmin = GL_LINEAR,
			unsigned int fmax = GL_LINEAR
		);
		void bind() const;
		void unbind() const;

		inline unsigned int getID() const { return ID; }
		inline unsigned int getWidth() const { return width; }
		inline unsigned int getHeight() const { return height; }
};