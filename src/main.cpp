#include <iostream>
#include <filesystem>
#include <fstream>
#include <8do/8do.hpp>

int main() {
    EightDo::Emulator emulator;
    CPU::CommonDevices::ROM* rom = new CPU::CommonDevices::ROM(0x0000, 0x7FFF);
    auto rom_res = rom->load_rom("../../../gen/hello_world.bin");

    if(rom_res.status != CPU::Device::Result::Status::Ok) {
        if(rom_res.status == CPU::Device::Result::Status::FileNotFound) {
            std::cerr << "ERROR: File not found" << std::endl;
        }
        else if(rom_res.status == CPU::Device::Result::Status::CannotReadFile) {
            std::cerr << "ERROR: Cannot read file" << std::endl;
        }

        return 1;
    }

    CPU::CommonDevices::RAM* ram = new CPU::CommonDevices::RAM(0x8000, 0xFFFF);

    emulator.add_device(rom);
    emulator.add_device(ram);

    emulator.start();

    return 0;
}