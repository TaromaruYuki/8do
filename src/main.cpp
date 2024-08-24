#include <iostream>
#include <filesystem>
#include <fstream>
#include <8do/8do.hpp>

int main() {
	EightDo::Emulator emulator;
	CPU::CommonDevices::ROM* rom = new CPU::CommonDevices::ROM({ .address = 0x0000 }, { .address = 0x7FFF });
	rom->load_rom("../../../gen/hello_world.bin");

	CPU::CommonDevices::RAM* ram = new CPU::CommonDevices::RAM({ .address = 0x8000 }, { .address = 0xFFFF });

	emulator.start();

	return 0;
}