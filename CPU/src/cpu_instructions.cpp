#include <CPU/cpu.hpp>
#include <limits>

void CPU::CPU::LDR(EightDo::Common::Pins* pins, AddressingModes addressing_mode) {
    switch(addressing_mode) {
        case AddressingModes::Immediate:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    this->DecodeRegister(this->metadata.reg0) = pins->data;
                    this->pc.address++;
                    this->finish(pins);
                break;
            }
        break;
        case AddressingModes::Absolute:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 3:
                    this->DecodeRegister(this->metadata.reg0) = pins->data;
                    this->pc.address++;
                    this->finish(pins);
                break;
            }
        break;
        case AddressingModes::Register:
            switch(this->cycleCount) {
                case 0:
                    this->DecodeRegister(this->metadata.reg0) = this->DecodeRegister(this->metadata.reg1);
                    this->finish(pins);
                break;
            }
        break;
        case AddressingModes::Pointer:
            switch (this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    this->tempaddr.value = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 2:
                    this->tempaddr.value |= pins->data;
                    this->tempaddr.extended = this->metadata.ext_addr;
                    pins->address = this->tempaddr;
                break;
                case 3:
                    this->temp8 = pins->data & 0x3;
                    pins->address.address = ++this->tempaddr.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 4:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->tempaddr.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 5:
                    this->temp16 |= pins->data;
                    pins->address.address = this->temp16;
                    pins->address.extended = this->temp8;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 6:
                    this->DecodeRegister(this->metadata.reg0) = pins->data;
                    this->pc.address++;
                    this->finish(pins);
                break;
            }
        break;
    }
}

void CPU::CPU::STR(EightDo::Common::Pins* pins, AddressingModes addressing_mode) {
    switch(addressing_mode) {
        case AddressingModes::Absolute:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->data = this->DecodeRegister(this->metadata.reg0);
                    pins->rw = EightDo::Common::ReadWrite::Write;
                break;
                case 3:
                    this->pc.address++;
                    this->finish(pins);
                break;
            }
        break;
    }
}

void CPU::CPU::ADD(EightDo::Common::Pins* pins, AddressingModes addressing_mode) {
    switch(addressing_mode) {
        case AddressingModes::Immediate:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    uint8_t& reg = this->DecodeRegister(this->metadata.reg0);
                    this->temp16 = (uint16_t)reg + (uint16_t)pins->data + (uint16_t)this->flags.C;

                    this->flags.C = this->temp16 > 0xFF;
                    this->flags.O = (~((uint16_t)reg ^ (uint16_t)pins->data) & ((uint16_t)reg ^ this->temp16)) & 0x0080; // 6502 Signed Overflow Logic (OLC)
                    this->flags.Z = (this->temp16 & 0xFF) == 0;
                    this->flags.N = (this->temp16 & 0xFF) & 0x80;
                    
                    reg = this->temp16 & 0xFF;

                    this->pc.address++;
                    this->finish(pins);
                break;
            }
        break;
        case AddressingModes::Absolute:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 3:
                    uint8_t& reg = this->DecodeRegister(this->metadata.reg0);
                    this->temp16 = (uint16_t)reg + (uint16_t)pins->data + (uint16_t)this->flags.C;

                    this->flags.C = this->temp16 > 0xFF;
                    this->flags.O = (~((uint16_t)reg ^ (uint16_t)pins->data) & ((uint16_t)reg ^ this->temp16)) & 0x0080; // 6502 Signed Overflow Logic (OLC)
                    this->flags.Z = (this->temp16 & 0xFF) == 0;
                    this->flags.N = (this->temp16 & 0xFF) & 0x80;

                    reg = this->temp16 & 0xFF;
                    
                    this->pc.address++;
                    this->finish(pins);
                break;
            }
        break;
        case AddressingModes::Register:
            switch(this->cycleCount) {
                case 0:
                    uint8_t& reg0 = this->DecodeRegister(this->metadata.reg0);
                    uint8_t& reg1 = this->DecodeRegister(this->metadata.reg1);
                    this->temp16 = (uint16_t)reg0 + (uint16_t)reg1 + (uint16_t)this->flags.C;

                    this->flags.C = this->temp16 > 0xFF;
                    this->flags.O = (~((uint16_t)reg0 ^ (uint16_t)reg1) & ((uint16_t)reg0 ^ this->temp16)) & 0x0080; // 6502 Signed Overflow Logic (OLC)
                    this->flags.Z = (this->temp16 & 0xFF) == 0;
                    this->flags.N = (this->temp16 & 0xFF) & 0x80;

                    reg0 = this->temp16 & 0xFF;
                    
                    this->finish(pins);
                break;
            }
        break;
    }
}

