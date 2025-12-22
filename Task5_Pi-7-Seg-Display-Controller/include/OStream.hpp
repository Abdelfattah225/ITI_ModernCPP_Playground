#pragma once
#include "Stream.hpp"
#include "stdexcept"

namespace HardwareIO{

    class OStream : virtual public Stream{
        private:

        public:

            virtual void writeDigit(int) = 0 ; 
   
    };

}

