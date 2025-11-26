
# C++ FileActions Class: A Study in Resource Management

This project is an implementation of a C++ class `FileActions` designed to explore fundamental C++ concepts including constructors, copy constructors, destructors, references, and the complexities of manual resource management with raw pointers.

The primary goal is to demonstrate the behavior of resource sharing between objects and to highlight the common pitfalls, specifically the "double free" error, that arise from improper ownership models.

## Core Concepts Demonstrated

This program is a practical exercise in the following C++ features:
*   **Parameterized Constructor**: Initializes an object by acquiring a system resource (a file descriptor) and allocating dynamic memory (`new`).
*   **Copy Constructor**: Implements a **shallow copy** for a raw pointer, causing two objects to share the same dynamically allocated resource. This is central to the lesson of the task.
*   **Destructor**: Responsible for releasing acquired resources (`close()` the file descriptor and `delete` the pointer), demonstrating the RAII (Resource Acquisition Is Initialization) principle.
*   **Pointers and Dynamic Memory**: Manual memory management using `new` and `delete`.
*   **References to Pointers (`int*&`)**: Shows how to initialize reference members, which must be done in a member initializer list.
*   **`std::initializer_list`**: Provides a convenient, modern C++ syntax for passing a list of actions to the class.

## How to Build and Run

The program is contained within a single `main.cpp` file and can be compiled with a standard C++ compiler like g++.

1.  **Compile the code:**
    ```sh
    g++ main.cpp -o a.out
    ```
2.  **Run the executable:**
    ```sh
    ./a.out
    ```

## Expected Output and Analysis
abdelfattah@3zzazi:~/Modern_CPP/Tasks/CPP_TASK2$ g++ main.cpp 

abdelfattah@3zzazi:~/Modern_CPP/Tasks/CPP_TASK2$ ./a.out 

Simulating 'read' for 1024 bytes.
Simulating 'write' of 512 bytes.

Creating a copy of the object...

Copy constructor called. Sharing file descriptor.
Simulating 'read' for 1024 bytes.
Simulating 'write' of 512 bytes.
free(): double free detected in tcache 2
Aborted (core dumped)

When run, the program will execute successfully up until the point where the objects are destroyed. It will then crash with a "double free" error. This is the **expected and correct outcome** for this exercise.

```
File 'test_file.txt' opened with fd: 3
2 actions registered.

--- Executing actions for fd: 3 ---
  -> Simulating 'read' for 1024 bytes.
  -> Simulating 'write' of 512 bytes.
------------------------------------

Creating a copy of the object...

Copy constructor called. Sharing file descriptor: 3

--- Executing actions for fd: 3 ---
  -> Simulating 'read' for 1024 bytes.
  -> Simulating 'write' of 512 bytes.
------------------------------------

Main function is about to end. Destructors will be called now.
The program will likely crash because of a double-free error.
Destructor called for object with fd: 3. Cleaning up.
Destructor called for object with fd: 3. Cleaning up.
free(): double free detected in tcache 2
Aborted (core dumped)
```

### Analysis of the Crash

The crash is the main lesson of this program. Here is the sequence of events that leads to it:

1.  **`obj1` is created**: It allocates memory for a pointer `fd` and opens a file.
2.  **`obj2` is created**: The copy constructor is called. It copies the *address* stored in `obj1.fd` into `obj2.fd`. Now, both objects point to the **exact same memory location**. They are sharing the resource.
3.  **`main()` finishes**: Objects on the stack are destroyed in reverse order of creation. `obj2` is destroyed first.
4.  **`obj2`'s destructor runs**: It calls `close()` on the file descriptor and `delete` on the pointer `fd`. The resource is successfully freed.
5.  **`obj1`'s destructor runs**: It attempts to call `close()` on a file descriptor that is already closed and, more critically, it calls `delete` on the pointer `fd`, which points to memory that has **already been freed**.
6.  **Double Free Detected**: The C++ runtime detects an attempt to free the same memory twice and terminates the program with an error.

This exercise perfectly demonstrates why sharing raw pointers that manage resources is dangerous and leads to undefined behavior. The ownership of the resource is ambiguous: who is responsible for cleaning it up?

### The Modern C++ Solution

In a real-world application, this problem is solved elegantly using **smart pointers**. Specifically, `std::shared_ptr` is designed for this exact scenario of shared ownership. It uses reference counting to ensure the resource is only deleted when the *last* owner is destroyed, automatically preventing the double-free error.
