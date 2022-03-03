#include "Screen.hpp"

#include <glad/glad.h>
#include <string>
#include <stdexcept>

#include "../Log.hpp"

Screen::Screen()
{
	pixels.resize(256 * 240);

	LOG_CORE_INFO("Creating vertex arrays");
	CreateVertexArray();

	LOG_CORE_INFO("Creating screen texture");
	CreateTexture();

	LOG_CORE_INFO("Creating screen shader");
	CreateShader();
}

Screen::~Screen()
{
	glDeleteTextures(1, &texture);
	glDeleteProgram(shader);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void Screen::SetPixel(uint16_t x, uint16_t y, Color color)
{
	pixels[y * 256 + x] = color;
}

void Screen::Render()
{
	glTextureSubImage2D(texture, 0, 0, 0, 256, 240, GL_RGB, GL_UNSIGNED_BYTE, (const void*)pixels.data());

	glBindTexture(GL_TEXTURE_2D, texture);
	glUseProgram(shader);
	glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Screen::CreateVertexArray()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	float vertices[4 * (3 + 2)] = {
		-1.0f, -1.0f, 0.0f,		0.0f, 1.0f,
		-1.0f,  1.0f, 0.0f,		0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,		1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,		1.0f, 1.0f
	};

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void Screen::CreateTexture()
{
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glTextureStorage2D(texture, 1, GL_RGB8, 256, 240);

	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Screen::CreateShader()
{
	GLint status;
	shader = glCreateProgram();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char* vertexShaderSource = R"(
		#version 460 core
		
		layout (location = 0) in vec3 a_Pos;
		layout (location = 1) in vec2 a_UV;

		out vec2 uvCoords;

		void main()
		{
			uvCoords = a_UV;
			gl_Position = vec4(a_Pos, 1.0f);
		}
	)";
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		char errorBuf[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, errorBuf);
		glDeleteShader(vertexShader);

		throw std::runtime_error("Vertex shader compilation error: " + std::string(errorBuf));
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragmentShaderSource = R"(
		#version 460 core
		
		out vec4 FragColor;
		in vec2 uvCoords;

		uniform sampler2D screen;

		void main()
		{
			FragColor = texture(screen, uvCoords);
		}
	)";
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		char errorBuf[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorBuf);
		glDeleteShader(fragmentShader);
		glDeleteShader(vertexShader);

		throw std::runtime_error("Fragment shader compilation error: " + std::string(errorBuf));
	}

	glAttachShader(shader, vertexShader);
	glAttachShader(shader, fragmentShader);
	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		char errorBuf[512];
		glGetProgramInfoLog(shader, 512, NULL, errorBuf);
		glDeleteShader(fragmentShader);
		glDeleteShader(vertexShader);

		throw std::runtime_error("Shader link error: " + std::string(errorBuf));
	}

	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
}
