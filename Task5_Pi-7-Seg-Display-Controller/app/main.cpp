#include <iostream>
#include <memory>
#include "SevenSegment.hpp"
#include "terminal.hpp"


int main() {
    using namespace HardwareIO;

    std::cout << "=== Raspberry Pi 7-Segment Controller ===" << std::endl;
    std::cout << std::endl;
    
    // ============================================
    // 1. Runtime Output Selection
    // ============================================
    std::cout << "Select output device:" << std::endl;
    std::cout << "1. Terminal" << std::endl;
    std::cout << "2. Seven Segment Display" << std::endl;
    std::cout << "Enter choice (1 or 2): ";
    
    int choice;
    std::cin >> choice;
    
    // ============================================
    // 2. Use shared_ptr for Multiple References
    // ============================================
    std::shared_ptr<IStream> input;
    std::shared_ptr<OStream> output;
    
    if (choice == 1) {
        auto device = std::make_shared<Terminal>();
        input = device;   // shared_ptr can be copied!
        output = device;  // Both point to same object
        std::cout << ">>> Using Terminal Output <<<" << std::endl;
    } 
    else if (choice == 2) {
        auto device = std::make_shared<SevenSegment>();
        input = device;
        output = device;
        std::cout << ">>> Using 7-Segment Display <<<" << std::endl;
    } 
    else {
        std::cerr << "Invalid choice! Defaulting to Terminal." << std::endl;
        auto device = std::make_shared<Terminal>();
        input = device;
        output = device;
    }
    
    std::cout << std::endl;

    // ============================================
    // 3. Main Loop
    // ============================================
    bool running = true;
    
    while (running) {
        try {
            int digit = input->readDigit();
            output->writeDigit(digit);
            
            std::cout << "Digit " << digit << " displayed successfully." << std::endl;
        } 
        catch (const std::invalid_argument& e) {
            std::cerr << "Input Error: " << e.what() << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Unexpected error: " << e.what() << std::endl;
        }

        std::cout << std::endl;
        std::cout << "Press 'q' to quit or any other key to continue: ";
        char ch;
        std::cin >> ch;
        
        if (ch == 'q' || ch == 'Q') {
            running = false;
        }
    }

    std::cout << "Goodbye!" << std::endl;
    return 0;
}