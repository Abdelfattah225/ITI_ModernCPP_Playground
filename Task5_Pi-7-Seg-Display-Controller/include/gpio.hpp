#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <initializer_list>

namespace MCAL {
    namespace GPIO {

        // Constants
        constexpr int GPIO_BASE = 512;

        constexpr int PinIN = 0;
        constexpr int PinOUT = 1;
        constexpr int PinHigh = 1;
        constexpr int PinLow = 0;

        struct PinsConfig {
            int PinNumber;
            int PinState;
            int PinDir;
        };

        class GpioPin {
        private:
            int fd;
            int PinNumber;
            int PinDirection;
            int PinState;

            const std::string ExportPATH = "/sys/class/gpio/export";
            const std::string UnexportPATH = "/sys/class/gpio/unexport";

            int writeToFile(const std::string& path, const std::string& value);
            std::string readFromFile(const std::string& path);
            void ActivePin();
            void DeactivePin();

        public:
            // Constructors
            GpioPin();
            GpioPin(int Num);
            GpioPin(int Num, int dir);
            GpioPin(int Num, int dir, int state);

            // Rule of Five
            GpioPin(const GpioPin & ref) = delete;
            GpioPin & operator=(const GpioPin & ref) = delete;
            GpioPin(GpioPin && ref) noexcept;
            GpioPin & operator=(GpioPin && ref) noexcept;

            // Methods
            void SetPinDir(int dir);
            void SetPinVal(int val);
            void Toggle_Pin();
            int GetPinValue();

            // Destructor
            ~GpioPin();
        };

        // Initialize multiple pins with same config
        std::vector<GpioPin> GPIO_InitPins(std::initializer_list<int> PinNumbers , int dir , int state);

        // Initialize multiple pins with custom configs
        std::vector<GpioPin> GPIO_InitPins(std::initializer_list<PinsConfig> configs);

    }
}
