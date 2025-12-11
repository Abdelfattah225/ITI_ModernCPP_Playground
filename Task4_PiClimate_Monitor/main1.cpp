#include <iostream>
#include <memory>
#include <array>
#include <cstdint>
#include <pigpio.h>
#include <stdexcept>

#include <cstring>  
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctime>
#include <unistd.h>
enum class SensorStatus
{
    Ok,
    Timeout,
    ChecksumError,
    NotConnected
};

struct SensorReading
{
    /* data */
    float Temperature;
    float humidity;
};
enum class PinDirection
{
    Input,
    Output
};
enum class PinValue
{
    Low,
    High
};

enum class PinNumber
{
    GPIO_PIN_2 = 2, // I2C1 SDA
    GPIO_PIN_3 = 3, // I2C1 SCL
    GPIO_PIN_4 = 4, // GPCLK0
    GPIO_PIN_5 = 5,
    GPIO_PIN_6 = 6,
    GPIO_PIN_7 = 7,   // SPI0 CE1
    GPIO_PIN_8 = 8,   // SPI0 CE0
    GPIO_PIN_9 = 9,   // SPI0 MISO
    GPIO_PIN_10 = 10, // SPI0 MOSI
    GPIO_PIN_11 = 11, // SPI0 SCLK
    GPIO_PIN_12 = 12, // PWM0
    GPIO_PIN_13 = 13, // PWM1
    GPIO_PIN_14 = 14, // UART TXD
    GPIO_PIN_15 = 15, // UART RXD
    GPIO_PIN_16 = 16,
    GPIO_PIN_17 = 17,
    GPIO_PIN_18 = 18, // PCM CLK / PWM0
    GPIO_PIN_19 = 19, // PCM FS / PWM1
    GPIO_PIN_20 = 20, // PCM DIN
    GPIO_PIN_21 = 21, // PCM DOUT
    GPIO_PIN_22 = 22,
    GPIO_PIN_23 = 23,
    GPIO_PIN_24 = 24,
    GPIO_PIN_25 = 25,
    GPIO_PIN_26 = 26,
    GPIO_PIN_27 = 27
};

class GPIO
{
private:
    PinNumber pinNumber;
    PinValue pinValue;
    PinDirection pinDir;
    static uint32_t InstanceCounter;

public:
    /* backbone functions*/

    // Prevent accidental conversions
    explicit GPIO(PinNumber pin) : pinNumber{pin}
    {
        // intialization using pigpio
        if (InstanceCounter == 0)
        {
            int status = gpioInitialise();
            if (status < 0)
            {
                throw std::runtime_error("GPIO Init Faild");
            }
        }
        InstanceCounter += 1;
        // default direction is input :)
        this->SetDir(PinDirection::Input);
    }
    // each pin should have one owner
    // Two objects shouldn't control same pin
    GPIO(const GPIO &) = delete;
    GPIO &operator=(const GPIO &) = delete;
    // allowing moving
    // Can transfer ownership
    GPIO(GPIO &&) = default;
    GPIO &operator=(GPIO &&) = default;

    ~GPIO()
    {
        // defalut state of gpio pin
        SetDir(PinDirection::Input);
        this->InstanceCounter--;
        if (InstanceCounter == 0)
        {
            gpioTerminate();
        }
    }

    /* member functions */
    void SetValue(PinValue value)
    {
        this->pinValue = value;
        // actual logic
        gpioWrite(static_cast<unsigned int>(this->pinNumber), static_cast<unsigned int>(this->pinValue));
    }
    //  They don't modify the object because using const
    PinValue GetValue() const
    {
        // return pinvalue
        unsigned int val;
        val = gpioRead(static_cast<unsigned int>(this->pinNumber));
        return (static_cast<PinValue>(val));
    }

