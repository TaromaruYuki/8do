#include <CPU/cpu.hpp>
#include <8do/bus.hpp>
#include <8do/devices.hpp>
#include <8do/debug_menu.hpp>

#include <raylib-cpp.hpp>
#include <rlImGui/rlImGui.h>
#include <imgui.h>

namespace EightDo {
    constexpr float HERTZ = 100000;
    constexpr int SCALE = 1;

    class Emulator : public Bus {
        CPU::CPU* cpu;
        EightDo::Common::Pins pins;
        raylib::Window* window;
        UI::DebugUI* debug_ui;
        EightDo::VideoDevice* video_device;

        uint8_t seabios8x8[2048] = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x7e, 0x81, 0xa5, 0x81, 0xbd, 0x99, 0x81, 0x7e, 
            0x7e, 0xff, 0xdb, 0xff, 0xc3, 0xe7, 0xff, 0x7e, 
            0x6c, 0xfe, 0xfe, 0xfe, 0x7c, 0x38, 0x10, 0x00, 
            0x10, 0x38, 0x7c, 0xfe, 0x7c, 0x38, 0x10, 0x00, 
            0x38, 0x7c, 0x38, 0xfe, 0xfe, 0x7c, 0x38, 0x7c, 
            0x10, 0x10, 0x38, 0x7c, 0xfe, 0x7c, 0x38, 0x7c, 
            0x00, 0x00, 0x18, 0x3c, 0x3c, 0x18, 0x00, 0x00, 
            0xff, 0xff, 0xe7, 0xc3, 0xc3, 0xe7, 0xff, 0xff, 
            0x00, 0x3c, 0x66, 0x42, 0x42, 0x66, 0x3c, 0x00, 
            0xff, 0xc3, 0x99, 0xbd, 0xbd, 0x99, 0xc3, 0xff, 
            0x0f, 0x07, 0x0f, 0x7d, 0xcc, 0xcc, 0xcc, 0x78, 
            0x3c, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x7e, 0x18, 
            0x3f, 0x33, 0x3f, 0x30, 0x30, 0x70, 0xf0, 0xe0, 
            0x7f, 0x63, 0x7f, 0x63, 0x63, 0x67, 0xe6, 0xc0, 
            0x99, 0x5a, 0x3c, 0xe7, 0xe7, 0x3c, 0x5a, 0x99, 
            0x80, 0xe0, 0xf8, 0xfe, 0xf8, 0xe0, 0x80, 0x00, 
            0x02, 0x0e, 0x3e, 0xfe, 0x3e, 0x0e, 0x02, 0x00, 
            0x18, 0x3c, 0x7e, 0x18, 0x18, 0x7e, 0x3c, 0x18, 
            0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x66, 0x00, 
            0x7f, 0xdb, 0xdb, 0x7b, 0x1b, 0x1b, 0x1b, 0x00, 
            0x3e, 0x63, 0x38, 0x6c, 0x6c, 0x38, 0xcc, 0x78, 
            0x00, 0x00, 0x00, 0x00, 0x7e, 0x7e, 0x7e, 0x00, 
            0x18, 0x3c, 0x7e, 0x18, 0x7e, 0x3c, 0x18, 0xff, 
            0x18, 0x3c, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x00, 
            0x18, 0x18, 0x18, 0x18, 0x7e, 0x3c, 0x18, 0x00, 
            0x00, 0x18, 0x0c, 0xfe, 0x0c, 0x18, 0x00, 0x00, 
            0x00, 0x30, 0x60, 0xfe, 0x60, 0x30, 0x00, 0x00, 
            0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xfe, 0x00, 0x00, 
            0x00, 0x24, 0x66, 0xff, 0x66, 0x24, 0x00, 0x00, 
            0x00, 0x18, 0x3c, 0x7e, 0xff, 0xff, 0x00, 0x00, 
            0x00, 0xff, 0xff, 0x7e, 0x3c, 0x18, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x30, 0x78, 0x78, 0x30, 0x30, 0x00, 0x30, 0x00, 
            0x6c, 0x6c, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x6c, 0x6c, 0xfe, 0x6c, 0xfe, 0x6c, 0x6c, 0x00, 
            0x30, 0x7c, 0xc0, 0x78, 0x0c, 0xf8, 0x30, 0x00, 
            0x00, 0xc6, 0xcc, 0x18, 0x30, 0x66, 0xc6, 0x00, 
            0x38, 0x6c, 0x38, 0x76, 0xdc, 0xcc, 0x76, 0x00, 
            0x60, 0x60, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x18, 0x30, 0x60, 0x60, 0x60, 0x30, 0x18, 0x00, 
            0x60, 0x30, 0x18, 0x18, 0x18, 0x30, 0x60, 0x00, 
            0x00, 0x66, 0x3c, 0xff, 0x3c, 0x66, 0x00, 0x00, 
            0x00, 0x30, 0x30, 0xfc, 0x30, 0x30, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x60, 
            0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 
            0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0x80, 0x00, 
            0x7c, 0xc6, 0xce, 0xde, 0xf6, 0xe6, 0x7c, 0x00, 
            0x30, 0x70, 0x30, 0x30, 0x30, 0x30, 0xfc, 0x00, 
            0x78, 0xcc, 0x0c, 0x38, 0x60, 0xcc, 0xfc, 0x00, 
            0x78, 0xcc, 0x0c, 0x38, 0x0c, 0xcc, 0x78, 0x00, 
            0x1c, 0x3c, 0x6c, 0xcc, 0xfe, 0x0c, 0x1e, 0x00, 
            0xfc, 0xc0, 0xf8, 0x0c, 0x0c, 0xcc, 0x78, 0x00, 
            0x38, 0x60, 0xc0, 0xf8, 0xcc, 0xcc, 0x78, 0x00, 
            0xfc, 0xcc, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x00, 
            0x78, 0xcc, 0xcc, 0x78, 0xcc, 0xcc, 0x78, 0x00, 
            0x78, 0xcc, 0xcc, 0x7c, 0x0c, 0x18, 0x70, 0x00, 
            0x00, 0x30, 0x30, 0x00, 0x00, 0x30, 0x30, 0x00, 
            0x00, 0x30, 0x30, 0x00, 0x00, 0x30, 0x30, 0x60, 
            0x18, 0x30, 0x60, 0xc0, 0x60, 0x30, 0x18, 0x00, 
            0x00, 0x00, 0xfc, 0x00, 0x00, 0xfc, 0x00, 0x00, 
            0x60, 0x30, 0x18, 0x0c, 0x18, 0x30, 0x60, 0x00, 
            0x78, 0xcc, 0x0c, 0x18, 0x30, 0x00, 0x30, 0x00, 
            0x7c, 0xc6, 0xde, 0xde, 0xde, 0xc0, 0x78, 0x00, 
            0x30, 0x78, 0xcc, 0xcc, 0xfc, 0xcc, 0xcc, 0x00, 
            0xfc, 0x66, 0x66, 0x7c, 0x66, 0x66, 0xfc, 0x00, 
            0x3c, 0x66, 0xc0, 0xc0, 0xc0, 0x66, 0x3c, 0x00, 
            0xf8, 0x6c, 0x66, 0x66, 0x66, 0x6c, 0xf8, 0x00, 
            0xfe, 0x62, 0x68, 0x78, 0x68, 0x62, 0xfe, 0x00, 
            0xfe, 0x62, 0x68, 0x78, 0x68, 0x60, 0xf0, 0x00, 
            0x3c, 0x66, 0xc0, 0xc0, 0xce, 0x66, 0x3e, 0x00, 
            0xcc, 0xcc, 0xcc, 0xfc, 0xcc, 0xcc, 0xcc, 0x00, 
            0x78, 0x30, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00, 
            0x1e, 0x0c, 0x0c, 0x0c, 0xcc, 0xcc, 0x78, 0x00, 
            0xe6, 0x66, 0x6c, 0x78, 0x6c, 0x66, 0xe6, 0x00, 
            0xf0, 0x60, 0x60, 0x60, 0x62, 0x66, 0xfe, 0x00, 
            0xc6, 0xee, 0xfe, 0xfe, 0xd6, 0xc6, 0xc6, 0x00, 
            0xc6, 0xe6, 0xf6, 0xde, 0xce, 0xc6, 0xc6, 0x00, 
            0x38, 0x6c, 0xc6, 0xc6, 0xc6, 0x6c, 0x38, 0x00, 
            0xfc, 0x66, 0x66, 0x7c, 0x60, 0x60, 0xf0, 0x00, 
            0x78, 0xcc, 0xcc, 0xcc, 0xdc, 0x78, 0x1c, 0x00, 
            0xfc, 0x66, 0x66, 0x7c, 0x6c, 0x66, 0xe6, 0x00, 
            0x78, 0xcc, 0xe0, 0x70, 0x1c, 0xcc, 0x78, 0x00, 
            0xfc, 0xb4, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00, 
            0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xfc, 0x00, 
            0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x78, 0x30, 0x00, 
            0xc6, 0xc6, 0xc6, 0xd6, 0xfe, 0xee, 0xc6, 0x00, 
            0xc6, 0xc6, 0x6c, 0x38, 0x38, 0x6c, 0xc6, 0x00, 
            0xcc, 0xcc, 0xcc, 0x78, 0x30, 0x30, 0x78, 0x00, 
            0xfe, 0xc6, 0x8c, 0x18, 0x32, 0x66, 0xfe, 0x00, 
            0x78, 0x60, 0x60, 0x60, 0x60, 0x60, 0x78, 0x00, 
            0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x02, 0x00, 
            0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78, 0x00, 
            0x10, 0x38, 0x6c, 0xc6, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 
            0x30, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0x76, 0x00, 
            0xe0, 0x60, 0x60, 0x7c, 0x66, 0x66, 0xdc, 0x00, 
            0x00, 0x00, 0x78, 0xcc, 0xc0, 0xcc, 0x78, 0x00, 
            0x1c, 0x0c, 0x0c, 0x7c, 0xcc, 0xcc, 0x76, 0x00, 
            0x00, 0x00, 0x78, 0xcc, 0xfc, 0xc0, 0x78, 0x00, 
            0x38, 0x6c, 0x60, 0xf0, 0x60, 0x60, 0xf0, 0x00, 
            0x00, 0x00, 0x76, 0xcc, 0xcc, 0x7c, 0x0c, 0xf8, 
            0xe0, 0x60, 0x6c, 0x76, 0x66, 0x66, 0xe6, 0x00, 
            0x30, 0x00, 0x70, 0x30, 0x30, 0x30, 0x78, 0x00, 
            0x0c, 0x00, 0x0c, 0x0c, 0x0c, 0xcc, 0xcc, 0x78, 
            0xe0, 0x60, 0x66, 0x6c, 0x78, 0x6c, 0xe6, 0x00, 
            0x70, 0x30, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00, 
            0x00, 0x00, 0xcc, 0xfe, 0xfe, 0xd6, 0xc6, 0x00, 
            0x00, 0x00, 0xf8, 0xcc, 0xcc, 0xcc, 0xcc, 0x00, 
            0x00, 0x00, 0x78, 0xcc, 0xcc, 0xcc, 0x78, 0x00, 
            0x00, 0x00, 0xdc, 0x66, 0x66, 0x7c, 0x60, 0xf0, 
            0x00, 0x00, 0x76, 0xcc, 0xcc, 0x7c, 0x0c, 0x1e, 
            0x00, 0x00, 0xdc, 0x76, 0x66, 0x60, 0xf0, 0x00, 
            0x00, 0x00, 0x7c, 0xc0, 0x78, 0x0c, 0xf8, 0x00, 
            0x10, 0x30, 0x7c, 0x30, 0x30, 0x34, 0x18, 0x00, 
            0x00, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0x76, 0x00, 
            0x00, 0x00, 0xcc, 0xcc, 0xcc, 0x78, 0x30, 0x00, 
            0x00, 0x00, 0xc6, 0xd6, 0xfe, 0xfe, 0x6c, 0x00, 
            0x00, 0x00, 0xc6, 0x6c, 0x38, 0x6c, 0xc6, 0x00, 
            0x00, 0x00, 0xcc, 0xcc, 0xcc, 0x7c, 0x0c, 0xf8, 
            0x00, 0x00, 0xfc, 0x98, 0x30, 0x64, 0xfc, 0x00, 
            0x1c, 0x30, 0x30, 0xe0, 0x30, 0x30, 0x1c, 0x00, 
            0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00, 
            0xe0, 0x30, 0x30, 0x1c, 0x30, 0x30, 0xe0, 0x00, 
            0x76, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x10, 0x38, 0x6c, 0xc6, 0xc6, 0xfe, 0x00, 
            0x78, 0xcc, 0xc0, 0xcc, 0x78, 0x18, 0x0c, 0x78, 
            0x00, 0xcc, 0x00, 0xcc, 0xcc, 0xcc, 0x7e, 0x00, 
            0x1c, 0x00, 0x78, 0xcc, 0xfc, 0xc0, 0x78, 0x00, 
            0x7e, 0xc3, 0x3c, 0x06, 0x3e, 0x66, 0x3f, 0x00, 
            0xcc, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0x7e, 0x00, 
            0xe0, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0x7e, 0x00, 
            0x30, 0x30, 0x78, 0x0c, 0x7c, 0xcc, 0x7e, 0x00, 
            0x00, 0x00, 0x78, 0xc0, 0xc0, 0x78, 0x0c, 0x38, 
            0x7e, 0xc3, 0x3c, 0x66, 0x7e, 0x60, 0x3c, 0x00, 
            0xcc, 0x00, 0x78, 0xcc, 0xfc, 0xc0, 0x78, 0x00, 
            0xe0, 0x00, 0x78, 0xcc, 0xfc, 0xc0, 0x78, 0x00, 
            0xcc, 0x00, 0x70, 0x30, 0x30, 0x30, 0x78, 0x00, 
            0x7c, 0xc6, 0x38, 0x18, 0x18, 0x18, 0x3c, 0x00, 
            0xe0, 0x00, 0x70, 0x30, 0x30, 0x30, 0x78, 0x00, 
            0xc6, 0x38, 0x6c, 0xc6, 0xfe, 0xc6, 0xc6, 0x00, 
            0x30, 0x30, 0x00, 0x78, 0xcc, 0xfc, 0xcc, 0x00, 
            0x1c, 0x00, 0xfc, 0x60, 0x78, 0x60, 0xfc, 0x00, 
            0x00, 0x00, 0x7f, 0x0c, 0x7f, 0xcc, 0x7f, 0x00, 
            0x3e, 0x6c, 0xcc, 0xfe, 0xcc, 0xcc, 0xce, 0x00, 
            0x78, 0xcc, 0x00, 0x78, 0xcc, 0xcc, 0x78, 0x00, 
            0x00, 0xcc, 0x00, 0x78, 0xcc, 0xcc, 0x78, 0x00, 
            0x00, 0xe0, 0x00, 0x78, 0xcc, 0xcc, 0x78, 0x00, 
            0x78, 0xcc, 0x00, 0xcc, 0xcc, 0xcc, 0x7e, 0x00, 
            0x00, 0xe0, 0x00, 0xcc, 0xcc, 0xcc, 0x7e, 0x00, 
            0x00, 0xcc, 0x00, 0xcc, 0xcc, 0x7c, 0x0c, 0xf8, 
            0xc3, 0x18, 0x3c, 0x66, 0x66, 0x3c, 0x18, 0x00, 
            0xcc, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0x78, 0x00, 
            0x18, 0x18, 0x7e, 0xc0, 0xc0, 0x7e, 0x18, 0x18, 
            0x38, 0x6c, 0x64, 0xf0, 0x60, 0xe6, 0xfc, 0x00, 
            0xcc, 0xcc, 0x78, 0xfc, 0x30, 0xfc, 0x30, 0x30, 
            0xf8, 0xcc, 0xcc, 0xfa, 0xc6, 0xcf, 0xc6, 0xc7, 
            0x0e, 0x1b, 0x18, 0x3c, 0x18, 0x18, 0xd8, 0x70, 
            0x1c, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0x7e, 0x00, 
            0x38, 0x00, 0x70, 0x30, 0x30, 0x30, 0x78, 0x00, 
            0x00, 0x1c, 0x00, 0x78, 0xcc, 0xcc, 0x78, 0x00, 
            0x00, 0x1c, 0x00, 0xcc, 0xcc, 0xcc, 0x7e, 0x00, 
            0x00, 0xf8, 0x00, 0xf8, 0xcc, 0xcc, 0xcc, 0x00, 
            0xfc, 0x00, 0xcc, 0xec, 0xfc, 0xdc, 0xcc, 0x00, 
            0x3c, 0x6c, 0x6c, 0x3e, 0x00, 0x7e, 0x00, 0x00, 
            0x38, 0x6c, 0x6c, 0x38, 0x00, 0x7c, 0x00, 0x00, 
            0x30, 0x00, 0x30, 0x60, 0xc0, 0xcc, 0x78, 0x00, 
            0x00, 0x00, 0x00, 0xfc, 0xc0, 0xc0, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0xfc, 0x0c, 0x0c, 0x00, 0x00, 
            0xc3, 0xc6, 0xcc, 0xde, 0x33, 0x66, 0xcc, 0x0f, 
            0xc3, 0xc6, 0xcc, 0xdb, 0x37, 0x6f, 0xcf, 0x03, 
            0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x18, 0x00, 
            0x00, 0x33, 0x66, 0xcc, 0x66, 0x33, 0x00, 0x00, 
            0x00, 0xcc, 0x66, 0x33, 0x66, 0xcc, 0x00, 0x00, 
            0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 
            0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 
            0xdb, 0x77, 0xdb, 0xee, 0xdb, 0x77, 0xdb, 0xee, 
            0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 
            0x18, 0x18, 0x18, 0x18, 0xf8, 0x18, 0x18, 0x18, 
            0x18, 0x18, 0xf8, 0x18, 0xf8, 0x18, 0x18, 0x18, 
            0x36, 0x36, 0x36, 0x36, 0xf6, 0x36, 0x36, 0x36, 
            0x00, 0x00, 0x00, 0x00, 0xfe, 0x36, 0x36, 0x36, 
            0x00, 0x00, 0xf8, 0x18, 0xf8, 0x18, 0x18, 0x18, 
            0x36, 0x36, 0xf6, 0x06, 0xf6, 0x36, 0x36, 0x36, 
            0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 
            0x00, 0x00, 0xfe, 0x06, 0xf6, 0x36, 0x36, 0x36, 
            0x36, 0x36, 0xf6, 0x06, 0xfe, 0x00, 0x00, 0x00, 
            0x36, 0x36, 0x36, 0x36, 0xfe, 0x00, 0x00, 0x00, 
            0x18, 0x18, 0xf8, 0x18, 0xf8, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00, 0xf8, 0x18, 0x18, 0x18, 
            0x18, 0x18, 0x18, 0x18, 0x1f, 0x00, 0x00, 0x00, 
            0x18, 0x18, 0x18, 0x18, 0xff, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00, 0xff, 0x18, 0x18, 0x18, 
            0x18, 0x18, 0x18, 0x18, 0x1f, 0x18, 0x18, 0x18, 
            0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 
            0x18, 0x18, 0x18, 0x18, 0xff, 0x18, 0x18, 0x18, 
            0x18, 0x18, 0x1f, 0x18, 0x1f, 0x18, 0x18, 0x18, 
            0x36, 0x36, 0x36, 0x36, 0x37, 0x36, 0x36, 0x36, 
            0x36, 0x36, 0x37, 0x30, 0x3f, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x3f, 0x30, 0x37, 0x36, 0x36, 0x36, 
            0x36, 0x36, 0xf7, 0x00, 0xff, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0xff, 0x00, 0xf7, 0x36, 0x36, 0x36, 
            0x36, 0x36, 0x37, 0x30, 0x37, 0x36, 0x36, 0x36, 
            0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 
            0x36, 0x36, 0xf7, 0x00, 0xf7, 0x36, 0x36, 0x36, 
            0x18, 0x18, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 
            0x36, 0x36, 0x36, 0x36, 0xff, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0xff, 0x00, 0xff, 0x18, 0x18, 0x18, 
            0x00, 0x00, 0x00, 0x00, 0xff, 0x36, 0x36, 0x36, 
            0x36, 0x36, 0x36, 0x36, 0x3f, 0x00, 0x00, 0x00, 
            0x18, 0x18, 0x1f, 0x18, 0x1f, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x1f, 0x18, 0x1f, 0x18, 0x18, 0x18, 
            0x00, 0x00, 0x00, 0x00, 0x3f, 0x36, 0x36, 0x36, 
            0x36, 0x36, 0x36, 0x36, 0xff, 0x36, 0x36, 0x36, 
            0x18, 0x18, 0xff, 0x18, 0xff, 0x18, 0x18, 0x18, 
            0x18, 0x18, 0x18, 0x18, 0xf8, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00, 0x1f, 0x18, 0x18, 0x18, 
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
            0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
            0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 
            0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 
            0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x76, 0xdc, 0xc8, 0xdc, 0x76, 0x00, 
            0x00, 0x78, 0xcc, 0xf8, 0xcc, 0xf8, 0xc0, 0xc0, 
            0x00, 0xfc, 0xcc, 0xc0, 0xc0, 0xc0, 0xc0, 0x00, 
            0x00, 0xfe, 0x6c, 0x6c, 0x6c, 0x6c, 0x6c, 0x00, 
            0xfc, 0xcc, 0x60, 0x30, 0x60, 0xcc, 0xfc, 0x00, 
            0x00, 0x00, 0x7e, 0xd8, 0xd8, 0xd8, 0x70, 0x00, 
            0x00, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x60, 0xc0, 
            0x00, 0x76, 0xdc, 0x18, 0x18, 0x18, 0x18, 0x00, 
            0xfc, 0x30, 0x78, 0xcc, 0xcc, 0x78, 0x30, 0xfc, 
            0x38, 0x6c, 0xc6, 0xfe, 0xc6, 0x6c, 0x38, 0x00, 
            0x38, 0x6c, 0xc6, 0xc6, 0x6c, 0x6c, 0xee, 0x00, 
            0x1c, 0x30, 0x18, 0x7c, 0xcc, 0xcc, 0x78, 0x00, 
            0x00, 0x00, 0x7e, 0xdb, 0xdb, 0x7e, 0x00, 0x00, 
            0x06, 0x0c, 0x7e, 0xdb, 0xdb, 0x7e, 0x60, 0xc0, 
            0x38, 0x60, 0xc0, 0xf8, 0xc0, 0x60, 0x38, 0x00, 
            0x78, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x00, 
            0x00, 0xfc, 0x00, 0xfc, 0x00, 0xfc, 0x00, 0x00, 
            0x30, 0x30, 0xfc, 0x30, 0x30, 0x00, 0xfc, 0x00, 
            0x60, 0x30, 0x18, 0x30, 0x60, 0x00, 0xfc, 0x00, 
            0x18, 0x30, 0x60, 0x30, 0x18, 0x00, 0xfc, 0x00, 
            0x0e, 0x1b, 0x1b, 0x18, 0x18, 0x18, 0x18, 0x18, 
            0x18, 0x18, 0x18, 0x18, 0x18, 0xd8, 0xd8, 0x70, 
            0x30, 0x30, 0x00, 0xfc, 0x00, 0x30, 0x30, 0x00, 
            0x00, 0x76, 0xdc, 0x00, 0x76, 0xdc, 0x00, 0x00, 
            0x38, 0x6c, 0x6c, 0x38, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 
            0x0f, 0x0c, 0x0c, 0x0c, 0xec, 0x6c, 0x3c, 0x1c, 
            0x78, 0x6c, 0x6c, 0x6c, 0x6c, 0x00, 0x00, 0x00, 
            0x70, 0x18, 0x30, 0x60, 0x78, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

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
                this->window = new raylib::Window(40 * 8 * SCALE, 30 * 8 * SCALE); // 40x30 - 8�8
                this->debug_ui = new UI::DebugUI(this->cpu->get_pc());
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

