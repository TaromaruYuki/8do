#pragma once

#include <inttypes.h>
#include <CPU/cpu.hpp>

namespace CPU {
	struct Device {
		struct Result {
			enum Status {
				Ok,
				NotMyAddress,
				ReadOnly,
				WriteOnly,
				InvalidAddress
			} status;
			uint8_t value;
		};

		virtual Result read(CPU::ExtendedAddress address) { Result result = { .status = Result::Status::NotMyAddress, .value = 0x00 }; return result; }
		virtual Result write(CPU::ExtendedAddress address, uint8_t data) { Result result = { .status = Result::Status::NotMyAddress, .value = 0x00 }; return result; }
		virtual size_t relative(CPU::ExtendedAddress) { return -1; }
		virtual size_t size() { return -1; }
		virtual CPU::CPU::ExtendedAddress start() { CPU::CPU::ExtendedAddress address = { .address = 0x00 }; return address; }
		virtual CPU::CPU::ExtendedAddress end() { CPU::CPU::ExtendedAddress address = { .address = 0x00 }; return address; }
	};
}