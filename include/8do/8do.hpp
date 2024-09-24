#include <CPU/cpu.hpp>
#include <8do/bus.hpp>
#include <8do/devices.hpp>

#include <raylib-cpp.hpp>
#include <rlImGui/rlImGui.h>
#include <imgui.h>

namespace EightDo {
    constexpr float HERTZ = 100000;

    class Emulator : public Bus {
        CPU::CPU* cpu;
        EightDo::Common::Pins pins;
        raylib::Window* window;
        bool open = true;

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

            Emulator() { 
                this->cpu = new CPU::CPU(&this->pins);
                this->window = new raylib::Window(80 * 8 * 2, 25 * 14 * 2); // 80×25 - 8×14
            }

            ~Emulator() {
                delete this->cpu;
                rlImGuiShutdown();
                window->Close();
                delete this->window;
            }

            void start() {
                rlImGuiSetup(true);

                while(!this->window->ShouldClose()) {
                    if (this->opts.break_on_hlt == 1 && this->cpu->get_state() == EightDo::Common::State::Halt) {
                        break;
                    }

                    this->loop();

                    BeginDrawing();

                    window->ClearBackground(RAYWHITE);
                    rlImGuiBegin();
                    ImGui::ShowDemoWindow(&open);
                    DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

                    DrawFPS(190, 300);
                    rlImGuiEnd();
                    EndDrawing();
                }
            }

            void set_options(Options options) { this->opts = options; };

        private:
            Options opts = {};

            void loop() {
                this->cpu->cycle(&this->pins);

                if (this->pins.bus_enable) {
                    if (this->pins.rw == EightDo::Common::ReadWrite::Read) {
                        EightDo::Device::Result result = this->read_write(this->pins.address);

                        if(result.status == EightDo::Device::Result::Status::Ok) {
                            this->pins.data = result.value;
                            return;
                        }

                        if (result.status == EightDo::Device::Result::Status::WriteOnly) {
                            this->pins.data = 0;
                            return;
                        }

                        std::cerr << "ERROR: " << result.status << std::endl;

                        exit(result.value);
                    }
                    else {
                        EightDo::Device::Result result = this->read_write(this->pins.address, this->pins.data);

                        if (result.status == EightDo::Device::Result::Status::Ok) {
                            if (this->pins.address.address == 0xA000)
                                std::cout << this->pins.data;

                            return;
                        }

                        if(result.status == EightDo::Device::Result::Status::ReadOnly) {
                            return;
                        }
                        
                        std::cerr << "ERROR: " << result.status << std::endl;

                        exit(result.value);
                    }
                }
            }
    };
}