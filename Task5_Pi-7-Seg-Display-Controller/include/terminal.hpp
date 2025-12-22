#pragma once

#include "Stream.hpp"
#include "OStream.hpp"
#include "IStream.hpp"
#include <iostream>

namespace HardwareIO {
    class Terminal: public IStream , public OStream{
        private:
            std::ostream & out = std::cout ;
        public:
        Terminal()=default;
            void writeDigit(int) override;
    };
}