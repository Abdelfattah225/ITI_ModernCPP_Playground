#include <iostream>
#include <pigpio.h>
#include <cstdint>

#define DHT_PIN 4
#define MAX_RETRIES 5

bool readDHT11(uint8_t data[5], uint32_t highTimes[40])
{
    // Send start signal
    gpioSetMode(DHT_PIN, PI_OUTPUT);
    gpioWrite(DHT_PIN, 0);
    gpioDelay(18000);
    gpioWrite(DHT_PIN, 1);
    gpioDelay(30);

    // Switch to input
    gpioSetMode(DHT_PIN, PI_INPUT);

    // Wait for LOW ack
    uint32_t timeout = 200;
    while (gpioRead(DHT_PIN) == 1)
    {
        if (--timeout == 0) return false;
        gpioDelay(1);
    }

    // Wait for HIGH ack
    timeout = 200;
    while (gpioRead(DHT_PIN) == 0)
    {
        if (--timeout == 0) return false;
        gpioDelay(1);
    }

    // Wait for HIGH to end
    timeout = 200;
    while (gpioRead(DHT_PIN) == 1)
    {
        if (--timeout == 0) return false;
        gpioDelay(1);
    }

    // Read 40 bits
    for (int i = 0; i < 40; i++)
    {
        int byteIndex = i / 8;
        int bitIndex = 7 - (i % 8);

        // Wait for LOW marker to end
        timeout = 200;
        while (gpioRead(DHT_PIN) == 0)
        {
            if (--timeout == 0) return false;
            gpioDelay(1);
        }

        // Measure HIGH duration
        uint32_t startTick = gpioTick();
        timeout = 200;
        while (gpioRead(DHT_PIN) == 1)
        {
            if (--timeout == 0) return false;
            gpioDelay(1);
        }
        uint32_t highTime = gpioTick() - startTick;
        highTimes[i] = highTime;

        if (highTime > 40)
        {
            data[byteIndex] |= (1 << bitIndex);
        }
    }

    return true;
}

int main()
{
    if (gpioInitialise() < 0)
    {
        std::cerr << "Failed to initialize pigpio!\n";
        return 1;
    }

    std::cout << "pigpio initialized successfully!\n";
    std::cout << "Waiting 2 seconds for sensor warm-up...\n";
    gpioDelay(2000000);

    uint8_t data[5] = {0};
    uint32_t highTimes[40] = {0};
    bool success = false;

    // Retry loop
    for (int attempt = 1; attempt <= MAX_RETRIES; attempt++)
    {
        std::cout << "\n=== Attempt " << attempt << " of " << MAX_RETRIES << " ===\n";
        
        // Clear data
        for (int i = 0; i < 5; i++) data[i] = 0;
        for (int i = 0; i < 40; i++) highTimes[i] = 0;

        if (readDHT11(data, highTimes))
        {
            // Verify checksum
            uint8_t checksum = data[0] + data[1] + data[2] + data[3];
            if (checksum == data[4])
            {
                success = true;
                std::cout << "✓ Read successful!\n";
                break;
            }
            else
            {
                std::cout << "✗ Checksum failed, retrying...\n";
            }
        }
        else
        {
            std::cout << "✗ Read failed, retrying...\n";
        }

        // DHT11 needs 1-2 seconds between reads
        gpioDelay(2000000);
    }

    if (!success)
    {
        std::cerr << "\nFailed after " << MAX_RETRIES << " attempts!\n";
        gpioTerminate();
        return 1;
    }

    // Print debug info
    std::cout << "\n=== HIGH durations (μs) ===\n";
    for (int i = 0; i < 40; i++)
    {
        std::cout << "Bit " << i << ": " << highTimes[i] << "μs → " 
                  << (highTimes[i] > 40 ? "1" : "0") << "\n";
    }

    // Print raw bytes
    std::cout << "\nRaw bytes: ";
    for (int i = 0; i < 5; i++)
    {
        std::cout << std::hex << (int)data[i] << " ";
    }
    std::cout << std::dec << "\n";

    // ============ DHT11 FORMAT ============
    // data[0] = Humidity integer
    // data[1] = Humidity decimal (usually 0)
    // data[2] = Temperature integer
    // data[3] = Temperature decimal
    // data[4] = Checksum
    
    float humidity = static_cast<float>(data[0]) + static_cast<float>(data[1]) / 10.0f;
    float temperature = static_cast<float>(data[2]) + static_cast<float>(data[3]) / 10.0f;

    // Print results
    std::cout << "\n══════════════════════════════\n";
    std::cout << "  Temperature: " << temperature << " °C\n";
    std::cout << "  Humidity:    " << humidity << " %\n";
    std::cout << "══════════════════════════════\n";

    gpioTerminate();
    return 0;
}