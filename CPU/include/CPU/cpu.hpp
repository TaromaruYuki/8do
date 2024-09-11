#pragma once

#include <inttypes.h>

namespace CPU {
    class CPU {
    public:
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
            uint8_t _: 4;                                     // Unused

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

    private:
        enum struct AddressingModes {
            Immediate,
            Absolute,
            Register,
            Pointer
        };

        union {
            struct {
                uint8_t reg0     : 3;
                uint8_t reg1     : 3;
                uint8_t ext_addr : 2;
            };
            uint8_t value;
        } metadata;                                           // Instruction Metadata (Byte 1)

        union {
            struct {
                uint8_t Z : 1;                                // Zero
                uint8_t N : 1;                                // Negative
                uint8_t C : 1;                                // Carry
                uint8_t O : 1;                                // Overflow
                uint8_t L : 1;                                // Less Than
                uint8_t G : 1;                                // Greater Than
                uint8_t I: 1;                                 // Interrupt Disable
                uint8_t _ : 1;		                          // Unused
            };
            uint8_t value;
        } flags;

        union {
            struct {
                uint16_t value : 10;
                uint16_t unused : 6;
            };

            uint16_t address;
        } sp;                                                 // Stack Pointer
                                  
        uint8_t ra = 0;                                       // A      Register
        uint8_t rb = 0;                                       // B      Register
        uint8_t rc = 0;                                       // C      Register
        uint8_t rd = 0;                                       // D      Register
        uint8_t ro = 0;                                       // Offset Register
                                         
        ExtendedAddress pc;                                   // Program Counter
        State state;
        int8_t cycleCount = 0;

        ExtendedAddress tempaddr = 0;                         // Temporary 18-bit value
        uint16_t temp16 = 0;                                  // Temporary 16-bit value
        uint8_t temp8 = 0;	                                  // Temporary 8-bit value
        uint8_t opcode = 0;	                                  // Current Opcode
        InterruptStatus int_status = InterruptStatus::None;
        InterruptNumber int_number = 0;

    public:
        CPU(Pins* pins) { this->reset(pins); };
        void reset(Pins* pins);
        InterruptStatus interrupt_status() { return this->int_status; }
        void cycle(Pins* pins);
        State get_state() const { return this->state; };

    private:
        void finish(Pins* pins);

        uint8_t& DecodeRegister(uint8_t reg);

        void reset_state_handler(Pins* pins);
        void fetch_state_handler(Pins* pins);
        void execute_state_handler(Pins* pins);
        void interrupt_state_handler(Pins* pins);

        void jump_if_flag(Pins* pins, bool flag);
        void jump_not_flag(Pins* pins, bool flag);

        void LDR(Pins* pins, AddressingModes addressing_mode);
        void STR(Pins* pins, AddressingModes addressing_mode);
        void ADD(Pins* pins, AddressingModes addressing_mode);
        void SUB(Pins* pins, AddressingModes addressing_mode);
        void JMP(Pins* pins, AddressingModes addressing_mode);
        void BIZ(Pins* pins);
        void BNZ(Pins* pins);
        void BIN(Pins* pins);
        void BNN(Pins* pins);
        void BIC(Pins* pins);
        void BNC(Pins* pins);
        void BIO(Pins* pins);
        void BNO(Pins* pins);
        void BIL(Pins* pins);
        void BNL(Pins* pins);
        void BIG(Pins* pins);
        void BNG(Pins* pins);
        void SBL(Pins* pins, AddressingModes addressing_mode);
        void SBR(Pins* pins, AddressingModes addressing_mode);
        void ROL(Pins* pins, AddressingModes addressing_mode);
        void ROR(Pins* pins, AddressingModes addressing_mode);
        void CLC(Pins* pins);
        void INC(Pins* pins, AddressingModes addressing_mode);
        void DEC(Pins* pins, AddressingModes addressing_mode);
        void HLT(Pins* pins);
        void CMP(Pins* pins, AddressingModes addressing_mode);
        void LDO(Pins* pins, AddressingModes addressing_mode);
        void NOP(Pins* pins);
        void PSH(Pins* pins, AddressingModes addressing_mode);
        void POP(Pins* pins, AddressingModes addressing_mode);
        void JSR(Pins* pins);
        void RET(Pins* pins);
        void BII(Pins* pins);
        void BNI(Pins* pins);
        void CLI(Pins* pins);
        void SEI(Pins* pins);
        void RTI(Pins* pins);
    };
}