void CPU::CPU::SUB(EightDo::Common::Pins* pins, AddressingModes addressing_mode) {
    switch(addressing_mode) {
        case AddressingModes::Immediate:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    uint8_t& reg = this->DecodeRegister(this->metadata.reg0);
                    uint16_t data = ((uint16_t)pins->data) ^ 0x00FF;
                    this->temp16 = (uint16_t)reg + data + (uint16_t)this->flags.C;

                    this->flags.C = this->temp16 > 0xFF;
                    this->flags.O = (this->temp16 ^ (uint16_t)reg) & (this->temp16 ^ data) & 0x0080; // 6502 Signed Overflow Logic (OLC)
                    this->flags.Z = (this->temp16 & 0xFF) == 0;
                    this->flags.N = (this->temp16 & 0xFF) & 0x80;
                    
                    reg = this->temp16 & 0xFF;

                    this->pc.address++;
                    this->finish(pins);
                break;
            }
        break;
        case AddressingModes::Absolute:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 3:
                    uint8_t& reg = this->DecodeRegister(this->metadata.reg0);
                    uint16_t data = ((uint16_t)pins->data) ^ 0x00FF;
                    this->temp16 = (uint16_t)reg + data + (uint16_t)this->flags.C;

                    this->flags.C = this->temp16 > 0xFF;
                    this->flags.O = (this->temp16 ^ (uint16_t)reg) & (this->temp16 ^ data) & 0x0080; // 6502 Signed Overflow Logic (OLC)
                    this->flags.Z = (this->temp16 & 0xFF) == 0;
                    this->flags.N = (this->temp16 & 0xFF) & 0x80;

                    reg = this->temp16 & 0xFF;
                    
                    this->pc.address++;
                    this->finish(pins);
                break;
            }
        break;
        case AddressingModes::Register:
            switch(this->cycleCount) {
                case 0:
                    uint8_t& reg0 = this->DecodeRegister(this->metadata.reg0);
                    uint8_t& reg1 = this->DecodeRegister(this->metadata.reg1);
                    uint16_t data = ((uint16_t)reg1) ^ 0x00FF;
                    this->temp16 = (uint16_t)reg0 + data + (uint16_t)this->flags.C;

                    this->flags.C = this->temp16 > 0xFF;
                    this->flags.O = (this->temp16 ^ (uint16_t)reg0) & (this->temp16 ^ data) & 0x0080; // 6502 Signed Overflow Logic (OLC)
                    this->flags.Z = (this->temp16 & 0xFF) == 0;
                    this->flags.N = (this->temp16 & 0xFF) & 0x80;

                    reg0 = this->temp16 & 0xFF;
                    
                    this->finish(pins);
                break;
            }
        break;
    }
}

void CPU::CPU::JMP(EightDo::Common::Pins* pins, AddressingModes addressing_mode) {
    switch (addressing_mode) {
        case AddressingModes::Absolute:
            switch (this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    this->pc.value = this->temp16;
                    this->pc.extended = this->metadata.ext_addr;
                    this->finish(pins);
                break;
            }
        break;
        case AddressingModes::Pointer:
            switch (this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    this->tempaddr.value = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 2:
                    this->tempaddr.value |= pins->data;
                    this->tempaddr.extended = this->metadata.ext_addr;
                    pins->address = this->tempaddr;
                break;
                case 3:
                    this->temp8 = pins->data & 0x3;
                    pins->address.address = ++this->tempaddr.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 4:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->tempaddr.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 5:
                    this->temp16 |= pins->data;
                    this->pc.value = this->temp16;
                    this->pc.extended = this->temp8;
                    this->finish(pins);
                break;
            }
        break;
    }
}

