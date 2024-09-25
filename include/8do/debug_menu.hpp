#pragma once

#include <inttypes.h>
#include <8do/common.hpp>
#include <imgui.h>

namespace EightDo::UI {
    class DebugUI {
        bool open = true;
        Common::ExtendedAddress* pc;
        int32_t pc_buf;

    public:
        DebugUI(Common::ExtendedAddress* pc) {
            this->pc = pc;
        }

        void draw(int fps) {
            if (open) {
                pc_buf = this->pc->address;
                ImGui::Begin("Debug", &this->open);
                ImGui::Text("FPS: %d", fps);

                ImGui::InputInt("Program Counter", &this->pc_buf, 1, 0x10, ImGuiInputTextFlags_CharsHexadecimal);

                ImGui::End();
            }
        }
    };
}