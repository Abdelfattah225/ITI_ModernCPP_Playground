# GPIO Driver Migration to C++

### **MCAL – STM32F401CC | GPIO Driver Conversion (C → C++)**

## 📌 **Overview**

This task focuses on converting the **GPIO driver for STM32F401CC** from **C** to **C++**.
The main goal is to reorganize the driver into a **C++-friendly structure** using:

* `namespace`
* `extern "C"` compatibility
* Better code organization
* Maintaining full backward compatibility with C-style MCU programming

The task involved editing multiple files:

* `GPIO_Interface.h`
* `GPIO_Private.h`
* `GPIO_Cfg.h`
* `GPIO.c` (renamed logically to `GPIO.cpp` after conversion)

---

## 🎯 **Task Objectives**

1. **Encapsulate the entire GPIO driver inside C++ namespaces:**

   ```cpp
   namespace MCAL {
       namespace GPIO {
           // driver code
       }
   }
   ```

2. Ensure **mixed C/C++ compilation** by using:

   ```cpp
   #ifdef __cplusplus
   extern "C" {
   }
   #endif
   ```

   when needed for C-based headers.

3. Convert all driver source files to support C++ while keeping:

   * Same register access logic
   * Same structures (GPIO_PinConfig_t)
   * No changes to hardware behavior

4. Update all configuration files to match C++ formatting.

---

## 🧩 **What Was Modified**

### **1️⃣ GPIO_Interface.h**

* Wrapped declarations inside:

  ```cpp
  #ifdef __cplusplus
  namespace MCAL {
      namespace GPIO {
  #endif
  ```
* Ensured the prototypes remain compatible with both C & C++.

---

### **2️⃣ GPIO_Private.h**

* No functional change, but covered inside namespaces.
* Added `__cplusplus` guards.

---

### **3️⃣ GPIO_Cfg.h**

* Fully rewritten into C++-style organization.
* Grouped configuration macros (LED, BUTTON, USART1, SPI1, I2C1).
* Wrapped file inside:

  ```cpp
  #ifdef __cplusplus
  namespace MCAL {
      namespace GPIO {
  #endif
  ```

---

### **4️⃣ GPIO.c → GPIO.cpp**

* Entire implementation wrapped inside the MCAL::GPIO namespace.
* No behavioral changes in register-level logic.
* Preserved STM32 register manipulation style.
* Functions updated:

  * `GPIO_PinInit`
  * `GPIO_PortInit`
  * `GPIO_ReadPin`
  * `GPIO_ReadPort`
  * `GPIO_WritePin`
  * `GPIO_WritePort`
  * `GPIO_TogglePin`
  * `GPIO_SetPin`
  * `GPIO_ResetPin`
  * `GPIO_ClockEnable`

Everything follows the same hardware logic, just safer for C++ compilation.

---

## ⚙️ **How to Compile**

Use **g++**, not gcc:

```bash
g++ -c GPIO.cpp -o GPIO.o
```

Or with the whole project:

```bash
g++ *.cpp -o firmware.elf
```

gcc ❌ **will not compile** because namespaces are C++-only.

---

## 🧪 **Testing**

* Successfully compiled in C++ environment.
* No register-level modifications.
* Checked for:

  * Alternate function writing
  * MODER/PUPDR/OSPEEDR updates
  * ODR/IDR reading/writing
  * Atomic set/reset using BSRR

All behavior matches the C version exactly.

---

## ✅ **Final Result**

You now have a **fully working C++ version of the STM32 GPIO driver**, organized under:

```
MCAL::GPIO
```

This makes the driver scalable, readable, and maintainable, while still preserving hardware-level performance.

---


