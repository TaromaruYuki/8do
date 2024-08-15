#include <iostream>
#include <filesystem>
#include <fstream>
#include <8do/8do.hpp>

int main() {
	EightDo::Emulator emulator;

    std::string file = "../../../gen/stack.bin";
	
    if(!std::filesystem::exists(file)) {
        std::cerr << "File '" << file << "' does not exist." << std::endl;
        return 1;
    }

    std::ifstream fs(file, std::ios::in | std::ios::binary);

    fs.read(reinterpret_cast<char*>(emulator.ram), 0xFFFF);

    if(fs.bad()) {
        std::cerr << "Filestream for file '" << file << "' is bad." << std::endl;
        return 1;
    }

    fs.close();

	emulator.start();

	return 0;
}