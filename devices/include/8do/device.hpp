#pragma once

#include <inttypes.h>
#include <8do/common.hpp>

namespace EightDo {
    struct Device {
        struct Result {
            enum Status {
                Ok = 0,
                NotMyAddress = 1,
                ReadOnly = 2,
                WriteOnly = 3,
                InvalidAddress = 4,
                FileNotFound = 5,
                CannotReadFile = 6,
                NoValidDevice = 7
            } status;
            uint8_t value;
        };

        virtual Result read(EightDo::Common::ExtendedAddress address) { Result result = { .status = Result::Status::NotMyAddress, .value = 0x00 }; return result; }
        virtual Result write(EightDo::Common::ExtendedAddress address, uint8_t data) { Result result = { .status = Result::Status::NotMyAddress, .value = 0x00 }; return result; }
        virtual size_t relative(EightDo::Common::ExtendedAddress) { return -1; }
        virtual size_t size() { return -1; }
        virtual bool in_range(EightDo::Common::ExtendedAddress address) {
            return address.address >= this->start().address && address.address <= this->end().address;
        }
        virtual EightDo::Common::ExtendedAddress start() { EightDo::Common::ExtendedAddress address = 0; return address; }
        virtual EightDo::Common::ExtendedAddress end() { EightDo::Common::ExtendedAddress address = 0; return address; }
    };

    struct VideoDevice : public Device {
        virtual uint8_t* vram_array() { return nullptr; }
        virtual size_t vram_size() { return 0; }
    };
}