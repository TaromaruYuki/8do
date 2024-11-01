#include <CPU/cpu.hpp>
#include <iostream>

void CPU::CPU::reset(EightDo::Common::Pins* pins) {
    this->ra = 0;
    this->rb = 0;
    this->rc = 0;
    this->rd = 0;
    this->ro = 0;

    this->pc.address = 0;
    this->sp.value = 0;
    this->sp.unused = 0x3F;

    this->flags.value = 0;
    this->metadata.value = 0;

    this->state = EightDo::Common::State::Reset;
    this->cycleCount = 0;

    pins->address.address = 0;
    pins->data = 0;
    pins->bus_enable = true;
    pins->rw = EightDo::Common::ReadWrite::Read;
}

void CPU::CPU::finish(EightDo::Common::Pins* pins) { 
    this->cycleCount = -1;
    pins->bus_enable = false;

    if(this->int_status != EightDo::Common::InterruptStatus::None) {
        this->state = EightDo::Common::State::Interrupt;
        return;
    }

    if (this->state == EightDo::Common::State::Halt) {
        return;
    }

    this->state = EightDo::Common::State::Fetch;
}

void CPU::CPU::cycle(EightDo::Common::Pins* pins) {
    if(pins->irq && this->int_status == EightDo::Common::InterruptStatus::None) {
        this->int_status = EightDo::Common::InterruptStatus::Normal;
	} else if(pins->nmi && this->int_status == EightDo::Common::InterruptStatus::None) {
        this->int_status = EightDo::Common::InterruptStatus::NonMaskable;
    }

    switch(this->state) {
        case EightDo::Common::State::Reset:
            this->reset_state_handler(pins);
            break;
        case EightDo::Common::State::Fetch:
            this->fetch_state_handler(pins);
            break;
        case EightDo::Common::State::Execute:
            this->execute_state_handler(pins);
            break;
        case EightDo::Common::State::Halt:
            return;
        case EightDo::Common::State::Interrupt:
            this->interrupt_state_handler(pins);
            break;
    }

    this->cycleCount++;
}

void CPU::CPU::reset_state_handler(EightDo::Common::Pins* pins) {
    switch(this->cycleCount) {
        case 0:
            pins->address.address = 0x0000;
            pins->rw = EightDo::Common::ReadWrite::Read;
            break;
        case 1:
            this->temp16 = ((uint16_t)pins->data) << 8;

            pins->address.address = 0x0001;
            pins->rw = EightDo::Common::ReadWrite::Read;
            break;
        case 2:
            this->temp16 |= pins->data;
            this->pc.value = this->temp16;

            this->state = EightDo::Common::State::Fetch;
            this->finish(pins);
            break;
    }
}

void CPU::CPU::fetch_state_handler(EightDo::Common::Pins* pins) {
    switch(this->cycleCount) {
        case 0:
            pins->address = this->pc;
            pins->rw = EightDo::Common::ReadWrite::Read;
            pins->bus_enable = true;
            break;
        case 1:
            this->opcode = pins->data;
            pins->address.address = ++this->pc.address;
            pins->rw = EightDo::Common::ReadWrite::Read;
            break;
        case 2:
            this->metadata.value = pins->data;
            this->pc.address++;

            this->finish(pins);
            this->state = EightDo::Common::State::Execute;
            break;
    }
}

