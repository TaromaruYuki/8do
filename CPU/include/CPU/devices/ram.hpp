#pragma once

#include <CPU/device.hpp>
#include <CPU/cpu.hpp>

namespace CPU::CommonDevices {
    class RAM : public Device {
        CPU::CPU::ExtendedAddress start_addr;
        CPU::CPU::ExtendedAddress end_addr;
        uint8_t* ram;

    public:
        RAM(CPU::CPU::ExtendedAddress start, CPU::CPU::ExtendedAddress end) : start_addr(start), end_addr(end) {
            this->ram = new uint8_t[this->size()];
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
                return { .status = Result::Status::Ok, .value = this->ram[this->relative(address)] };
            }

            return { .status = Result::Status::NotMyAddress, .value = 0x00 };
        }

        Result write(CPU::CPU::ExtendedAddress address, uint8_t data) {
            if (address.address >= this->start_addr.address && address.address <= this->end_addr.address) {
                this->ram[this->relative(address)] = data;
                return { .status = Result::Status::Ok, .value = data };
            }

            return { .status = Result::Status::NotMyAddress, .value = 0x00 };
        }
    };
}