void CPU::CPU::BIZ(EightDo::Common::Pins* pins) {
    this->jump_if_flag(pins, this->flags.Z);
}

void CPU::CPU::BNZ(EightDo::Common::Pins* pins) {
    this->jump_not_flag(pins, this->flags.Z);
}

void CPU::CPU::BIN(EightDo::Common::Pins* pins) {
    this->jump_if_flag(pins, this->flags.N);
}

void CPU::CPU::BNN(EightDo::Common::Pins* pins) {
    this->jump_not_flag(pins, this->flags.N);
}

void CPU::CPU::BIC(EightDo::Common::Pins* pins) {
    this->jump_if_flag(pins, this->flags.C);
}

void CPU::CPU::BNC(EightDo::Common::Pins* pins) {
    this->jump_not_flag(pins, this->flags.C);
}

void CPU::CPU::BIO(EightDo::Common::Pins* pins) {
    this->jump_if_flag(pins, this->flags.O);
}

void CPU::CPU::BNO(EightDo::Common::Pins* pins) {
    this->jump_not_flag(pins, this->flags.O);
}

void CPU::CPU::BIL(EightDo::Common::Pins* pins) {
    this->jump_if_flag(pins, this->flags.L);
}

void CPU::CPU::BNL(EightDo::Common::Pins* pins) {
    this->jump_not_flag(pins, this->flags.L);
}

void CPU::CPU::BIG(EightDo::Common::Pins* pins) {
    this->jump_if_flag(pins, this->flags.G);
}

void CPU::CPU::BNG(EightDo::Common::Pins* pins) {
    this->jump_not_flag(pins, this->flags.G);
}

void CPU::CPU::SBL(EightDo::Common::Pins* pins, AddressingModes addressing_mode) {
    switch(addressing_mode) {
        case AddressingModes::Absolute:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 3:
                    this->flags.C = pins->data & 0x80;
                    
                    pins->data <<= 1;

                    this->flags.Z = pins->data == 0;
                    this->flags.N = pins->data & 0x80;

                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = EightDo::Common::ReadWrite::Write;
                break;
                case 4:
                    this->pc.address++;
                    this->finish(pins);
                break;
            }
        break;
        case AddressingModes::Register:
            uint8_t& reg = this->DecodeRegister(this->metadata.reg0);
            this->flags.C = reg & 0x80;

            reg <<= 1;

            this->flags.Z = reg == 0;
            this->flags.N = reg & 0x80;

            this->finish(pins);
        break;
    }
}

void CPU::CPU::SBR(EightDo::Common::Pins* pins, AddressingModes addressing_mode) {
    switch(addressing_mode) {
        case AddressingModes::Absolute:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 3:
                    this->flags.C = pins->data & 0x01;
                    
                    pins->data >>= 1;

                    this->flags.Z = pins->data == 0;
                    this->flags.N = pins->data & 0x80;

                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = EightDo::Common::ReadWrite::Write;
                break;
                case 4:
                    this->pc.address++;
                    this->finish(pins);
                break;
            }
        break;
        case AddressingModes::Register:
            uint8_t& reg = this->DecodeRegister(this->metadata.reg0);
            this->flags.C = reg & 0x01;

            reg >>= 1;

            this->flags.Z = reg == 0;
            this->flags.N = reg & 0x80;

            this->finish(pins);
        break;
    }
}

