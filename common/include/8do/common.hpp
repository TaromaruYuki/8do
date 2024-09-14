#include <inttypes.h>

namespace EightDo::Common {
    enum struct ReadWrite {
        Read,
        Write
    };

    enum struct State {
        Reset,
        Fetch,
        Execute,
        Halt,
        Interrupt
    };

    enum struct InterruptStatus {
        None,
        Normal,
        NonMaskable
    };

    struct InterruptNumber {
        uint8_t value : 4;
        uint8_t _ : 4;                                     // Unused

        InterruptNumber(int number) {
            this->value = number & 0xF;
        }
    };

    union ExtendedAddress {
        struct {
            uint32_t value : 16;
            uint32_t extended : 2;
            uint32_t unused : 14;
        };

        uint32_t address : 18;

        ExtendedAddress() = default;

        ExtendedAddress(int number) {
            this->address = number & 0x3FFFF;
        }
    };

    struct Pins {
        ExtendedAddress address = 0x00000;
        uint8_t data = 0x00;
        ReadWrite rw = ReadWrite::Read;
        bool bus_enable = true;
        bool irq = false;                                 // Interrupt Request
        bool nmi = false;                                 // Non-maskable Interrupt
        bool iak = false;                                 // Interrupt Acknowledge
    };
}