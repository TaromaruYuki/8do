## Force test instruction

```cpp
#include <8do/8do.hpp>

int main() {
	EightDo::CPU::Pins pins;
	EightDo::CPU cpu;

	cpu.reset();

	cpu.cycle(&pins); // |
	cpu.cycle(&pins); // | Ignoring the restart state
	cpu.cycle(&pins); // |

	cpu.cycle(&pins); // Asking for the opcode
	pins.data = 0x01; // Sending opcode (LD Abs)

	cpu.cycle(&pins); // Receiving the opcode, asking for the metadata
	pins.data = 0b00000001; // Sending metadata (reg0 - 0b001 | RB)

	cpu.cycle(&pins); // Receiving the metadata, switching to the execute state

	cpu.cycle(&pins); // Asking for upper byte of the address
	pins.data = 0xAB; // Sending upper byte of the address

	cpu.cycle(&pins); // Receiving the upper byte of the address, asking for the lower byte of the address
	pins.data = 0xCD; // Sending lower byte of the address

	cpu.cycle(&pins); // Receiving the lower byte of the address, asking for the data at the address
	pins.data = 0xEF; // Sending the data at the address

	cpu.cycle(&pins); // Receiving the data at the address, storing it in the register, and switching to the fetch state

	return 0;
}
```