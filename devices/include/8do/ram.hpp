#pragma once

#include <8do/device.hpp>

namespace EightDo::Devices {
    class RAM : public EightDo::Device {
        EightDo::Common::ExtendedAddress start_addr;
        EightDo::Common::ExtendedAddress end_addr;
        uint8_t* ram;

    public:
        RAM(EightDo::Common::ExtendedAddress start, EightDo::Common::ExtendedAddress end) : start_addr(start), end_addr(end) {
            this->ram = new uint8_t[this->size()];
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
                return { .status = Result::Status::Ok, .value = this->ram[this->relative(address)] };
            }

            return { .status = Result::Status::NotMyAddress, .value = 0x00 };
        }

        Result write(EightDo::Common::ExtendedAddress address, uint8_t data) {
            if (address.address >= this->start_addr.address && address.address <= this->end_addr.address) {
                this->ram[this->relative(address)] = data;
                return { .status = Result::Status::Ok, .value = data };
            }

            return { .status = Result::Status::NotMyAddress, .value = 0x00 };
        }
    };
}