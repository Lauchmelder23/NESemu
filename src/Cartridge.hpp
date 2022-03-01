#pragma once

#include <string>
#include <memory>

#include "Types.hpp"
#include "Mapper.hpp"

class Bus;

/**
 * @brief Represents a cartridge and handles CPU/PPU read/writes.
 */
class Cartridge
{
public:
	/**
	 * @brief Add a cartridge to the Bus.
	 */
	Cartridge(Bus* bus);
	~Cartridge();

	/**
	 * @brief Read from the CPU.
	 */
	inline Byte ReadCPU(Word addr) { return mapper->ReadCPU(addr); }

	/**
	 * @brief Read from the PPU.
	 */
	inline Byte ReadPPU(Word addr) { return mapper->ReadPPU(addr); }

	/**
	 * @brief Wrote from the CPU.
	 */
	inline void WriteCPU(Word addr, Byte val) { mapper->WriteCPU(addr, val); }

	/**
	 * @brief Write from the PPU.
	 */
	inline void WritePPU(Word addr, Byte val) { mapper->WritePPU(addr, val); }

	
	inline bool MapCIRAM(Word& addr) { return mapper->MapCIRAM(addr); }
	inline Byte ReadVRAM(Word addr) { return mapper->ReadVRAM(addr); }
	inline void WriteVRAM(Word addr, Byte val) { mapper->WriteVRAM(addr, val); }

	/**
	 * @brief Load an iNES file from disk.
	 */
	void Load(std::string path);

	/**
	 * @brief Returns the Mapper used by the cartridge.
	 */
	inline Mapper* GetMapper() { return mapper; }

private:
	Mapper* mapper;
	Bus* bus;
};
