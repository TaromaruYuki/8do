#pragma once

#include <8do/device.hpp>
#include <raylib-cpp.hpp>

namespace EightDo::Devices {
    class Video : public EightDo::Device {
        enum struct Mode {
            ClassicText = 0
        };

        Mode video_mode;
        uint8_t* vram;

    public:
        Video() {
            this->vram = new uint8_t[0xFFEF];
        }
    };
}