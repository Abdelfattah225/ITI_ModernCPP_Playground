#include "IStream.hpp"
#include <iostream>  
#include <limits>    

namespace HardwareIO {

    int IStream::readDigit() {
        char inputChar;
        std::cout << "Enter a digit (0-9): ";
        std::cin >> inputChar;

        if (inputChar >= '0' && inputChar <= '9') {
        return static_cast<int>(inputChar - '0'); 
        } else {
            throw std::invalid_argument("Input is not a single digit between 0 and 9.");
        }
    }

}