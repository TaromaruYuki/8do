#pragma once

#include <8do/device.hpp>
#include <raylib-cpp.hpp>

namespace EightDo::Devices {
    class Video : public EightDo::VideoDevice {
        enum struct Mode {
            ClassicText = 0
        };

        Mode video_mode;
        uint8_t* vram;
        size_t vram_size_t = 0xFFEF;

        EightDo::Common::ExtendedAddress start_addr = 0x10000;
        EightDo::Common::ExtendedAddress end_addr   = 0x1FFFF;

    public:
        Video() {
            this->vram = new uint8_t[this->vram_size_t];

            uint8_t clear_buf[] = { 'A', 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00 };
            uint8_t counter = 0;
            for(size_t i = 0; i < this->vram_size_t; i++) {
                this->vram[i] = clear_buf[counter++];
                counter %= 7;
            }
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
                switch (this->relative(address)) {
                    case 0x0:
                        return { .status = Result::Status::Ok, .value = (uint8_t)this->video_mode };
                        break;
                    case 0x1:
                    case 0x2:
                    case 0x3:
                    case 0x4:
                    case 0x5:
                    case 0x6:
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
                        return { .status = Result::Status::Ok, .value = this->vram[this->relative(address) - 0x10] };
                }
            }

            return { .status = Result::Status::NotMyAddress, .value = 0x00 };
        }

        Result write(EightDo::Common::ExtendedAddress address, uint8_t data) {
            if (address.address >= this->start_addr.address && address.address <= this->end_addr.address) {
                switch (this->relative(address)) {
                case 0x0:
                case 0x1:
                case 0x2:
                case 0x3:
                case 0x4:
                case 0x5:
                case 0x6:
                case 0x7:
                case 0x8:
                case 0x9:
                case 0xA:
                case 0xB:
                case 0xC:
                case 0xD:
                case 0xE:
                case 0xF:
                    return { .status = Result::Status::ReadOnly, .value = 0x00 };
                    break;
                default:
                    this->vram[this->relative(address) - 0x10] = data;
                    return { .status = Result::Status::Ok, .value = data };
                }
            }

            return { .status = Result::Status::NotMyAddress, .value = 0x00 };
        }

        uint8_t* vram_array() { return this->vram; }
        size_t vram_size() const { return this->vram_size_t; }
    };
}