                    this->begin_drawing();

                    window->ClearBackground(RAYWHITE);

                    for (size_t i = 0; i < 40 * 30; i++) {
                        size_t x = i % 40;
                        size_t y = i / 40;

                        uint8_t c  = *(this->video_device->vram_array() + (((x * 30) + y + 0) - i));
                        uint8_t fr = *(this->video_device->vram_array() + (((x * 30) + y + 1) - i));
                        uint8_t fg = *(this->video_device->vram_array() + (((x * 30) + y + 2) - i));
                        uint8_t fb = *(this->video_device->vram_array() + (((x * 30) + y + 3) - i));
                        uint8_t br = *(this->video_device->vram_array() + (((x * 30) + y + 4) - i));
                        uint8_t bg = *(this->video_device->vram_array() + (((x * 30) + y + 5) - i));
                        uint8_t bb = *(this->video_device->vram_array() + (((x * 30) + y + 6) - i));

                        int32_t mask[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };
                        uint8_t* gylph = seabios8x8 + (int)c * 8;

                        for (size_t j = 0; j < 64; j++) {
                            size_t cx = j % 8;
                            size_t cy = j / 8;

                            if (gylph[cy] & mask[cx])
                                //raylib::Color::Color(fr, fg, fb).DrawRectangle((x * 8 + cx) * SCALE, (y * 8 + cy) * SCALE, SCALE, SCALE);
                                raylib::Color::Color(fr, fg, fb).DrawPixel(x * 8 + cx, y * 8 + cy);
                            else
                                //raylib::Color::Color(br, bg, bb).DrawRectangle((x * 8 + cx) * SCALE, (y * 8 + cy) * SCALE, SCALE, SCALE);
                                raylib::Color::Color(br, bg, bb).DrawPixel(x * 8 + cx, y * 8 + cy);
                        }
                    }

                    this->debug_ui->draw(this->window->GetFPS());

                    this->end_drawing();
                }
            }

            void set_options(Options options) { this->opts = options; };

            size_t add_video_device(VideoDevice* video_device) {
                this->video_device = video_device;

                return this->add_device(video_device);
            }

        private:
            Options opts = {};

            void begin_drawing() {
                this->window->BeginDrawing();
                rlImGuiBegin();
            }

            void end_drawing() {
                rlImGuiEnd();
                this->window->EndDrawing();
            }

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