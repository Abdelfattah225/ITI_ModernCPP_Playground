#include <iostream>
#include <fstream>
#include <string>
#include <cstring> 
#include <fcntl.h> 
#include <unistd.h> 
#include <thread> 
#include <chrono> 
#include <initializer_list>
#include <vector>


using namespace std;

namespace MCAL {
    namespace GPIO {
        // Base address for your specific hardware (as per your previous request)
        constexpr int GPIO_BASE = 512;
        
        constexpr int PinIN = 0;
        constexpr int PinOUT = 1;
        constexpr int PinHigh = 1;
        constexpr int PinLow = 0;

        class GpioPin {
        private:
            int fd;
            // FIXED: Changed from int* to int to prevent Segmentation Faults
            int PinNumber;
            int PinDirection;
            int PinState;
            
            const string ExportPATH = "/sys/class/gpio/export";
            const string UnexportPATH = "/sys/class/gpio/unexport";

            int writeToFile(const string& path, const string& value) {
                fd = open(path.c_str(), O_WRONLY);
                if (fd < 0) {
                    cerr << "Error: Can't open " << path << endl;
                    return -1;
                }
                auto numBytes = write(fd, value.c_str(), value.length());
                close(fd);
                return numBytes;
            }

            string readFromFile(const string& path) {
            fd = open(path.c_str(), O_RDONLY);
            if (fd < 0) {
                cerr << "Error: Can't open " << path << " - " << strerror(errno) << endl;
                return "";
            }
            char buffer[64];
            auto numBytes = read(fd, buffer, sizeof(buffer) - 1);
            close(fd);
            
            if (numBytes > 0) {
                buffer[numBytes] = '\0';
                return string(buffer);
            }
            return "";
            }
            void ActivePin() {
                int absolutePin = GPIO_BASE + PinNumber; 
                string pinStr = to_string(absolutePin);
                
                cout << "Exporting GPIO " << absolutePin << " (Pin " << PinNumber << ")" << endl;
                writeToFile(ExportPATH, pinStr);
                        std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
            }

            void DeactivePin() {
                int absolutePin = GPIO_BASE + PinNumber; 
                string pinStr = to_string(absolutePin);
                
                cout << "Unexporting GPIO " << absolutePin << endl;
                writeToFile(UnexportPATH, pinStr);
            }

        public:
            // Default constructor
            GpioPin() : PinNumber(-1) {}

            // Constructor with Pin Number only
            GpioPin(int Num){
                 PinNumber = Num;
                 ActivePin();
            }

            // Constructor with Pin + Direction
            GpioPin(int Num, int dir)
            {
                PinNumber = Num;
                PinDirection = dir;
                ActivePin();
                SetPinDir(PinDirection);
            }

            // Full Constructor
            GpioPin(int Num, int dir, int state) {
                PinNumber = Num;
                PinDirection = dir;
                PinState = state;
                
                ActivePin();
                SetPinDir(PinDirection);  
                SetPinVal(PinState);
            }
            GpioPin(const  GpioPin & ref) = delete;
            GpioPin & operator=(const GpioPin & ref) = delete;

            // Move constructor
            GpioPin(GpioPin && ref)
            {
                fd= ref.fd;
                PinNumber = ref.PinNumber;
                PinDirection = ref.PinDirection;
                PinState= ref.PinState; 
                ref.PinNumber = -1; 
            }
            GpioPin & operator=(GpioPin && ref) {
                if(this != &ref)
                {
                    if(PinNumber != -1)
                    {
                        DeactivePin();
                    }
                    fd = ref.fd;
                    PinNumber = ref.PinNumber;
                    PinDirection = ref.PinDirection;
                    PinState = ref.PinState;
                    ref.PinNumber = -1;
                }
                return *this;
                
            }

