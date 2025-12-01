Here is a comprehensive `README.md` file documenting your project. It covers the project description, the OOP design principles used in your code, the cross-compilation process, and deployment steps.

You can save this file as **`README.md`** inside your `CPP_TASK3` folder.

***

```markdown
# C++ OOP GPIO Control for Raspberry Pi 3 (Cross-Compiled)

This project implements a C++ Object-Oriented wrapper for the Linux `sysfs` GPIO interface. It allows for controlling Raspberry Pi 3 GPIO pins using high-level abstractions (classes, vectors, and move semantics).

 The project is cross-compiled on a host Linux machine using a custom **Crosstool-NG** toolchain targeting **AArch64** (ARM64).

## 1. Project Overview

### Features
*   **OOP Design:** Encapsulates GPIO logic within the `MCAL::GPIO::GpioPin` class.
*   **RAII Principle:** 
    *   **Constructor:** Automatically "exports" the pin via sysfs.
    *   **Destructor:** Automatically "unexports" the pin to free resources when the object goes out of scope.
*   **Move Semantics:** Implements Move Constructors and Move Assignment operators (Rule of 5) to safely manage file descriptors and prevent double-freeing resources. Copying is disabled to ensure unique ownership of a physical pin.
*   **Bulk Initialization:** Supports initializing multiple pins with uniform or distinct configurations using `std::vector` and `std::initializer_list`.

### Hardware Configuration
*   **Target:** Raspberry Pi 3 (Running 64-bit Linux).
*   **GPIO Base:** The code is configured with `GPIO_BASE = 512` (Common for Ubuntu Server on RPi). 
    *   *Note: If using standard Raspberry Pi OS, you may need to change this to `0` or `399` in `gpio.cpp`.*

## 2. Environment Setup

Before compiling, ensure your custom toolchain is added to your system `PATH`.

**Toolchain Path:** `~/x-tools/aarch64-rpi3-linux-gnu/bin`

Run this command in your terminal:
```bash
export PATH=$PATH:~/x-tools/aarch64-rpi3-linux-gnu/bin
```

To verify the compiler is accessible:
```bash
aarch64-rpi3-linux-gnu-g++ --version
```

## 3. Compilation Steps

### Step 1: Sanity Check (Optional)
Compile a simple "Hello World" program to ensure the toolchain works.

1.  Create `demo.cpp` (if not exists).
2.  Compile:
    ```bash
    aarch64-rpi3-linux-gnu-g++ -o demo_app demo.cpp
    ```

### Step 2: Compile the GPIO Application
Compile the main `gpio.cpp` file.

```bash
aarch64-rpi3-linux-gnu-g++ -o gpio_app gpio.cpp
```

*   **-o gpio_app**: The name of the resulting executable.
*   **gpio.cpp**: Your source code file.

### Step 3: Verify Architecture
Ensure the binary is built for ARM64 and not the host x86 machine.

```bash
file gpio_app
```
**Expected Output:** `ELF 64-bit LSB executable, ARM aarch64...`

## 4. Deployment

Transfer the compiled executable to your Raspberry Pi using `scp`.

```bash
scp gpio_app <username>@<pi_ip_address>:/home/<username>/
```
*   Replace `<username>` with your RPi username (e.g., `pi` or `ubuntu`).
*   Replace `<pi_ip_address>` with the RPi's IP address (e.g., `192.168.1.15`).

## 5. Execution on Raspberry Pi

1.  **SSH into the Pi:**
    ```bash
    ssh <username>@<pi_ip_address>
    ```

2.  **Navigate to the directory:**
    ```bash
    cd /home/<username>/
    ```

3.  **Run the application:**
    Since the code accesses hardware `/sys/class/gpio`, you usually need `root` privileges.

    ```bash
    sudo ./gpio_app
    ```

## 6. Code Usage Example

The `main` function demonstrates initializing a list of custom pins:

```cpp
// Initialize Pin 17 (IN), Pin 18 (OUT/HIGH), Pin 19 (OUT/LOW)
auto customPins = MCAL::GPIO::GPIO_InitPins({
    {17, MCAL::GPIO::PinIN,  MCAL::GPIO::PinLow},   
    {18, MCAL::GPIO::PinOUT, MCAL::GPIO::PinHigh},  
    {19, MCAL::GPIO::PinOUT, MCAL::GPIO::PinLow}    
});
```

When `customPins` goes out of scope (end of program), the destructors will automatically unexport these pins from the system.
```
