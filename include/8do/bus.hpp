#include <8do/device.hpp>
#include <8do/devices.hpp>
#include <vector>

namespace EightDo {
    class Bus {
        std::vector<EightDo::Device*> devices;

    public:
        size_t add_device(EightDo::Device* device) {
            devices.push_back(device);
            return devices.size() - 1;
        }

        void remove_device(size_t idx) {
            devices.erase(devices.begin() + idx);
        }

        EightDo::Device::Result read_write(EightDo::Common::ExtendedAddress address) {
            for (EightDo::Device* device : devices) {
                EightDo::Device::Result result = device->read(address);

                if (result.status == EightDo::Device::Result::Status::NotMyAddress) {
                    continue;
                }

                if (result.status == EightDo::Device::Result::Status::WriteOnly) {
                    continue;
                }
                
                return result;
            }

            return { .status = EightDo::Device::Result::Status::NoValidDevice, .value = 1 };
        }

        EightDo::Device::Result read_write(EightDo::Common::ExtendedAddress address, uint8_t data) {
            for (EightDo::Device* device : devices) {
                EightDo::Device::Result result = device->write(address, data);

                if (result.status == EightDo::Device::Result::Status::NotMyAddress) {
                    continue;
                }

                if (result.status == EightDo::Device::Result::Status::ReadOnly) {
                    continue;
                }

                return result;
            }

            return { .status = EightDo::Device::Result::Status::NoValidDevice, .value = 1 };
        }
    };
}