            void SetPinDir(int dir)
            {
                PinDirection = dir; 
                int absolutePin = GPIO_BASE + PinNumber;
                string path = "/sys/class/gpio/gpio" + to_string(absolutePin) + "/direction";
                
                if(PinDirection == PinIN) {
                    writeToFile(path, "in");
                }
                else if(PinDirection == PinOUT) {
                    writeToFile(path, "out");
                }
                else {
                    cout << "Invalid pin Direction\n";
                }
            }

            
            void SetPinVal(int val)
            {
                PinState = val; 
                int absolutePin = GPIO_BASE + PinNumber;
                string path = "/sys/class/gpio/gpio" + to_string(absolutePin) + "/value";
                
                if(val == PinLow) {
                    writeToFile(path, "0");
                }
                else if(val == PinHigh) {
                    writeToFile(path, "1");
                }
                else {
                    cout << "Invalid pin Value\n";
                }
            }
            void Toggle_Pin()
            {
                cout << "Setting High\n";
                SetPinVal(MCAL::GPIO::PinHigh);
                sleep(3); 

                cout << "Setting Low\n";
                SetPinVal(MCAL::GPIO::PinLow);
                sleep(3);
            }
            int GetPinValue()
            {
                int absolutePin = GPIO_BASE + PinNumber;
                string path = "/sys/class/gpio/gpio" + to_string(absolutePin) + "/value";
                string PinValue = readFromFile(path);
                int value = stoi(PinValue);
                return value;

            }
            // deconstructor
            ~GpioPin()
            {
                if(PinNumber != -1) {
                    DeactivePin();
                }
            }
         };

         // same configurations
         vector<GpioPin> GPIO_InitPins(initializer_list<int> PinNumbers , int dir , int state)
         {
            vector<GpioPin> pins;
            pins.reserve(PinNumbers.size());

            for(auto PinNumber : PinNumbers)
            {
                cout<<" Intializition Pin " << PinNumber << endl;
                pins.emplace_back(PinNumber,dir,state);
            }
            return pins;
         }

        struct PinsConfig{
            int PinNumber;
            int PinState;
            int PinDir;
        };
         // different configuraionts
        vector<GpioPin> GPIO_InitPins(initializer_list<PinsConfig> configs){

            vector<GpioPin> pins;
            pins.reserve(configs.size());


            for(auto pincfg : configs )
            {
                cout<<"Initaialization Pin " << pincfg.PinNumber << " dir= "<<pincfg.PinDir << "  state="<<pincfg.PinState<<endl;
                pins.emplace_back(pincfg.PinNumber,pincfg.PinDir,pincfg.PinState);

            }
            return pins;
        }

    }


}


int main(void)
{
    cout << "Starting GPIO Program...\n";

    
    // MCAL::GPIO::GpioPin PIN0(27, MCAL::GPIO::PinOUT, MCAL::GPIO::PinHigh);
    
    // while (1)
    // {
    //     PIN0.Toggle_Pin();
    //     if(PIN0.GetPinValue() > 0)
    //     {
    //         cout << " PIN HIGH \n"; 
    //         // PIN0.SetPinVal(MCAL::GPIO::PinLow);
    //     }
    //     else{
    //         cout << " PIN LOW \n"; 
    //         // PIN0.SetPinVal(MCAL::GPIO::PinHigh);

    //     }
    // }


    // /** init multiple pins with same config */
    // auto output = MCAL::GPIO::GPIO_InitPins({24,23,29},MCAL::GPIO::PinOUT,MCAL::GPIO::PinHigh);
    // while (1)
    // {
    //     /* code */
    // }
     auto customPins = MCAL::GPIO::GPIO_InitPins({
        {17, MCAL::GPIO::PinIN,  MCAL::GPIO::PinLow},   
        {18, MCAL::GPIO::PinOUT, MCAL::GPIO::PinHigh},  
        {19, MCAL::GPIO::PinOUT, MCAL::GPIO::PinLow}    
        });
           return 0;
}            // PIN0.SetPinVal(MCAL::GPIO::PinLow);