void CPU::CPU::execute_state_handler(EightDo::Common::Pins* pins) {
    pins->bus_enable = true;

    switch(this->opcode) {
        case 0x2B: LDR(pins, AddressingModes::Immediate); break;
        case 0x23: LDR(pins, AddressingModes::Absolute); break;
        case 0x34: LDR(pins, AddressingModes::Register); break;
        case 0x3A: STR(pins, AddressingModes::Absolute); break;
        case 0x12: ADD(pins, AddressingModes::Immediate); break;
        case 0x0A: ADD(pins, AddressingModes::Absolute); break;
        case 0x1B: ADD(pins, AddressingModes::Register); break;
        case 0x33: SUB(pins, AddressingModes::Immediate); break;
        case 0x4B: SUB(pins, AddressingModes::Absolute); break;
        case 0x3C: SUB(pins, AddressingModes::Register); break;
        case 0xE7: JMP(pins, AddressingModes::Absolute); break;
        case 0xB0: BIZ(pins); break;
        case 0xB1: BNZ(pins); break;
        case 0xB2: BIN(pins); break;
        case 0xB3: BNN(pins); break;
        case 0xB4: BIC(pins); break;
        case 0xB5: BNC(pins); break;
        case 0xB6: BIO(pins); break;
        case 0xB7: BNO(pins); break;
        case 0xB8: BIL(pins); break;
        case 0xB9: BNL(pins); break;
        case 0xBA: BIG(pins); break;
        case 0xBB: BNG(pins); break;
        case 0x22: SBL(pins, AddressingModes::Absolute); break;
        case 0x01: SBL(pins, AddressingModes::Register); break;
        case 0x28: SBR(pins, AddressingModes::Absolute); break;
        case 0x39: SBR(pins, AddressingModes::Register); break;
        case 0x2e: ROL(pins, AddressingModes::Absolute); break;
        case 0x3F: ROL(pins, AddressingModes::Register); break;
        case 0x54: ROR(pins, AddressingModes::Absolute); break;
        case 0x45: ROR(pins, AddressingModes::Register); break;
        case 0xD2: CLC(pins); break;
        case 0x3B: INC(pins, AddressingModes::Absolute); break;
        case 0x2C: INC(pins, AddressingModes::Register); break;
        case 0x0D: DEC(pins, AddressingModes::Absolute); break;
        case 0x1E: DEC(pins, AddressingModes::Register); break;
        case 0xE8: HLT(pins); break;
        case 0x29: CMP(pins, AddressingModes::Immediate); break;
        case 0x21: CMP(pins, AddressingModes::Absolute); break;
        case 0x32: CMP(pins, AddressingModes::Register); break;
        case 0x20: LDO(pins, AddressingModes::Absolute); break;
        case 0x31: LDO(pins, AddressingModes::Register); break;
        case 0x00: NOP(pins); break;
        case 0x74: PSH(pins, AddressingModes::Immediate); break;
        case 0x4C: PSH(pins, AddressingModes::Absolute); break;
        case 0x3D: PSH(pins, AddressingModes::Register); break;
        case 0x30: POP(pins, AddressingModes::Absolute); break;
        case 0x41: POP(pins, AddressingModes::Register); break;
        case 0xEF: JSR(pins); break;
        case 0xEB: RET(pins); break;
        case 0x52: LDR(pins, AddressingModes::Pointer); break;
        case 0x49: STR(pins, AddressingModes::Pointer); break;
        case 0x37: JMP(pins, AddressingModes::Pointer); break;
        case 0xD4: BII(pins); break;
        case 0xD9: BNI(pins); break;
        case 0xD8: CLI(pins); break;
        case 0xE1: SEI(pins); break;
        case 0x0F: RTI(pins); break;
        case 0x2F: LDO(pins, AddressingModes::Pointer); break;
        default: {
            std::cerr << "Unknown opcode: 0x" << std::hex << (uint16_t)this->opcode << std::endl << "PC: 0x" << std::hex << this->pc.address - 2 << std::endl;

            std::exit(1);
        }
    }
}

