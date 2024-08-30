#pragma once

#include <CPU/device.hpp>
#include <CPU/cpu.hpp>
#include <filesystem>
#include <fstream>

namespace CPU::CommonDevices {
    class ROM : public Device {
        CPU::CPU::ExtendedAddress start_addr;
        CPU::CPU::ExtendedAddress end_addr;
        uint8_t* rom;

    public:
        ROM(CPU::CPU::ExtendedAddress start, CPU::CPU::ExtendedAddress end) : start_addr(start), end_addr(end) {
            this->rom = new uint8_t[this->size()];
        }

        Result load_rom(std::string file) {
            if (!std::filesystem::exists(file)) {
                return { .status = Result::Status::FileNotFound, .value = 1 };
            }

            std::ifstream fs(file, std::ios::in | std::ios::binary);

            fs.read(reinterpret_cast<char*>(this->rom), this->size());

            if (fs.fail() || fs.bad()) {
                return { .status = Result::Status::CannotReadFile, .value = 1 };
            }

            fs.close();

            return { .status = Result::Status::Ok, .value = 0 };
        }

        size_t size() {
            return (size_t)this->end_addr.address - (size_t)this->start_addr.address;
        }

        size_t relative(CPU::CPU::ExtendedAddress address) {
            return (size_t)address.address - (size_t)this->start_addr.address;
        }

        CPU::CPU::ExtendedAddress start() {
            return this->start_addr;
        }

        CPU::CPU::ExtendedAddress end() {
            return this->end_addr;
        }

        Result read(CPU::CPU::ExtendedAddress address) {
            if (address.address >= this->start_addr.address && address.address <= this->end_addr.address) {
                return { .status = Result::Status::Ok, .value = this->rom[this->relative(address)] };
            }

            return { .status = Result::Status::NotMyAddress, .value = 0x00 };
        }

        Result write(CPU::CPU::ExtendedAddress address, uint8_t data) {
            return { .status = Result::Status::ReadOnly, .value = 0x00 };
        }
    };
}