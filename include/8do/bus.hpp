#include <CPU/device.hpp>
#include <CPU/devices/ram.hpp>
#include <CPU/devices/rom.hpp>
#include <vector>

namespace EightDo {
    class Bus {
        std::vector<CPU::Device*> devices;

    public:
        size_t add_device(CPU::Device* device) {
            devices.push_back(device);
            return devices.size() - 1;
        }

        void remove_device(size_t idx) {
            devices.erase(devices.begin() + idx);
        }

        CPU::Device::Result read_write(EightDo::Common::ExtendedAddress address) {
            for (CPU::Device* device : devices) {
                CPU::Device::Result result = device->read(address);

                if (result.status == CPU::Device::Result::Status::NotMyAddress) {
                    continue;
                }

                if (result.status == CPU::Device::Result::Status::WriteOnly) {
                    continue;
                }
                
                return result;
            }

            return { .status = CPU::Device::Result::Status::NoValidDevice, .value = 1 };
        }

        CPU::Device::Result read_write(EightDo::Common::ExtendedAddress address, uint8_t data) {
            for (CPU::Device* device : devices) {
                CPU::Device::Result result = device->write(address, data);

                if (result.status == CPU::Device::Result::Status::NotMyAddress) {
                    continue;
                }

                if (result.status == CPU::Device::Result::Status::ReadOnly) {
                    continue;
                }

                return result;
            }

            return { .status = CPU::Device::Result::Status::NoValidDevice, .value = 1 };
        }
    };
}