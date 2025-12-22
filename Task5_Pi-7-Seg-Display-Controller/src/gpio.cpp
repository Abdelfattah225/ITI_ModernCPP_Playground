#include "gpio.hpp"
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <cerrno>

namespace MCAL {
    namespace GPIO {

        // ---------- Private helpers ----------
        int GpioPin::writeToFile(const std::string& path, const std::string& value) {
            fd = open(path.c_str(), O_WRONLY);
            if (fd < 0) {
                std::cerr << "Error: Can't open " << path << " - " << strerror(errno) << std::endl;
                return -1;
            }
            auto numBytes = write(fd, value.c_str(), value.length());
            close(fd);
            return numBytes;
        }

        std::string GpioPin::readFromFile(const std::string& path) {
            fd = open(path.c_str(), O_RDONLY);
            if (fd < 0) {
                std::cerr << "Error: Can't open " << path << " - " << strerror(errno) << std::endl;
                return "";
            }
            char buffer[64];
            auto numBytes = read(fd, buffer, sizeof(buffer) - 1);
            close(fd);

            if (numBytes > 0) {
                buffer[numBytes] = '\0';
                return std::string(buffer);
            }
            return "";
        }

        void GpioPin::ActivePin() {
            int absolutePin = GPIO_BASE + PinNumber;
            std::string pinStr = std::to_string(absolutePin);
            std::cout << "Exporting GPIO " << absolutePin << " (Pin " << PinNumber << ")" << std::endl;
            writeToFile(ExportPATH, pinStr);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        void GpioPin::DeactivePin() {
            int absolutePin = GPIO_BASE + PinNumber;
            std::string pinStr = std::to_string(absolutePin);
            std::cout << "Unexporting GPIO " << absolutePin << std::endl;
            writeToFile(UnexportPATH, pinStr);
        }

        // ---------- Constructors ----------
        GpioPin::GpioPin() : PinNumber(-1), PinDirection(PinOUT), PinState(PinLow), fd(-1) {}

        GpioPin::GpioPin(int Num) : PinNumber(Num), PinDirection(PinOUT), PinState(PinLow), fd(-1) {
            ActivePin();
        }

        GpioPin::GpioPin(int Num, int dir) : PinNumber(Num), PinDirection(dir), PinState(PinLow), fd(-1) {
            ActivePin();
            SetPinDir(PinDirection);
        }

        GpioPin::GpioPin(int Num, int dir, int state) : PinNumber(Num), PinDirection(dir), PinState(state), fd(-1) {
            ActivePin();
            SetPinDir(PinDirection);
            SetPinVal(PinState);
        }

        // ---------- Move constructor / assignment ----------
        GpioPin::GpioPin(GpioPin && ref) noexcept
            : fd(ref.fd), PinNumber(ref.PinNumber), PinDirection(ref.PinDirection), PinState(ref.PinState)
        {
            ref.PinNumber = -1; // prevent deactivation in moved-from
        }

        GpioPin & GpioPin::operator=(GpioPin && ref) noexcept {
            if(this != &ref) {
                if(PinNumber != -1) DeactivePin();
                fd = ref.fd;
                PinNumber = ref.PinNumber;
                PinDirection = ref.PinDirection;
                PinState = ref.PinState;
                ref.PinNumber = -1;
            }
            return *this;
        }

        // ---------- Methods ----------
        void GpioPin::SetPinDir(int dir) {
            PinDirection = dir;
            int absolutePin = GPIO_BASE + PinNumber;
            std::string path = "/sys/class/gpio/gpio" + std::to_string(absolutePin) + "/direction";

            if(PinDirection == PinIN) writeToFile(path, "in");
            else if(PinDirection == PinOUT) writeToFile(path, "out");
            else std::cout << "Invalid pin Direction\n";
        }

        void GpioPin::SetPinVal(int val) {
            PinState = val;
            int absolutePin = GPIO_BASE + PinNumber;
            std::string path = "/sys/class/gpio/gpio" + std::to_string(absolutePin) + "/value";

            if(val == PinLow) writeToFile(path, "0");
            else if(val == PinHigh) writeToFile(path, "1");
            else std::cout << "Invalid pin Value\n";
        }

        void GpioPin::Toggle_Pin() {
            SetPinVal(PinHigh);
            sleep(3);
            SetPinVal(PinLow);
            sleep(3);
        }

        int GpioPin::GetPinValue() {
            int absolutePin = GPIO_BASE + PinNumber;
            std::string PinValue = readFromFile("/sys/class/gpio/gpio" + std::to_string(absolutePin) + "/value");
            return std::stoi(PinValue);
        }

        GpioPin::~GpioPin() {
            if(PinNumber != -1) DeactivePin();
        }

        // ---------- Initialize multiple pins ----------
        std::vector<GpioPin> GPIO_InitPins(std::initializer_list<int> PinNumbers , int dir , int state) {
            std::vector<GpioPin> pins;
            pins.reserve(PinNumbers.size());
            for(auto PinNumber : PinNumbers)
                pins.emplace_back(PinNumber, dir, state);
            return pins;
        }

        std::vector<GpioPin> GPIO_InitPins(std::initializer_list<PinsConfig> configs) {
            std::vector<GpioPin> pins;
            pins.reserve(configs.size());
            for(auto cfg : configs)
                pins.emplace_back(cfg.PinNumber, cfg.PinDir, cfg.PinState);
            return pins;
        }

    } // namespace GPIO
} // namespace MCAL
