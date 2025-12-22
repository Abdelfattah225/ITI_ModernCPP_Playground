#pragma once

#include "Stream.hpp"
#include "stdexcept"

namespace HardwareIO{

    class IStream : virtual public Stream{
        private:

        public:
            IStream() = default;
            int readDigit();

    };

}