void CPU::CPU::interrupt_state_handler(EightDo::Common::Pins* pins) {
    switch (this->int_status) {
        case EightDo::Common::InterruptStatus::Normal:
            switch(this->cycleCount) {
                case 0:
                    pins->iak = 1;
                    break;
                case 1:
                    pins->bus_enable = true;
                    pins->iak = 0;
                    this->int_number = pins->data;
                    pins->address = this->int_number.value * 2;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 2:
                    this->temp16 = pins->data << 8;
                    pins->address.address = ++pins->address.address;
                    pins->rw = EightDo::Common::ReadWrite::Read;
                break;
                case 3:
                    this->temp16 |= pins->data;
                    pins->address.address = this->sp.address;
                    pins->data = this->pc.value >> 8;
                    pins->rw = EightDo::Common::ReadWrite::Write;
                break;
                case 4:
                    pins->address.address = ++this->sp.address;
                    pins->data = this->pc.value;
                    pins->rw = EightDo::Common::ReadWrite::Write;
                break;
                case 5:
                    pins->address.address = ++this->sp.address;
                    pins->data = this->pc.extended;
                    pins->rw = EightDo::Common::ReadWrite::Write;
                break;
                case 6:
                    pins->address.address = ++this->sp.address;
                    pins->data = this->flags.value;
                    pins->rw = EightDo::Common::ReadWrite::Write;
                break;
                case 7:
                    this->pc.address = this->temp16;
                    this->int_status = EightDo::Common::InterruptStatus::None;
                    this->finish(pins);
                break;
            }
        break;
        case EightDo::Common::InterruptStatus::NonMaskable:
            switch (this->cycleCount) {
            case 0:
                pins->iak = 1;
                break;
            case 1:
                pins->bus_enable = true;
                pins->iak = 0;
                pins->address = 0x02;
                pins->rw = EightDo::Common::ReadWrite::Read;
                break;
            case 2:
                this->temp16 = pins->data << 8;
                pins->address.address = ++pins->address.address;
                pins->rw = EightDo::Common::ReadWrite::Read;
                break;
            case 3:
                this->temp16 |= pins->data;
                pins->address.address = this->sp.address;
                pins->data = this->pc.value >> 8;
                pins->rw = EightDo::Common::ReadWrite::Write;
                break;
            case 4:
                pins->address.address = ++this->sp.address;
                pins->data = this->pc.value;
                pins->rw = EightDo::Common::ReadWrite::Write;
                break;
            case 5:
                pins->address.address = ++this->sp.address;
                pins->data = this->pc.extended;
                pins->rw = EightDo::Common::ReadWrite::Write;
                break;
            case 6:
                pins->address.address = ++this->sp.address;
                pins->data = this->flags.value;
                pins->rw = EightDo::Common::ReadWrite::Write;
                break;
            case 7:
                this->pc.address = this->temp16;
                this->int_status = EightDo::Common::InterruptStatus::None;
                this->finish(pins);
                break;
            }
            break;
    }
}

uint8_t& CPU::CPU::DecodeRegister(uint8_t reg) {
    switch(reg) {
        case 0x0:
            return this->ra;
            break;
        case 0x1:
            return this->rb;
            break;
        case 0x2:
            return this->rc;
            break;
        case 0x3:
            return this->rd;
            break;
        case 0x4:
            return this->ro;
            break;
        case 0x5:
            return this->ra;
            break;
        case 0x6:
            return this->rb;
            break;
        case 0x7:
            return this->rc;
            break;
        default:
            exit(1);
            break;
    }
}

void CPU::CPU::jump_if_flag(EightDo::Common::Pins* pins, bool flag) {
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
            if(flag) {
                this->pc.value = this->temp16;
                this->pc.extended = this->metadata.ext_addr;
            }
            else {
                this->pc.address++;
            }

            this->state = EightDo::Common::State::Fetch;
            this->finish(pins);
        break;
    }
}

void CPU::CPU::jump_not_flag(EightDo::Common::Pins* pins, bool flag) {
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
            if(!flag) {
                this->pc.value = this->temp16;
                this->pc.extended = this->metadata.ext_addr;
            } else {
                this->pc.address++;
            }

            this->state = EightDo::Common::State::Fetch;
            this->finish(pins);
        break;
    }
}