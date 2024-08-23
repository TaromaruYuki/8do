#include <CPU/lib.hpp>

namespace EightDo {
	constexpr float HERTZ = 100000;

	class Emulator {
		CPU::CPU* cpu;
		CPU::CPU::Pins pins;

		public:
			union Options {
				struct {
					uint8_t break_on_hlt : 1;
					uint8_t verbose_debug : 1;
					uint8_t unused : 6;
				};

				uint8_t data;

				Options() { this->data = 1; }
				static Options empty() { Options opts; opts.data = 0; return opts; }
			};

			uint8_t* ram_ptr;
			CPU::CommonDevices::RAM ram = CPU::CommonDevices::RAM({ .address = 0x00000 }, { .address = 0x3FFFF }, &ram_ptr);

			Emulator() { this->cpu = new CPU::CPU(&this->pins); }

			void start() {
				while(1) {
					if (this->opts.break_on_hlt == 1 && this->cpu->get_state() == CPU::CPU::State::Halt) {
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
					if (this->pins.rw == CPU::CPU::ReadWrite::Read) {
						this->pins.data = this->ram.read(this->pins.address).value;
					}
					else {
						this->ram.write(this->pins.address, this->pins.data);

						if (this->pins.address.address == 0xA000) { // TEMP: Basic printing mainly for debugging
							if (this->ram_ptr[0xA001] == 0x00) {
								std::cout << this->pins.data;
							} else if (this->ram_ptr[0xA001] == 0x01) {
								std::cout << std::to_string((uint16_t)this->pins.data);
							} 
						}
					}
				}
			}
	};
}