void CPU::CPU::ROL(EightDo::Common::Pins* pins, AddressingModes addressing_mode) {
    switch(addressing_mode) {
        case AddressingModes::Absolute:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 3:
                    this->flags.C = pins->data & 0x80;
                    
                    pins->data = (pins->data << 1) | (pins->data >> 7);

                    this->flags.Z = pins->data == 0;
                    this->flags.N = pins->data & 0x80;

                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = EightDo::Common::ReadWrite::Write;
                break;
                case 4:
                    this->pc.address++;
                    this->finish(pins);
                break;
            }
        break;
        case AddressingModes::Register:
            uint8_t& reg = this->DecodeRegister(this->metadata.reg0);
            this->flags.C = reg & 0x80;

            reg = (reg << 1) | (reg >> 7);

            this->flags.Z = reg == 0;
            this->flags.N = reg & 0x80;

            this->finish(pins);
        break;
    }
}

void CPU::CPU::ROR(EightDo::Common::Pins* pins, AddressingModes addressing_mode) {
    switch(addressing_mode) {
        case AddressingModes::Absolute:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 3:
                    this->flags.C = pins->data & 0x01;
                    
                    pins->data = (pins->data >> 1) | (pins->data << 7);

                    this->flags.Z = pins->data == 0;
                    this->flags.N = pins->data & 0x80;

                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = EightDo::Common::ReadWrite::Write;
                break;
                case 4:
                    this->pc.address++;
                    this->finish(pins);
                break;
            }
        break;
        case AddressingModes::Register:
            uint8_t& reg = this->DecodeRegister(this->metadata.reg0);
            this->flags.C = reg & 0x01;

            reg = (reg >> 1) | (reg << 7);

            this->flags.Z = reg == 0;
            this->flags.N = reg & 0x80;

            this->finish(pins);
        break;
    }
}

void CPU::CPU::CLC(EightDo::Common::Pins* pins) {
    this->flags.C = 0;
    this->finish(pins);
}

void CPU::CPU::INC(EightDo::Common::Pins* pins, AddressingModes addressing_mode) {
    switch(addressing_mode) {
        case AddressingModes::Absolute:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 3:
                    this->flags.Z = ++pins->data == 0;
                    this->flags.N = pins->data & 0x80;

                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = EightDo::Common::ReadWrite::Write;
                break;
                case 4:
                    this->pc.address++;
                    this->finish(pins);
                break;
            }
        break;
        case AddressingModes::Register:
            uint8_t& reg = this->DecodeRegister(this->metadata.reg0);

            this->flags.Z = ++reg == 0;
            this->flags.N = reg & 0x80;

            this->finish(pins);
        break;
    }
}

void CPU::CPU::DEC(EightDo::Common::Pins* pins, AddressingModes addressing_mode) {
    switch(addressing_mode) {
        case AddressingModes::Absolute:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 3:
                    this->flags.Z = --pins->data == 0;
                    this->flags.N = pins->data & 0x80;

                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = EightDo::Common::ReadWrite::Write;
                break;
                case 4:
                    this->pc.address++;
                    this->finish(pins);
                break;
            }
        break;
        case AddressingModes::Register:
            uint8_t& reg = this->DecodeRegister(this->metadata.reg0);

            this->flags.Z = --reg == 0;
            this->flags.N = reg & 0x80;

            this->finish(pins);
        break;
    }
}

void CPU::CPU::HLT(EightDo::Common::Pins* pins) {
    this->state = EightDo::Common::State::Halt;
    this->finish(pins);
}

