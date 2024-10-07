#pragma once

#include <8do/device.hpp>
#include <8do/ansi.hpp>
#include <optional>

namespace EightDo::Devices {
    class STDOut : public EightDo::Device {
        enum struct Command {
            None = 0,
            MoveCursor = 1,
            PrintChar = 2,
        };

        EightDo::Common::ExtendedAddress start_addr = 0x10000;
        EightDo::Common::ExtendedAddress end_addr   = 0x1FFFF;
        VideoDevice::Flags flags;
        VideoDevice::Resolution resolution;
        Common::ConsoleWindow console;
        uint8_t data;
        Command current_command = Command::None;
        std::optional<uint8_t> command_data = std::nullopt;

    public:
        STDOut() : resolution(40, 30), flags(0) {
            console.hide_cursor();
        }

        ~STDOut() {
            console.show_cursor();
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
                    case 0x10: // Command
                        return { .status = Result::Status::WriteOnly, .value = 0x00 };
                        break;
                    case 0x11: // Data
                        return { .status = Result::Status::Ok, .value = data };
                    default:
                        return { .status = Result::Status::Ok, .value = 0 };
                }
            }

            return { .status = Result::Status::NotMyAddress, .value = 0x00 };
        }

        Result write(EightDo::Common::ExtendedAddress address, uint8_t data) {
            if (this->in_range(address)) {
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
                    return { .status = Result::Status::Ok, .value = 0x00 };
                    break;
                case 0x10:
                    switch (data) {
                        case 0x0: // Move Cursor Up
                            this->console.move_cursor_up();
                            break;
                        case 0x1: // Move Cursor Down
                            this->console.move_cursor_down();
                            break;
                        case 0x2: // Move Cursor Left
                            this->console.move_cursor_left();
                            break;
                        case 0x3: // Move Cursor Right
                            this->console.move_cursor_right();
                            break;
                        case 0x4: // Move Cursor To
                            this->current_command = Command::MoveCursor;
                            this->command_data = std::nullopt;
                            break;
                    }
                    break;
                case 0x11:
                    switch (this->current_command) {
                        case Command::MoveCursor:
                            if (this->command_data) {
                                this->console.move_cursor(this->command_data.value(), data);
                                this->current_command = Command::None;
                                this->command_data = std::nullopt;
                            }
                            else {
                                this->command_data = data;
                            }
                            break;
                        case Command::PrintChar:
                            this->console.print_char((char)data);
                            break;
                    }
                }
            }
            return { .status = Result::Status::NotMyAddress, .value = 0x00 };
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