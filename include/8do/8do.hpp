#include <CPU/lib.hpp>
#include <8do/bus.hpp>

namespace EightDo {
	constexpr float HERTZ = 100000;

	class Emulator : public Bus {
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
						CPU::Device::Result result = this->read_write(this->pins.address);

						if(result.status == CPU::Device::Result::Status::Ok) {
							this->pins.data = result.value;
							return;
						}

						std::cerr << "ERROR: " << result.status << std::endl;

						exit(result.value);
					}
					else {
						CPU::Device::Result result = this->read_write(this->pins.address, this->pins.data);

						if (result.status == CPU::Device::Result::Status::Ok) {
							if (this->pins.address.address == 0xA000)
								std::cout << this->pins.data;

							return;
						}
						
						std::cerr << "ERROR: " << result.status << std::endl;

						exit(result.value);
					}
				}
			}
	};
}