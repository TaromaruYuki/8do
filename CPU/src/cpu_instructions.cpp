#include <CPU/cpu.hpp>
#include <limits>

void CPU::CPU::LDR(Pins* pins, AddressingModes addressing_mode) {
    switch(addressing_mode) {
        case AddressingModes::Immediate:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = ReadWrite::Read;
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
                    pins->rw = ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = ReadWrite::Read;
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
                    pins->rw = ReadWrite::Read;
                break;
                case 1:
                    this->tempaddr.value = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = ReadWrite::Read;
                break;
                case 2:
                    this->tempaddr.value |= pins->data;
                    this->tempaddr.extended = this->metadata.ext_addr;
                    pins->address = this->tempaddr;
                break;
                case 3:
                    this->temp8 = pins->data & 0x3;
                    pins->address.address = ++this->tempaddr.address;
                    pins->rw = ReadWrite::Read;
                break;
                case 4:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->tempaddr.address;
                    pins->rw = ReadWrite::Read;
                break;
                case 5:
                    this->temp16 |= pins->data;
                    pins->address.address = this->temp16;
                    pins->address.extended = this->temp8;
                    pins->rw = ReadWrite::Read;
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

void CPU::CPU::STR(Pins* pins, AddressingModes addressing_mode) {
    switch(addressing_mode) {
        case AddressingModes::Absolute:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->data = this->DecodeRegister(this->metadata.reg0);
                    pins->rw = ReadWrite::Write;
                break;
                case 3:
                    this->pc.address++;
                    this->finish(pins);
                break;
            }
        break;
    }
}

void CPU::CPU::ADD(Pins* pins, AddressingModes addressing_mode) {
    switch(addressing_mode) {
        case AddressingModes::Immediate:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = ReadWrite::Read;
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
                    pins->rw = ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = ReadWrite::Read;
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

void CPU::CPU::SUB(Pins* pins, AddressingModes addressing_mode) {
    switch(addressing_mode) {
        case AddressingModes::Immediate:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = ReadWrite::Read;
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
                    pins->rw = ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = ReadWrite::Read;
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

void CPU::CPU::JMP(Pins* pins, AddressingModes addressing_mode) {
    switch (addressing_mode) {
        case AddressingModes::Absolute:
            switch (this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = ReadWrite::Read;
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
                    pins->rw = ReadWrite::Read;
                break;
                case 1:
                    this->tempaddr.value = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = ReadWrite::Read;
                break;
                case 2:
                    this->tempaddr.value |= pins->data;
                    this->tempaddr.extended = this->metadata.ext_addr;
                    pins->address = this->tempaddr;
                break;
                case 3:
                    this->temp8 = pins->data & 0x3;
                    pins->address.address = ++this->tempaddr.address;
                    pins->rw = ReadWrite::Read;
                break;
                case 4:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->tempaddr.address;
                    pins->rw = ReadWrite::Read;
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

void CPU::CPU::BIZ(Pins* pins) {
    this->jump_if_flag(pins, this->flags.Z);
}

void CPU::CPU::BNZ(Pins* pins) {
    this->jump_not_flag(pins, this->flags.Z);
}

void CPU::CPU::BIN(Pins* pins) {
    this->jump_if_flag(pins, this->flags.N);
}

void CPU::CPU::BNN(Pins* pins) {
    this->jump_not_flag(pins, this->flags.N);
}

void CPU::CPU::BIC(Pins* pins) {
    this->jump_if_flag(pins, this->flags.C);
}

void CPU::CPU::BNC(Pins* pins) {
    this->jump_not_flag(pins, this->flags.C);
}

void CPU::CPU::BIO(Pins* pins) {
    this->jump_if_flag(pins, this->flags.O);
}

void CPU::CPU::BNO(Pins* pins) {
    this->jump_not_flag(pins, this->flags.O);
}

void CPU::CPU::BIL(Pins* pins) {
    this->jump_if_flag(pins, this->flags.L);
}

void CPU::CPU::BNL(Pins* pins) {
    this->jump_not_flag(pins, this->flags.L);
}

void CPU::CPU::BIG(Pins* pins) {
    this->jump_if_flag(pins, this->flags.G);
}

void CPU::CPU::BNG(Pins* pins) {
    this->jump_not_flag(pins, this->flags.G);
}

void CPU::CPU::SBL(Pins* pins, AddressingModes addressing_mode) {
    switch(addressing_mode) {
        case AddressingModes::Absolute:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = ReadWrite::Read;
                break;
                case 3:
                    this->flags.C = pins->data & 0x80;
                    
                    pins->data <<= 1;

                    this->flags.Z = pins->data == 0;
                    this->flags.N = pins->data & 0x80;

                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = ReadWrite::Write;
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

void CPU::CPU::SBR(Pins* pins, AddressingModes addressing_mode) {
    switch(addressing_mode) {
        case AddressingModes::Absolute:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = ReadWrite::Read;
                break;
                case 3:
                    this->flags.C = pins->data & 0x01;
                    
                    pins->data >>= 1;

                    this->flags.Z = pins->data == 0;
                    this->flags.N = pins->data & 0x80;

                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = ReadWrite::Write;
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

void CPU::CPU::ROL(Pins* pins, AddressingModes addressing_mode) {
    switch(addressing_mode) {
        case AddressingModes::Absolute:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = ReadWrite::Read;
                break;
                case 3:
                    this->flags.C = pins->data & 0x80;
                    
                    pins->data = (pins->data << 1) | (pins->data >> 7);

                    this->flags.Z = pins->data == 0;
                    this->flags.N = pins->data & 0x80;

                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = ReadWrite::Write;
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

void CPU::CPU::ROR(Pins* pins, AddressingModes addressing_mode) {
    switch(addressing_mode) {
        case AddressingModes::Absolute:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = ReadWrite::Read;
                break;
                case 3:
                    this->flags.C = pins->data & 0x01;
                    
                    pins->data = (pins->data >> 1) | (pins->data << 7);

                    this->flags.Z = pins->data == 0;
                    this->flags.N = pins->data & 0x80;

                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = ReadWrite::Write;
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

void CPU::CPU::CLC(Pins* pins) {
    this->flags.C = 0;
    this->finish(pins);
}

void CPU::CPU::INC(Pins* pins, AddressingModes addressing_mode) {
    switch(addressing_mode) {
        case AddressingModes::Absolute:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = ReadWrite::Read;
                break;
                case 3:
                    this->flags.Z = ++pins->data == 0;
                    this->flags.N = pins->data & 0x80;

                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = ReadWrite::Write;
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

void CPU::CPU::DEC(Pins* pins, AddressingModes addressing_mode) {
    switch(addressing_mode) {
        case AddressingModes::Absolute:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = ReadWrite::Read;
                break;
                case 3:
                    this->flags.Z = --pins->data == 0;
                    this->flags.N = pins->data & 0x80;

                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = ReadWrite::Write;
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

void CPU::CPU::HLT(Pins* pins) {
    this->state = State::Halt;
    this->finish(pins);
}

void CPU::CPU::CMP(Pins* pins, AddressingModes addressing_modes) {
    switch(addressing_modes) {
        case AddressingModes::Immediate:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = ReadWrite::Read;
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
                    pins->rw = ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = ReadWrite::Read;
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

void CPU::CPU::LDO(Pins* pins) {
    switch (this->cycleCount) {
        case 0:
            pins->address = this->pc;
            pins->rw = ReadWrite::Read;
        break;
        case 1:
            this->temp16 = pins->data << 8;
            pins->address.address = ++this->pc.address;
            pins->rw = ReadWrite::Read;
        break;
        case 2:
            this->temp16 |= pins->data;
            pins->address.value = this->temp16;
            pins->address.extended = this->metadata.ext_addr;
            pins->address.address += this->ro;
            pins->rw = ReadWrite::Read;
        break;
        case 3:
            this->DecodeRegister(this->metadata.reg0) = pins->data;
            this->pc.address++;
            this->finish(pins);
        break;
    }
}

void CPU::CPU::NOP(Pins* pins) {
    this->finish(pins);
}

void CPU::CPU::PSH(Pins* pins, AddressingModes addressing_mode) {
    switch (addressing_mode) {
        case AddressingModes::Immediate:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = ReadWrite::Read;
                break;
                case 1:
                    pins->address.address = this->sp.address;
                    pins->rw = ReadWrite::Write;
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
                    pins->rw = ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = ReadWrite::Read;
                break;
                case 3:
                    pins->address.address = this->sp.address;
                    pins->rw = ReadWrite::Write;
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
                    pins->rw = ReadWrite::Write;
                break;
                case 1:
                    this->sp.value++;
                    this->pc.address++;
                    this->finish(pins);
                break;
            }
        break;
    }
}

void CPU::CPU::POP(Pins* pins, AddressingModes addressing_mode) {
    switch(addressing_mode) {
        case AddressingModes::Absolute:
            switch(this->cycleCount) {
                case 0:
                    pins->address = this->pc;
                    pins->rw = ReadWrite::Read;
                break;
                case 1:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++this->pc.address;
                    pins->rw = ReadWrite::Read;
                break;
                case 2:
                    this->temp16 |= pins->data;
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = ReadWrite::Read;
                break;
                case 3:
                    pins->address.address = (--this->sp.address) | 0xFC00;
                    pins->rw = ReadWrite::Read;
                break;
                case 4:
                    pins->address.value = this->temp16;
                    pins->address.extended = this->metadata.ext_addr;
                    pins->rw = ReadWrite::Write;
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
                    pins->rw = ReadWrite::Read;
                break;
                case 1:
                    this->DecodeRegister(this->metadata.reg0) = pins->data;

                    this->finish(pins);
                break;
            }
        break;
    }
}

void CPU::CPU::JSR(Pins* pins) {
    switch (this->cycleCount) {
        case 0:
            pins->address = this->pc;
            pins->rw = ReadWrite::Read;
        break;
        case 1:
            this->temp16 = pins->data << 8;
            pins->address.address = ++this->pc.address;
            pins->rw = ReadWrite::Read;
        break;
        case 2:
            this->temp16 |= pins->data;
            pins->address.address = this->sp.address;
            pins->data = ++this->pc.address >> 8;
            pins->rw = ReadWrite::Write;
        break;
        case 3:
            this->sp.value++;
            pins->address.address = this->sp.address;
            pins->data = this->pc.value & 0xFF;
            pins->rw = ReadWrite::Write;
        break;
        case 4:
            this->sp.value++;
            pins->address.address = this->sp.address;
            pins->data = this->pc.extended;
            pins->rw = ReadWrite::Write;
        break;
        case 5:
            this->sp.value++;
            this->pc.value = this->temp16;
            this->pc.extended = this->metadata.ext_addr;
            this->finish(pins);
        break;
    }
}

void CPU::CPU::RET(Pins* pins) {
    switch(this->cycleCount) {
        case 0:
            pins->address.address = (--this->sp.address) | 0xFC00;
            pins->rw = ReadWrite::Read;
        break;
        case 1:
            this->temp8 = pins->data;
            pins->address.address = (--this->sp.address) | 0xFC00;
            pins->rw = ReadWrite::Read;
        break;
        case 2:
            this->temp16 = pins->data;
            pins->address.address = (--this->sp.address) | 0xFC00;
            pins->rw = ReadWrite::Read;
        break;
        case 3:
            this->temp16 |= pins->data << 8;
            this->pc.value = this->temp16;
            this->pc.extended = this->temp8;

            this->finish(pins);
        break;
    }
}

void CPU::CPU::BII(Pins* pins) {
    this->jump_if_flag(pins, this->flags.I);
}

void CPU::CPU::BNI(Pins* pins) {
    this->jump_not_flag(pins, this->flags.I);
}

void CPU::CPU::CLI(Pins* pins) {
    this->flags.I = 0;
    this->finish(pins);
}

void CPU::CPU::SEI(Pins* pins) {
    this->flags.I = 1;
    this->finish(pins);
}
