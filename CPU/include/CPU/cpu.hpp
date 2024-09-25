#pragma once

#include <inttypes.h>
#include <8do/common.hpp>

namespace CPU {
    class CPU {
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
                                         
        EightDo::Common::ExtendedAddress pc;                                   // Program Counter
        EightDo::Common::State state;
        int8_t cycleCount = 0;

        EightDo::Common::ExtendedAddress tempaddr = 0;                         // Temporary 18-bit value
        uint16_t temp16 = 0;                                  // Temporary 16-bit value
        uint8_t temp8 = 0;	                                  // Temporary 8-bit value
        uint8_t opcode = 0;	                                  // Current Opcode
        EightDo::Common::InterruptStatus int_status = EightDo::Common::InterruptStatus::None;
        EightDo::Common::InterruptNumber int_number = 0;

    public:
        CPU(EightDo::Common::Pins* pins) { this->reset(pins); };
        void reset(EightDo::Common::Pins* pins);
        EightDo::Common::InterruptStatus interrupt_status() { return this->int_status; }
        void cycle(EightDo::Common::Pins* pins);
        EightDo::Common::State get_state() const { return this->state; };

        EightDo::Common::ExtendedAddress* get_pc() {
            return &this->pc;
        }

    private:
        void finish(EightDo::Common::Pins* pins);

        uint8_t& DecodeRegister(uint8_t reg);

        void reset_state_handler(EightDo::Common::Pins* pins);
        void fetch_state_handler(EightDo::Common::Pins* pins);
        void execute_state_handler(EightDo::Common::Pins* pins);
        void interrupt_state_handler(EightDo::Common::Pins* pins);

        void jump_if_flag(EightDo::Common::Pins* pins, bool flag);
        void jump_not_flag(EightDo::Common::Pins* pins, bool flag);

        void LDR(EightDo::Common::Pins* pins, AddressingModes addressing_mode);
        void STR(EightDo::Common::Pins* pins, AddressingModes addressing_mode);
        void ADD(EightDo::Common::Pins* pins, AddressingModes addressing_mode);
        void SUB(EightDo::Common::Pins* pins, AddressingModes addressing_mode);
        void JMP(EightDo::Common::Pins* pins, AddressingModes addressing_mode);
        void BIZ(EightDo::Common::Pins* pins);
        void BNZ(EightDo::Common::Pins* pins);
        void BIN(EightDo::Common::Pins* pins);
        void BNN(EightDo::Common::Pins* pins);
        void BIC(EightDo::Common::Pins* pins);
        void BNC(EightDo::Common::Pins* pins);
        void BIO(EightDo::Common::Pins* pins);
        void BNO(EightDo::Common::Pins* pins);
        void BIL(EightDo::Common::Pins* pins);
        void BNL(EightDo::Common::Pins* pins);
        void BIG(EightDo::Common::Pins* pins);
        void BNG(EightDo::Common::Pins* pins);
        void SBL(EightDo::Common::Pins* pins, AddressingModes addressing_mode);
        void SBR(EightDo::Common::Pins* pins, AddressingModes addressing_mode);
        void ROL(EightDo::Common::Pins* pins, AddressingModes addressing_mode);
        void ROR(EightDo::Common::Pins* pins, AddressingModes addressing_mode);
        void CLC(EightDo::Common::Pins* pins);
        void INC(EightDo::Common::Pins* pins, AddressingModes addressing_mode);
        void DEC(EightDo::Common::Pins* pins, AddressingModes addressing_mode);
        void HLT(EightDo::Common::Pins* pins);
        void CMP(EightDo::Common::Pins* pins, AddressingModes addressing_mode);
        void LDO(EightDo::Common::Pins* pins, AddressingModes addressing_mode);
        void NOP(EightDo::Common::Pins* pins);
        void PSH(EightDo::Common::Pins* pins, AddressingModes addressing_mode);
        void POP(EightDo::Common::Pins* pins, AddressingModes addressing_mode);
        void JSR(EightDo::Common::Pins* pins);
        void RET(EightDo::Common::Pins* pins);
        void BII(EightDo::Common::Pins* pins);
        void BNI(EightDo::Common::Pins* pins);
        void CLI(EightDo::Common::Pins* pins);
        void SEI(EightDo::Common::Pins* pins);
        void RTI(EightDo::Common::Pins* pins);
    };
}