void CPU::CPU::CMP(EightDo::Common::Pins* pins, AddressingModes addressing_modes) {
    switch(addressing_modes) {
        case AddressingModes::Immediate:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    uint8_t& reg = this->DecodeRegister(this->metadata.reg0);
                    
                    this->flags.Z = reg == pins->data;
                    this->flags.N = (reg - pins->data) & 0x80;
                    this->flags.G = reg > pins->data;
                    this->flags.L = reg < pins->data;

                    this->pc.address++;
                    this->finish(pins);
                break;
            }
        break;
        case AddressingModes::Absolute:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 3:
                    uint8_t& reg = this->DecodeRegister(this->metadata.reg0);
                    
                    this->flags.Z = reg == pins->data;
                    this->flags.N = (reg - pins->data) & 0x80;
                    this->flags.G = reg > pins->data;
                    this->flags.L = reg < pins->data;

                    this->pc.address++;
                    this->finish(pins);
                break;
            }
        break;
        case AddressingModes::Register:
            uint8_t& reg0 = this->DecodeRegister(this->metadata.reg0);
            uint8_t& reg1 = this->DecodeRegister(this->metadata.reg1);
            
            this->flags.Z = reg0 == reg1;
            this->flags.N = (reg0 - reg1) & 0x80;
            this->flags.G = reg0 > reg1;
            this->flags.L = reg0 < reg1;

            this->finish(pins);
        break;
    }
}

void CPU::CPU::LDO(EightDo::Common::Pins* pins, AddressingModes addressing_mode) {
    switch (addressing_mode) {
        case AddressingModes::Absolute:
            switch (this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->address.address += this->ro;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 3:
                    this->DecodeRegister(this->metadata.reg0) = pins->data;
                    this->pc.address++;
                    this->finish(pins);
                break;
            }
        break;
        case AddressingModes::Register:
           switch(this->cycleCount) {
               case 0: 
                   pins->address = this->DecodeRegister(this->metadata.reg1) + this->ro;
                   pins->rw = EightDo::Common::ReadWrite::Read;
               break;
               case 1:
                   this->DecodeRegister(this->metadata.reg0) = pins->data;
                   this->finish(pins);
                break;
           }
        break;
        case AddressingModes::Pointer:
            switch (this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    this->tempaddr.value = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 2:
                    this->tempaddr.value |= pins->data;
                    this->tempaddr.extended = this->metadata.ext_addr;
                    pins->address = this->tempaddr;
                break;
                case 3:
                    this->temp8 = pins->data & 0x3;
                    pins->address.address = ++this->tempaddr.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 4:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->tempaddr.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 5:
                    this->temp16 |= pins->data;
                    pins->address.address = this->temp16;
                    pins->address.extended = this->temp8;
                    pins->address.address += this->ro;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 6:
                    this->DecodeRegister(this->metadata.reg0) = pins->data;
                    this->pc.address++;
                    this->finish(pins);
                break;
            }
        break;
    }
}

void CPU::CPU::NOP(EightDo::Common::Pins* pins) {
    this->finish(pins);
}

void CPU::CPU::PSH(EightDo::Common::Pins* pins, AddressingModes addressing_mode) {
    switch (addressing_mode) {
        case AddressingModes::Immediate:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    pins->address.address = this->sp.address;
                    pins->rw = EightDo::Common::ReadWrite::Write;
                break;
                case 2:
                    this->sp.value++;
                    this->pc.address++;
                    this->finish(pins);
            }
        break;
        case AddressingModes::Absolute:
            switch (this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 3:
                    pins->address.address = this->sp.address;
                    pins->rw = EightDo::Common::ReadWrite::Write;
                break;
                case 4:
                    this->sp.value++;
                    this->pc.address++;
                    this->finish(pins);
                break;
            }
        break;
        case AddressingModes::Register:
            switch (this->cycleCount) {
                case 0:
                    pins->address.address = this->sp.address;
                    pins->data = this->DecodeRegister(this->metadata.reg0);
                    pins->rw = EightDo::Common::ReadWrite::Write;
                break;
                case 1:
                    this->sp.value++;
                    this->finish(pins);
                break;
            }
        break;
    }
}

