#include <8do/cpu.hpp>

namespace EightDo {
	constexpr float HERTZ = 100000;

	class Emulator {
		CPU* cpu;
		CPU::Pins pins;

		public:
			union Options {
				struct {
					uint8_t break_on_hlt : 1;
					uint8_t unused : 7;
				};

				uint8_t data;

				Options() { this->data = 1; }
				Options empty() { this->data = 0; }
			};

			uint8_t* ram = new uint8_t[0x3FFFF];

			Emulator() { this->cpu = new EightDo::CPU(&this->pins); }

			void start() {
				while(1) {
					if (this->opts.break_on_hlt == 1 && this->cpu->get_state() == CPU::State::Halt) {
						break;
					}

					this->loop();
				}
			}

			void set_options(Options options) { this->opts = options; };

		private:
			Options opts = {};

			void loop() {
				this->cpu->cycle(&this->pins);

				if (this->pins.bus_enable) {
					if (this->pins.rw == CPU::ReadWrite::Read) {
						this->pins.data = this->ram[this->pins.address.address];
					}
					else {
						this->ram[this->pins.address.address] = this->pins.data;

						if (this->pins.address.address == 0xA000) {
							if (this->ram[0xA001] == 0x00) {
								std::cout << this->pins.data;
							} else if (this->ram[0xA001] == 0x01) {
								std::cout << std::to_string((uint16_t)this->pins.data);
							} 
						}
					}
				}
			}
	};
}