#include "terminal.hpp"

namespace HardwareIO{

    void Terminal::writeDigit(int x) {
        out <<  x << std::endl;
    }
}