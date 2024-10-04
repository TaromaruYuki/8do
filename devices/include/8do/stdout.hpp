#pragma once

#include <8do/device.hpp>
#include <ncurses.h>
// https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/helloworld.html

namespace EightDo::Devices {
    class STDOut : public EightDo::Device {
        EightDo::Common::ExtendedAddress start_addr = 0x10000;
        EightDo::Common::ExtendedAddress end_addr   = 0x1FFFF;
        VideoDevice::Flags flags;
        VideoDevice::Resolution resolution;
    public:
        STDOut() : resolution(40, 30), flags(0) {
            initscr();
        }

        ~STDOut() {
            endwin();
        }

        Result read(EightDo::Common::ExtendedAddress address) {
            if(this->in_range(address)) {
                switch (this->relative(address)) {
                    case 0x0:
                        return { .status = Result::Status::Ok, .value = 0x00 };
                        break;
                    case 0x1:
                        return { .status = Result::Status::Ok, .value = this->flags.data};
                        break;
                    case 0x2:
                        return { .status = Result::Status::Ok, .value = this->resolution.width_hb };
                        break;
                    case 0x3:
                        return { .status = Result::Status::Ok, .value = this->resolution.width_lb };
                        break;
                    case 0x4:
                        return { .status = Result::Status::Ok, .value = this->resolution.height_hb };
                        break;
                    case 0x5:
                        return { .status = Result::Status::Ok, .value = this->resolution.height_lb };
                        break;
                    case 0x6: // Video Mode Count
                        return { .status = Result::Status::Ok, .value = 1};
                        break;
                    case 0x7:
                    case 0x8:
                    case 0x9:
                    case 0xA:
                    case 0xB:
                    case 0xC:
                    case 0xD:
                    case 0xE:
                    case 0xF:
                        return { .status = Result::Status::Ok, .value = 0x00 };
                        break;
                    default:
                        return { .status = Result::Status::Ok, .value = 0 };
                }
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

        Result update() {
            printw("Hello, world!");
            refresh();
        }
    };
}