void CPU::CPU::POP(EightDo::Common::Pins* pins, AddressingModes addressing_mode) {
    switch(addressing_mode) {
        case AddressingModes::Absolute:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 3:
                    pins->address.address = (--this->sp.address) | 0xFC00;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 4:
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = EightDo::Common::ReadWrite::Write;
                break;
                case 5:
                    this->finish(pins);
                break;
            }
        break;
        case AddressingModes::Register:
            switch (this->cycleCount) {
                case 0:
                    pins->address.address = (--this->sp.address) | 0xFC00;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 1:
                    this->DecodeRegister(this->metadata.reg0) = pins->data;

                    this->finish(pins);
                break;
            }
        break;
    }
}

void CPU::CPU::JSR(EightDo::Common::Pins* pins) {
    switch (this->cycleCount) {
        case 0:
            pins->address = this->pc;
            pins->rw = EightDo::Common::ReadWrite::Read;
        break;
        case 1:
            this->temp16 = pins->data << 8;
            pins->address.address = ++this->pc.address;
            pins->rw = EightDo::Common::ReadWrite::Read;
        break;
        case 2:
            this->temp16 |= pins->data;
            pins->address.address = this->sp.address;
            pins->data = ++this->pc.address >> 8;
            pins->rw = EightDo::Common::ReadWrite::Write;
        break;
        case 3:
            this->sp.value++;
            pins->address.address = this->sp.address;
            pins->data = this->pc.value & 0xFF;
            pins->rw = EightDo::Common::ReadWrite::Write;
        break;
        case 4:
            this->sp.value++;
            pins->address.address = this->sp.address;
            pins->data = this->pc.extended;
            pins->rw = EightDo::Common::ReadWrite::Write;
        break;
        case 5:
            this->sp.value++;
            this->pc.value = this->temp16;
            this->pc.extended = this->metadata.ext_addr;
            this->finish(pins);
        break;
    }
}

void CPU::CPU::RET(EightDo::Common::Pins* pins) {
    switch(this->cycleCount) {
        case 0:
            pins->address.address = (--this->sp.address) | 0xFC00;
            pins->rw = EightDo::Common::ReadWrite::Read;
        break;
        case 1:
            this->temp8 = pins->data;
            pins->address.address = (--this->sp.address) | 0xFC00;
            pins->rw = EightDo::Common::ReadWrite::Read;
        break;
        case 2:
            this->temp16 = pins->data;
            pins->address.address = (--this->sp.address) | 0xFC00;
            pins->rw = EightDo::Common::ReadWrite::Read;
        break;
        case 3:
            this->temp16 |= pins->data << 8;
            this->pc.value = this->temp16;
            this->pc.extended = this->temp8;

            this->finish(pins);
        break;
    }
}

void CPU::CPU::BII(EightDo::Common::Pins* pins) {
    this->jump_if_flag(pins, this->flags.I);
}

void CPU::CPU::BNI(EightDo::Common::Pins* pins) {
    this->jump_not_flag(pins, this->flags.I);
}

void CPU::CPU::CLI(EightDo::Common::Pins* pins) {
    this->flags.I = 0;
    this->finish(pins);
}

void CPU::CPU::SEI(EightDo::Common::Pins* pins) {
    this->flags.I = 1;
    this->finish(pins);
}

void CPU::CPU::RTI(EightDo::Common::Pins* pins) {
    switch (this->cycleCount) {
        case 0:
            pins->address.address = this->sp.address | 0xFC00;
            pins->rw = EightDo::Common::ReadWrite::Read;
        break;
        case 1:
            this->flags.value = pins->data;
            pins->address.address = (--this->sp.address) | 0xFC00;
            pins->rw = EightDo::Common::ReadWrite::Read;
            break;
        case 2:
            this->tempaddr.extended = pins->data;
            pins->address.address = (--this->sp.address) | 0xFC00;
            pins->rw = EightDo::Common::ReadWrite::Read;
        break;
        case 3:
            this->tempaddr.value = pins->data;
            pins->address.address = (--this->sp.address) | 0xFC00;
            pins->rw = EightDo::Common::ReadWrite::Read;
        break;
        case 4:
            this->tempaddr.value |= pins->data << 8;
            this->pc = this->tempaddr;

            this->finish(pins);
        break;
    }
}
