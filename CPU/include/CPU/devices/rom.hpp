#pragma once

#include <CPU/device.hpp>
#include <CPU/cpu.hpp>
#include <filesystem>
#include <fstream>

namespace CPU::CommonDevices {
    class ROM : public Device {
        EightDo::Common::ExtendedAddress start_addr;
        EightDo::Common::ExtendedAddress end_addr;
        uint8_t* rom;

    public:
        ROM(EightDo::Common::ExtendedAddress start, EightDo::Common::ExtendedAddress end) : start_addr(start), end_addr(end) {
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

        size_t relative(EightDo::Common::ExtendedAddress address) {
            return (size_t)address.address - (size_t)this->start_addr.address;
        }

        EightDo::Common::ExtendedAddress start() {
            return this->start_addr;
        }

        EightDo::Common::ExtendedAddress end() {
            return this->end_addr;
        }

        Result read(EightDo::Common::ExtendedAddress address) {
            if (address.address >= this->start_addr.address && address.address <= this->end_addr.address) {
                return { .status = Result::Status::Ok, .value = this->rom[this->relative(address)] };
            }

            return { .status = Result::Status::NotMyAddress, .value = 0x00 };
        }

        Result write(EightDo::Common::ExtendedAddress address, uint8_t data) {
            return { .status = Result::Status::ReadOnly, .value = 0x00 };
        }
    };
}