    void SetDir(PinDirection dir)
    {
        this->pinDir = dir;
        // actual logic
        gpioSetMode(static_cast<unsigned int>(this->pinNumber), static_cast<unsigned int>(this->pinDir));
    }
    //  They don't modify the object because using const
    PinDirection GetDir() const
    {
        // return pindir
        unsigned int dir;
        dir = gpioGetMode(static_cast<unsigned int>(this->pinNumber));
        return (static_cast<PinDirection>(dir));
    }
};

class Sensor
{
protected:
public:
    Sensor() = default;
    virtual ~Sensor() = default;

    virtual std::pair<SensorStatus, SensorReading> ReadSensorData() = 0;
};

class DHT11 : public Sensor
{
private:
    std::array<uint8_t, 5> buffer;
    std::unique_ptr<GPIO> _gpio;
    static constexpr int MAX_RETRIES = 5;

public:
    DHT11() = delete;
    // Prevent accidental conversions
    explicit DHT11(PinNumber pin) : _gpio{std::make_unique<GPIO>(pin)}, buffer{}
    {
        std::cout << "Waiting 2 seconds for sensor warm-up...\n";
        gpioDelay(2000000); // 2 seconds = 2,000,000 microseconds
    }
    // override tells the compiler: "I intend to override a virtual function from the base class."
    std::pair<SensorStatus, SensorReading> ReadSensorData() override
    {

        for (int attempt = 1; attempt <= MAX_RETRIES; attempt++)
        {
            std::cout << "Attempt " << attempt << " of " << MAX_RETRIES << "...\n";

            bool attemptFailed = false;

            // step1; pi to sensor => start signal
            _gpio->SetDir(PinDirection::Output);
            _gpio->SetValue(PinValue::Low);
            gpioDelay(18000); // wait 18msec
            _gpio->SetValue(PinValue::High);
            gpioDelay(30); // wait 30 usec

            // step2: sensor to pi => ack signal
            _gpio->SetDir(PinDirection::Input);

            // wait for low 80 usec
            if (waitForPin(PinValue::Low, 200) != SensorStatus::Ok)
            {
                std::cout << "Error Receving Low ack form sensor\n";
                attemptFailed = true;
            }
            if (!attemptFailed && waitForPin(PinValue::High, 200) != SensorStatus::Ok)
            {
                std::cout << "Error Receving High ack form sensor\n";
                attemptFailed = true;
            }

            // fill buffer with zerooooooooooos
            buffer.fill(0);
            // step3: get sensor values

            if (!attemptFailed)
            {
                for (int i = 0; i < 40; i++)
                {
                    int byteIndex = i / 8;      // Which byte?
                    int bitIndex = 7 - (i % 8); // Which bit in that byte?

                    // Wait for LOW marker to end
                    if (waitForPin(PinValue::Low, 200) != SensorStatus::Ok)
                    {
                        std::cout << "Error: Receving Low marker at bit " << i << "\n";
                        attemptFailed = true;
                        break;
                    }
                    // Wait for HIGH to start
                    if (waitForPin(PinValue::High, 200) != SensorStatus::Ok)
                    {
                        std::cout << "Error: Receving High marker at bit " << i << "\n";
                        attemptFailed = true;
                        break;
                    }

                    // Measure HIGH duration
                    // If duration > 40μs → bit is 1
                    // Store bit in buffer

                    int duration = MeasureHighDuration();
                    if (duration == -1)
                    {
                        // FIX: Don't throw, set flag and break
                        std::cout << "Error: high duraiton timeout at bit " << i << "\n";
                        attemptFailed = true;
                        break;
                    }
                    else if (duration > 40)
                    {
                        // To set a bit:
                        buffer[byteIndex] |= (1 << bitIndex);
                    }
                }
            }

            // Check if read succeeded and validate checksum
            if (!attemptFailed)
            {
                // step4: Validate checksum
                if (checkSum() == SensorStatus::Ok)
                {
                    // Success! Return the reading
                    SensorReading reading;
                    reading.Temperature = extractTemperature();
                    reading.humidity = extractHumidity();
                    return {SensorStatus::Ok, reading};
                }
                else
                {
                    std::cout << "Checksum error\n";
                }
            }

            // Wait 2 seconds before next attempt
            if (attempt < MAX_RETRIES)
            {
                std::cout << "Retrying in 2 seconds...\n";
                gpioDelay(2000000);
            }
        }

        return {SensorStatus::Timeout, {}};
    }
    SensorStatus checkSum() const
    {
        uint32_t sum = 0;
        SensorStatus status;
        for (int i = 0; i < buffer.max_size() - 1; i++)
        {
            sum += buffer[i];
        }
        // we must mask the sum value with 0xff because the value of sum may be greater than 255
        if ((sum & 0xff) == buffer[4])
        {
            status = SensorStatus::Ok;
        }
        else
        {
            status = SensorStatus::ChecksumError;
        }
        return status;
    }

