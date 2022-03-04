#pragma once

#include <vector>
#include "DebugWindow.hpp"
#include "../Types.hpp"

class Mapper;

class PatternTableViewer : 
	public DebugWindow
{
public:
	PatternTableViewer(Debugger* debugger, Mapper* mapper);
	~PatternTableViewer();

	virtual void OnRender() override;

private:
	void DecodePatternTable(int index, std::vector<Color>& buffer);

private:
	Mapper* mapper;
	uint32_t texture;
};
