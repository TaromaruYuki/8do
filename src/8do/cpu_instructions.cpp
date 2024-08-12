#include <8do/cpu.hpp>
#include <limits>

void EightDo::CPU::LDR(Pins* pins, AddressingModes addressing_mode) {
	switch(addressing_mode) {
		case AddressingModes::Immediate:
			switch(this->cycleCount) {
				case 0:
					pins->address = this->pc;
					pins->rw = ReadWrite::Read;
				break;
				case 1:
					this->DecodeRegister(this->metadata.reg0) = pins->data;
					this->pc++;
					this->state = State::Fetch;
					this->finish();
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
					pins->address = ++this->pc;
					pins->rw = ReadWrite::Read;
				break;
				case 2:
					this->temp16 |= pins->data;
					pins->address = this->temp16;
					pins->rw = ReadWrite::Read;
				break;
				case 3:
					this->DecodeRegister(this->metadata.reg0) = pins->data;
					this->pc++;
					this->state = State::Fetch;
					this->finish();
				break;
			}
		break;
		case AddressingModes::Register:
			switch(this->cycleCount) {
				case 0:
					this->DecodeRegister(this->metadata.reg0) = this->DecodeRegister(this->metadata.reg1);
					this->state = State::Fetch;
					this->finish();
				break;
			}
		break;
	}
}

void EightDo::CPU::STR(Pins* pins, AddressingModes addressing_mode) {
	switch(addressing_mode) {
		case AddressingModes::Absolute:
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
					pins->address = this->temp16;
					pins->data = this->DecodeRegister(this->metadata.reg0);
					pins->rw = ReadWrite::Write;
				break;
				case 3:
					this->pc++;
					this->state = State::Fetch;
					this->finish();
				break;
			}
		break;
	}
}

void EightDo::CPU::ADD(Pins* pins, AddressingModes addressing_mode) {
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

					this->pc++;
					this->state = State::Fetch;
					this->finish();
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
					pins->address = ++this->pc;
					pins->rw = ReadWrite::Read;
				break;
				case 2:
					this->temp16 |= pins->data;
					pins->address = this->temp16;
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
					
					this->pc++;
					this->state = State::Fetch;
					this->finish();
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
					
					this->state = State::Fetch;
					this->finish();
				break;
			}
		break;
	}
}

void EightDo::CPU::SUB(Pins* pins, AddressingModes addressing_mode) {
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

					this->pc++;
					this->state = State::Fetch;
					this->finish();
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
					pins->address = ++this->pc;
					pins->rw = ReadWrite::Read;
				break;
				case 2:
					this->temp16 |= pins->data;
					pins->address = this->temp16;
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
					
					this->pc++;
					this->state = State::Fetch;
					this->finish();
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
					
					this->state = State::Fetch;
					this->finish();
				break;
			}
		break;
	}
}

void EightDo::CPU::JMP(Pins* pins) {
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
			this->pc = this->temp16;
			this->state = State::Fetch;
			this->finish();
		break;
	}
}

void EightDo::CPU::BIZ(Pins* pins) {
	this->jump_if_flag(pins, this->flags.Z);
}

void EightDo::CPU::BNZ(Pins* pins) {
	this->jump_not_flag(pins, this->flags.Z);
}

void EightDo::CPU::BIN(Pins* pins) {
	this->jump_if_flag(pins, this->flags.N);
}

void EightDo::CPU::BNN(Pins* pins) {
	this->jump_not_flag(pins, this->flags.N);
}

void EightDo::CPU::BIC(Pins* pins) {
	this->jump_if_flag(pins, this->flags.C);
}

void EightDo::CPU::BNC(Pins* pins) {
	this->jump_not_flag(pins, this->flags.C);
}

void EightDo::CPU::BIO(Pins* pins) {
	this->jump_if_flag(pins, this->flags.O);
}

void EightDo::CPU::BNO(Pins* pins) {
	this->jump_not_flag(pins, this->flags.O);
}

void EightDo::CPU::BIL(Pins* pins) {
	this->jump_if_flag(pins, this->flags.L);
}

void EightDo::CPU::BNL(Pins* pins) {
	this->jump_not_flag(pins, this->flags.L);
}

void EightDo::CPU::BIG(Pins* pins) {
	this->jump_if_flag(pins, this->flags.G);
}

void EightDo::CPU::BNG(Pins* pins) {
	this->jump_not_flag(pins, this->flags.G);
}

void EightDo::CPU::SBL(Pins* pins, AddressingModes addressing_mode) {
	switch(addressing_mode) {
		case AddressingModes::Absolute:
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
					pins->address = this->temp16;
					pins->rw = ReadWrite::Read;
				break;
				case 3:
					this->flags.C = pins->data & 0x80;
					
					pins->data <<= 1;

					this->flags.Z = pins->data == 0;
					this->flags.N = pins->data & 0x80;

					pins->address = this->temp16;
					pins->rw = ReadWrite::Write;
				break;
				case 4:
					this->pc++;
					this->state = State::Fetch;
					this->finish();
				break;
			}
		break;
		case AddressingModes::Register:
			uint8_t& reg = this->DecodeRegister(this->metadata.reg0);
			this->flags.C = reg & 0x80;

			reg <<= 1;

			this->flags.Z = reg == 0;
			this->flags.N = reg & 0x80;

			this->state = State::Fetch;
			this->finish();
		break;
	}
}