    float extractTemperature() const
    {
        // DHT11: buffer[2] = integer part, buffer[3] = decimal part
        return static_cast<float>(buffer[2]) + static_cast<float>(buffer[3]) / 10.0f;
    }
    float extractHumidity() const
    {
        // DHT11: buffer[0] = integer part, buffer[1] = decimal part
        return static_cast<float>(buffer[0]) + static_cast<float>(buffer[1]) / 10.0f;
    }

    // helper function
    SensorStatus waitForPin(PinValue expected, uint32_t timeoutMicros)
    {
        uint32_t startTick = gpioTick();

        while (_gpio->GetValue() != expected)
        {
            if (gpioTick() - startTick > timeoutMicros)
            {
                return SensorStatus::Timeout;
            }
        }

        return SensorStatus::Ok;
    }
    int MeasureHighDuration()
    {
        uint32_t startTick = gpioTick(); // Get start timestamp

        while (_gpio->GetValue() == PinValue::High)
        {
            if (gpioTick() - startTick > 200) // Timeout
            {
                return -1;
            }
        }

        return gpioTick() - startTick; // Actual elapsed microseconds
    }
};

class SensorLogger
{

private:
    SensorReading reading;
    std::string filename;
    int fd;
    char timeBuffer[64];
    char logBuffer[128];

public:
    SensorLogger(std::string name) : filename{name}
    {
    }
    ~SensorLogger(){
    };
    SensorLogger(const SensorLogger&) = delete;
    SensorLogger& operator=(const SensorLogger&) = delete;

    void log(SensorReading Reading) 
    {
         reading = Reading;
        get_currentTime();
        sprintf(logBuffer, "[%s] Temp: %.1f°C, Humidity: %.1f%%\n",timeBuffer, reading.Temperature, reading.humidity);
        writeToFile(filename,logBuffer);
        close(fd);

    }

    int writeToFile(const std::string &path, const std::string &value)
    {
        fd = open(path.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);   
        if (fd < 0)
        {
            std::cerr << "Error: Can't open " << path << std::endl;
            return -1;
        }
        auto numBytes = write(fd, value.c_str(), value.length());
       
        return numBytes;
    }
    void get_currentTime()
    {
        std::time_t now = std::time(nullptr);
        std::tm *tm = std::localtime(&now);
        std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", tm);
    }
};

uint32_t GPIO::InstanceCounter = 0;


int main()
{
    try
    {
        DHT11 sensor(PinNumber::GPIO_PIN_4);
        SensorLogger logger("sensor_log.txt"); 
        
        while (true)
        {
            auto [status, reading] = sensor.ReadSensorData();
            
            if (status == SensorStatus::Ok)
            {
                std::cout << "Temp: " << reading.Temperature << "°C | ";
                std::cout << "Humidity: " << reading.humidity << "%\n";
                
                logger.log(reading); 
            }
            
            gpioDelay(2000000);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }
    return 0;
}