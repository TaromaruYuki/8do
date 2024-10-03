#pragma once

#include <8do/device.hpp>

namespace EightDo::Devices {
    class STDOut : public EightDo::Device {
        EightDo::Common::ExtendedAddress start_addr = 0x10000;
        EightDo::Common::ExtendedAddress end_addr   = 0x1FFFF;
    public:
        Result read(EightDo::Common::ExtendedAddress address) {
            if(this->in_range(address)) {
                
            }
        }

        Result write(EightDo::Common::ExtendedAddress address, uint8_t data) {
            Result result = { .status = Result::Status::NotMyAddress, .value = 0x00 };
            return result;
        }

        size_t relative(EightDo::Common::ExtendedAddress address) {
            return (size_t)address.address - (size_t)this->start_addr.address;
        }

        size_t size() {
            return (size_t)this->end_addr.address - (size_t)this->start_addr.address;
        }

        EightDo::Common::ExtendedAddress start() { return this->start_addr; }
        EightDo::Common::ExtendedAddress end() { return this->end_addr; }
    };
}