#pragma once

#include <cstdint>
#include <vector>

struct Color
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

class Screen
{
public:
	Screen();
	~Screen();

	void SetPixel(uint16_t x, uint16_t y, Color color);
	void Render();

private:
	void CreateVertexArray();
	void CreateTexture();
	void CreateShader();

private:
	uint32_t texture = 0;
	uint32_t shader = 0;
	uint32_t vao = 0;
	uint32_t vbo = 0;

	std::vector<Color> pixels;
};
