#include "shader.hpp"
#include <array>
#include <utility>

Shader::Shader(Shader&& other) noexcept : ID(std::exchange(other.ID, 0)) {}
Shader& Shader::operator=(Shader&& other) noexcept {
	if (this != &other) {
		if( ID != 0 ) glDeleteProgram(ID);
		ID = std::exchange(other.ID, 0);
	}

	return *this;
}

void Shader::compile(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	if (geometryPath != nullptr) {
		gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	}

	try {
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

		if (geometryPath != nullptr) {
			gShaderFile.open(geometryPath);
			std::stringstream gShaderStream;

			gShaderStream << gShaderFile.rdbuf();

			gShaderFile.close();

			geometryCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure e) {
		std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		return;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, nullptr);
	glCompileShader(vertex);
	checkErrors(vertex, Type::VERTEX);

	unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, nullptr);
	glCompileShader(fragment);
	checkErrors(fragment, Type::FRAGMENT);
	
	unsigned int geometry = 0;
	if (geometryPath != nullptr) {
		const char* gShaderCode = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, nullptr);
		glCompileShader(geometry);
		checkErrors(geometry, Type::GEOMETRY);
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	if (geometryPath != nullptr) {
		glAttachShader(ID, geometry);
	}

	glLinkProgram(ID);
	checkErrors(ID, Type::PROGRAM);
	
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometryPath != nullptr) {
		glDeleteShader(geometry);
	}
}

void Shader::use() const {
	glUseProgram(ID);
}
void Shader::setBool(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
}
void Shader::setInt(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setVec3f(const std::string& name, const glm::vec3& vec) const {
	glUniform3f(glGetUniformLocation(ID, name.c_str()), vec.x, vec.y, vec.z);
}
void Shader::setVec3f(const std::string& name, float x, float y, float z) const {
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::setVec4f(const std::string& name, const glm::vec4& vec) const {
	glUniform4f(glGetUniformLocation(ID, name.c_str()), vec.x, vec.y, vec.z, vec.w);
}
void Shader::setVec4f(const std::string& name, float x, float y, float z, float w) const {
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::checkErrors(unsigned int id, Type type) {
	int succes = 0;
	std::array<char, 1024> infoLog = {};

	if (type == Type::PROGRAM) {
		glGetProgramiv(id, GL_LINK_STATUS, &succes);
		if (!succes) {
			glGetProgramInfoLog(id, infoLog.size(), nullptr, infoLog.data());
			std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog.data() << std::endl;
			return;
		}
	}
	else {
		glGetShaderiv(id, GL_COMPILE_STATUS, &succes);
		if (!succes) {
			glGetShaderInfoLog(id, infoLog.size(), nullptr, infoLog.data());
			const char* variant = "UNKNOWN";
			switch (type) {
			case Type::VERTEX:   variant = "VERTEX"; break;
			case Type::FRAGMENT: variant = "FRAGMENT"; break;
			case Type::GEOMETRY: variant = "GEOMETRY"; break;
			default: break;
			}

			std::cerr << "ERROR::SHADER::" << variant << "::COMPILATION_FAILED\n" << infoLog.data() << std::endl;
			return;
		}
	}
}

Shader::~Shader() {
	if (ID != 0) glDeleteProgram(ID);
}
