#pragma once

#include <iostream>


namespace EightDo::Common {
    class ConsoleWindow {
        uint16_t cursor_x = 0;
        uint16_t cursor_y = 0;

    public:
        void move_cursor(uint16_t x, uint16_t y) {
            std::cout << "\033[" << y + 1 << ";" << x + 1 << "H";
            cursor_x = x;
            cursor_y = y;
        }

        void move_cursor_up() {
            std::cout << "\033[" << 1 << "A";
            cursor_y--;
        }

        void move_cursor_down() {
            std::cout << "\033[" << 1 << "B";
            cursor_y++;
        }

        void move_cursor_left() {
            std::cout << "\033[" << 1 << "D";
            cursor_x--;
        }

        void move_cursor_right() {
            std::cout << "\033[" << 1 << "C";
            cursor_x++;
        }

        void save_cursor_position() {
            std::cout << "\033[s";
        }

        void restore_cursor_position() {
            std::cout << "\033[u";
        }

        void show_cursor() {
            std::cout << "\033[?25h";
        }

        void hide_cursor() {
            std::cout << "\033[?25l";
        }

        void print_char(char c) {
            std::cout << c;
        }
    };
}