#include <inttypes.h>

namespace EightDo {
	class CPU {
	public:
		enum struct ReadWrite {
			Read,
			Write
		};

		struct MapperValue {
			uint8_t value : 2;
		};

		struct Pins {
			uint16_t address = 0x0000;
			uint8_t data = 0x00;
			ReadWrite rw = ReadWrite::Read;
			MapperValue mapper;
		};

	private:
		enum struct State {
			Reset,
			Fetch,
			Execute,
			Halt
		};

		enum struct AddressingModes {
			Immediate,
			Absolute,
			Register
		};

		union {
			struct {
				uint8_t reg0     : 3;
				uint8_t reg1     : 3;
				uint8_t ext_addr : 2;
			};
			uint8_t value;
		} metadata;                    // Current Instruction Metadata

		union {
			struct {
				uint8_t Z : 1;         // Zero
				uint8_t N : 1;         // Negative
				uint8_t C : 1;         // Carry
				uint8_t O : 1;         // Overflow
				uint8_t L : 1;         // Less Than
				uint8_t G : 1;         // Greater Than
				uint8_t _ : 2;		   // Unused
			};
			uint8_t value;
		} flags;				       // Flags Register
						          
		uint8_t ra = 0;                // A      Register
		uint8_t rb = 0;                // B      Register
		uint8_t rc = 0;                // C      Register
		uint8_t rd = 0;                // D      Register
		uint8_t ro = 0;                // Offset Register
						          	   
		uint16_t pc = 0;               // Program Counter
		uint8_t sp = 0;                // Stack Pointer
		State state;                   // Current State
		int8_t cycleCount = 0;         // Current Cycle
						          	   
		uint16_t temp16 = 0;           // Temporary 16-bit value
		uint8_t temp8 = 0;	           // Temporary 8-bit value
		uint8_t opcode = 0;	           // Current Opcode
		
	public:
		CPU() { this->reset(); };
		void reset();
		void cycle(Pins* pins);

	private:
		void finish() { this->cycleCount = -1; };
		uint8_t& DecodeRegister(uint8_t reg);

		void reset_state_handler(Pins* pins);
		void fetch_state_handler(Pins* pins);
		void execute_state_handler(Pins* pins);

		void jump_if_flag(Pins* pins, bool flag);
		void jump_not_flag(Pins* pins, bool flag);

		void LDR(Pins* pins, AddressingModes addressing_mode);
		void STR(Pins* pins, AddressingModes addressing_mode);
		void ADD(Pins* pins, AddressingModes addressing_mode);
		void SUB(Pins* pins, AddressingModes addressing_mode);
		void JMP(Pins* pins);
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
		void CLC();
		void INC(Pins* pins, AddressingModes addressing_mode);
		void DEC(Pins* pins, AddressingModes addressing_mode);
		void HLT();
		void CMP(Pins* pins, AddressingModes addressing_mode);
	};
}