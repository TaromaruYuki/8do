#include <8do/cpu.hpp>
#include <iostream>

void EightDo::CPU::reset() {
	this->ra = 0;
	this->rb = 0;
	this->rc = 0;
	this->rd = 0;
	this->ro = 0;

	this->pc = 0;
	this->sp = 0;

	this->flags.value = 0;
	this->metadata.value = 0;

	this->state = State::Reset;
	this->cycleCount = 0;
}

void EightDo::CPU::cycle(Pins* pins) {
	switch(this->state) {
		case State::Reset:
			this->reset_state_handler(pins);
			break;
		case State::Fetch:
			this->fetch_state_handler(pins);
			break;
		case State::Execute:
			this->execute_state_handler(pins);
			break;
		case State::Halt:
			return;
	}

	this->cycleCount++;
}

void EightDo::CPU::reset_state_handler(Pins* pins) {
	switch(this->cycleCount) {
		case 0:
			pins->address = 0x0000;
			pins->rw = ReadWrite::Read;
			break;
		case 1:
			this->temp16 = ((uint16_t)pins->data) << 8;

			pins->address = 0x0001;
			pins->rw = ReadWrite::Read;
			break;
		case 2:
			this->temp16 |= pins->data;
			this->pc = this->temp16;

			this->state = State::Fetch;
			this->finish();
			break;
	}
}

void EightDo::CPU::fetch_state_handler(Pins* pins) {
	switch(this->cycleCount) {
		case 0:
			pins->address = this->pc;
			pins->rw = ReadWrite::Read;
			break;
		case 1:
			this->opcode = pins->data;
			pins->address = ++this->pc;
			pins->rw = ReadWrite::Read;
			break;
		case 2:
			this->metadata.value = pins->data;
			this->pc++;

			this->state = State::Execute;
			this->finish();
			break;
	}
}

void EightDo::CPU::execute_state_handler(Pins* pins) {
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
		case 0xE7: JMP(pins); break;
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
		case 0xD2: CLC(); break;
		case 0x3B: INC(pins, AddressingModes::Absolute); break;
		case 0x2C: INC(pins, AddressingModes::Register); break;
		case 0x0D: DEC(pins, AddressingModes::Absolute); break;
		case 0x1E: DEC(pins, AddressingModes::Register); break;
		case 0xE8: HLT(); break;
		case 0x29: CMP(pins, AddressingModes::Immediate); break;
		case 0x21: CMP(pins, AddressingModes::Absolute); break;
		case 0x32: CMP(pins, AddressingModes::Register); break;
		default: {
			std::cerr << "Unknown opcode: 0x" << std::hex << (uint16_t)this->opcode << std::endl << "PC: 0x" << std::hex << this->pc << std::endl;

			std::exit(1);
		}
	}
}

uint8_t& EightDo::CPU::DecodeRegister(uint8_t reg) {
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
	}
}

void EightDo::CPU::jump_if_flag(Pins* pins, bool flag) {
	switch(this->cycleCount) {
		case 0:
			pins->address = this->pc;
			pins->rw = ReadWrite::Read;
		break;
		case 1:
			this->temp16 = pins->data << 8;
			pins->address = ++this->pc;
			pins->rw = ReadWrite::Read;
		break;
		case 2:
			this->temp16 |= pins->data;
			if(flag) {
				this->pc = this->temp16;
			}

			this->state = State::Fetch;
			this->finish();
		break;
	}
}

void EightDo::CPU::jump_not_flag(Pins* pins, bool flag) {
	switch(this->cycleCount) {
		case 0:
			pins->address = this->pc;
			pins->rw = ReadWrite::Read;
		break;
		case 1:
			this->temp16 = pins->data << 8;
			pins->address = ++this->pc;
			pins->rw = ReadWrite::Read;
		break;
		case 2:
			this->temp16 |= pins->data;
			if(!flag) {
				this->pc = this->temp16;
			} else {
				this->pc++;
			}

			this->state = State::Fetch;
			this->finish();
		break;
	}
}