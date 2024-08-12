#include <8do/cpu.hpp>

namespace EightDo {
	constexpr float HERTZ = 100000;

	class Emulator {
		CPU cpu;
		CPU::Pins pins;

		public:
			uint8_t* ram = new uint8_t[0xFFFF];

			void start() {
				while(1) {
					this->loop();
				}
			}

		private:
			void loop() {
				this->cpu.cycle(&this->pins);

				if(this->pins.rw == CPU::ReadWrite::Read) {
					this->pins.data = this->ram[this->pins.address];
				} else {
					this->ram[this->pins.address] = this->pins.data;
				}
			}
	};
}