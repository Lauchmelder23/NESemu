#pragma once

#include <vector>
#include "Types.hpp"

class Bus;
class Screen;

enum class ScanlineType
{
	PreRender,
	Visible,
	PostRender,
	VBlank
};

enum class CycleType
{
	Idle,
	Fetching,
	SpriteFetching,
	PreFetching,
	UnknownFetching
};

enum class FetchingPhase
{
	NametableByte,
	AttributeTableByte,
	PatternTableLo,
	PatternTableHi
};

union VRAMAddress
{
	struct
	{
		Word CoarseX : 5;
		Word CoarseY : 5;
		Word NametableSel : 2;
		Word FineY : 3;
	};

	Word Raw;
};

union ShiftRegister
{
	struct
	{
		Byte Lo;
		Byte Hi;
	};

	Word Raw;
};

struct Sprite
{
	Byte Lo, Hi;
	Byte Latch;
	Byte Counter;
	Byte FineX;
};

struct Pixel
{
	Byte color;
	Byte palette;
	Byte priority;
	bool isZeroSprite;
};

/**
 * @brief The PPU of the NES.
 */
class PPU
{
	friend class PPUWatcher;
	friend class OAMViewer;

public:
	static const std::vector<Color> colorTable;

public:
	PPU(Bus* bus, Screen* screen);

	/**
	 * @brief Powerup PPU.
	 * Internal state corresponds to powerup state
	 */
	void Powerup();

	/**
	 * @brief Powerup PPU.
	 * Internal state corresponds to reset state
	 */
	void Reset();

	/**
	 * @brief Tick PPU forward once.
	 */
	void Tick();

	/**
	 * @brief Read from memory mapped PPU regs.
	 */
	Byte ReadRegister(Byte id);

	/**
	 * @brief Write to memory mapped PPU regs.
	 */
	void WriteRegister(Byte id, Byte val);

	Byte ReadOAM(Byte offset);
	void WriteOAM(Byte offset, Byte val);

	/**
	 * @brief Check whether the PPU finished rendering a frame.
	 * Returns true if the VBlankStart cycle was hit previously. The function resets
	 * the boolearn when it is called
	 */
	inline bool IsFrameDone() { bool returnVal = isFrameDone; isFrameDone = false; return returnVal; }

private:
	/**
	 * @brief Wraps Bus::ReadPPU.
	 */
	Byte Read(Word addr);
	
	/**
	 * @brief Wraps Bus::WritePPU.
	 */
	void Write(Word addr, Byte val);

	void UpdateState();
	void EvaluateBackgroundTiles();
	void EvaluateSprites();

	Pixel GetBackgroundPixel();
	Pixel GetSpritePixel();

	Color MultiplexPixel(Pixel background, Pixel sprite);

private: // Registers

	union
	{
		struct
		{
			Byte BaseNametableAddr : 2;
			Byte VRAMAddrIncrement : 1;
			Byte SpritePatternTableAddr : 1;
			Byte BackgrPatternTableAddr : 1;
			Byte SpriteSize : 1;
			Byte MasterSlaveSelect : 1;
			Byte VBlankNMI : 1;
		} Flag;

		Byte Raw;
	} ppuctrl;

	union
	{
		struct
		{
			Byte Greyscale : 1;
			Byte BackgroundOnLeft : 1;
			Byte SpriteOnLeft : 1;
			Byte ShowBackground : 1;
			Byte ShowSprites : 1;
			Byte EmphasizeRed : 1;
			Byte EmphasizeGreen : 1;
			Byte EmphasizeBlue : 1;
		} Flag;

		Byte Raw;
	} ppumask;

	union
	{
		struct
		{
			Byte Unused : 5;
			Byte SpriteOverflow : 1;
			Byte SpriteZeroHit : 1;
			Byte VBlankStarted : 1;
		} Flag;

		Byte Raw;
	} ppustatus;

	struct
	{
		Byte x;
		Byte y;
	} ppuscroll;

	Address ppuaddr;
	Byte oamaddr;

	VRAMAddress current{ 0 };
	VRAMAddress temporary{ 0 };
	uint16_t fineX;
	bool addressLatch = false;

	Byte latch = 0;

	Word x, y;
	Byte nametableByte = 0x00;
	Byte attributeTableByte = 0x00;
	Byte patternTableLo = 0x00;
	Byte patternTableHi = 0x00;

	ShiftRegister loTile{ 0 };
	ShiftRegister hiTile{ 0 };
	ShiftRegister hiAttribute{ 0 };
	ShiftRegister loAttribute{ 0 };

	std::vector<Byte> OAM;
	std::vector<Byte> secondaryOAM;
	std::vector<Sprite> sprites;
	Byte OAMOverrideSignal = 0x00;
	Byte freeSecondaryOAMSlot = 0x00;
	Byte currentlyEvaluatedSprite = 0x00;

private:
	ScanlineType scanlineType;
	CycleType cycleType;
	FetchingPhase fetchPhase;

	uint8_t memoryAccessLatch = 0;
	bool isFrameDone = false;
	Bus* bus;
	Screen* screen;
};
