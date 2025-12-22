#include "SevenSegment.hpp"
#include "gpio.hpp"

namespace HardwareIO
{
    void SevenSegment::init_map()
    {
        SevenSegment::representation = {
            {0, {0, 0, 0, 0, 0, 0, 1}},
            {1, {1, 0, 0, 1, 1, 1, 1}},
            {2, {0, 0, 1, 0, 0, 1, 0}},
            {3, {0, 0, 0, 0, 1, 1, 0}},
            {4, {1, 0, 0, 1, 1, 0, 0}},
            {5, {0, 1, 0, 0, 1, 0, 0}},
            {6, {0, 1, 0, 0, 0, 0, 0}},
            {7, {0, 0, 0, 1, 1, 1, 1}},
            {8, {0, 0, 0, 0, 0, 0, 0}},
            {9, {0, 0, 0, 0, 1, 0, 0}}};
    }
    SevenSegment::SevenSegment()
    {
        init_map();
        arr.fill(0); // initialize segment states
        this->Pins = std::move(MCAL::GPIO::GPIO_InitPins({
            {SevenSegmentPins[0], MCAL::GPIO::PinHigh, MCAL::GPIO::PinOUT},
            {SevenSegmentPins[1], MCAL::GPIO::PinHigh, MCAL::GPIO::PinOUT},
            {SevenSegmentPins[2], MCAL::GPIO::PinHigh, MCAL::GPIO::PinOUT},
            {SevenSegmentPins[3], MCAL::GPIO::PinHigh, MCAL::GPIO::PinOUT},
            {SevenSegmentPins[4], MCAL::GPIO::PinHigh, MCAL::GPIO::PinOUT},
            {SevenSegmentPins[5], MCAL::GPIO::PinHigh, MCAL::GPIO::PinOUT},
            {SevenSegmentPins[6], MCAL::GPIO::PinHigh, MCAL::GPIO::PinOUT},
        }));
    }

    void SevenSegment::writeDigit(int x)
    {
        SevenSegment::arr = SevenSegment::representation[x];
        for (int i = 0; i < 7; i++)
        {
            if (arr[i] == 0)
            {
                Pins[i].SetPinVal(MCAL::GPIO::PinLow);
            }
            else if(arr[i]==1){
                Pins[i].SetPinVal(MCAL::GPIO::PinHigh);
            }
        }
    }

} // namespace name