void EightDo::CPU::SBR(Pins* pins, AddressingModes addressing_mode) {
	switch(addressing_mode) {
		case AddressingModes::Absolute:
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
					pins->address = this->temp16;
					pins->rw = ReadWrite::Read;
				break;
				case 3:
					this->flags.C = pins->data & 0x01;
					
					pins->data >>= 1;

					this->flags.Z = pins->data == 0;
					this->flags.N = pins->data & 0x80;

					pins->address = this->temp16;
					pins->rw = ReadWrite::Write;
				break;
				case 4:
					this->pc++;
					this->state = State::Fetch;
					this->finish();
				break;
			}
		break;
		case AddressingModes::Register:
			uint8_t& reg = this->DecodeRegister(this->metadata.reg0);
			this->flags.C = reg & 0x01;

			reg >>= 1;

			this->flags.Z = reg == 0;
			this->flags.N = reg & 0x80;

			this->state = State::Fetch;
			this->finish();
		break;
	}
}

void EightDo::CPU::ROL(Pins* pins, AddressingModes addressing_mode) {
	switch(addressing_mode) {
		case AddressingModes::Absolute:
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
					pins->address = this->temp16;
					pins->rw = ReadWrite::Read;
				break;
				case 3:
					this->flags.C = pins->data & 0x80;
					
					pins->data = (pins->data << 1) | (pins->data >> 7);

					this->flags.Z = pins->data == 0;
					this->flags.N = pins->data & 0x80;

					pins->address = this->temp16;
					pins->rw = ReadWrite::Write;
				break;
				case 4:
					this->pc++;
					this->state = State::Fetch;
					this->finish();
				break;
			}
		break;
		case AddressingModes::Register:
			uint8_t& reg = this->DecodeRegister(this->metadata.reg0);
			this->flags.C = reg & 0x80;

			reg = (reg << 1) | (reg >> 7);

			this->flags.Z = reg == 0;
			this->flags.N = reg & 0x80;

			this->state = State::Fetch;
			this->finish();
		break;
	}
}

void EightDo::CPU::ROR(Pins* pins, AddressingModes addressing_mode) {
	switch(addressing_mode) {
		case AddressingModes::Absolute:
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
					pins->address = this->temp16;
					pins->rw = ReadWrite::Read;
				break;
				case 3:
					this->flags.C = pins->data & 0x01;
					
					pins->data = (pins->data >> 1) | (pins->data << 7);

					this->flags.Z = pins->data == 0;
					this->flags.N = pins->data & 0x80;

					pins->address = this->temp16;
					pins->rw = ReadWrite::Write;
				break;
				case 4:
					this->pc++;
					this->state = State::Fetch;
					this->finish();
				break;
			}
		break;
		case AddressingModes::Register:
			uint8_t& reg = this->DecodeRegister(this->metadata.reg0);
			this->flags.C = reg & 0x01;

			reg = (reg >> 1) | (reg << 7);

			this->flags.Z = reg == 0;
			this->flags.N = reg & 0x80;

			this->state = State::Fetch;
			this->finish();
		break;
	}
}

void EightDo::CPU::CLC() {
	this->flags.C = 0;
	this->state = State::Fetch;
	this->finish();
}

void EightDo::CPU::INC(Pins* pins, AddressingModes addressing_mode) {
	switch(addressing_mode) {
		case AddressingModes::Absolute:
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
					pins->address = this->temp16;
					pins->rw = ReadWrite::Read;
				break;
				case 3:
					this->flags.Z = ++pins->data == 0;
					this->flags.N = pins->data & 0x80;

					pins->address = this->temp16;
					pins->rw = ReadWrite::Write;
				break;
				case 4:
					this->pc++;
					this->state = State::Fetch;
					this->finish();
				break;
			}
		break;
		case AddressingModes::Register:
			uint8_t& reg = this->DecodeRegister(this->metadata.reg0);

			this->flags.Z = ++reg == 0;
			this->flags.N = reg & 0x80;

			this->state = State::Fetch;
			this->finish();
		break;
	}
}

void EightDo::CPU::DEC(Pins* pins, AddressingModes addressing_mode) {
	switch(addressing_mode) {
		case AddressingModes::Absolute:
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
					pins->address = this->temp16;
					pins->rw = ReadWrite::Read;
				break;
				case 3:
					this->flags.Z = --pins->data == 0;
					this->flags.N = pins->data & 0x80;

					pins->address = this->temp16;
					pins->rw = ReadWrite::Write;
				break;
				case 4:
					this->pc++;
					this->state = State::Fetch;
					this->finish();
				break;
			}
		break;
		case AddressingModes::Register:
			uint8_t& reg = this->DecodeRegister(this->metadata.reg0);

			this->flags.Z = --reg == 0;
			this->flags.N = reg & 0x80;

			this->state = State::Fetch;
			this->finish();
		break;
	}
}

void EightDo::CPU::HLT() {
	this->state = State::Halt;
	this->finish();
}

void EightDo::CPU::CMP(Pins* pins, AddressingModes addressing_modes) {
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

					this->pc++;
					this->state = State::Fetch;
					this->finish();
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
					pins->address = ++this->pc;
					pins->rw = ReadWrite::Read;
				break;
				case 2:
					this->temp16 |= pins->data;
					pins->address = this->temp16;
					pins->rw = ReadWrite::Read;
				break;
				case 3:
					uint8_t& reg = this->DecodeRegister(this->metadata.reg0);
					
					this->flags.Z = reg == pins->data;
					this->flags.N = (reg - pins->data) & 0x80;
					this->flags.G = reg > pins->data;
					this->flags.L = reg < pins->data;

					this->pc++;
					this->state = State::Fetch;
					this->finish();
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

			this->state = State::Fetch;
			this->finish();
		break;
	}
}
