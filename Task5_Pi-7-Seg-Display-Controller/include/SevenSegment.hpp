#pragma once
#include "IStream.hpp"
#include "OStream.hpp"
#include <vector>
#include <memory>
#include <map>
#include <array>
#include "gpio.hpp"

namespace HardwareIO{

    constexpr int SevenSegmentPins[7] = {17,18,19,20,21,22,23};

    class SevenSegment :  public IStream,  public OStream  {
        private:

            std::array<int, 7> arr;
            std::map<int, std::array<int,7> >representation;
            void init_map();
            std::vector<MCAL::GPIO::GpioPin> Pins;
        public:

        SevenSegment();
       

        SevenSegment(const SevenSegment&) = delete;
        SevenSegment& operator=(const SevenSegment&) = delete;

        SevenSegment(SevenSegment&&) = default;
        SevenSegment& operator=(SevenSegment&&) = default;

        void writeDigit(int x) override ;

    };
}