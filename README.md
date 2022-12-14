  ## About The Project

Apollo is an experimental operating system for the Amiga 4000 computer with support for user and kernel space concurrency and reentracy.

One aspect that sets it aside from traditional threading-based OSs is that it is purely built around dispatch queues similar to Apple's Grand Central Dispatch rather than threads. So there is no support for creating threads in user space nor in kernel space. Instead the kernel implements a virtual processor concept and it maintains a pool of virtual processors. The size of the pool is dynamically and automatically adjusted based on the needs of the dispatch queues. All kernel and user space concurrency is achieved by creating dispatch queues and by submitting work items to a dispatch queue. Work items are from the viewpoint of the user just closures (function callbacks plus state).

Another interesting aspect is interrupt handling. Code which wants to react to an interrupt registers either a binary or a counting semaphore with the interrupt controller for the interrupt it wants to handle. The interrupt system will then signal the semaphore when an interrupt occures. You would use a counting semaphore if it is important that you do not miss any interrupt occurences and a binary semaphore when you only care about the fact that at least one interrupt has occured. The advantage of translating interrupts into signals on a semaphore is that the interrupt handling code executes in a well-defined context that is the same kind of context that any other kind of code runs in. It also gives the interrupt handling code more flexibility since it does not have to immediately react to an interrupt. The information that an interrupt has happened is never lost whether the interrupt handler code happened to be busy with other things at the time the interrupt occured or not.

The kernel itself is fully reentrant and supports permanent concurrency. This means that virtual processors continue to be scheduled and context switched even while the CPU is executing inside kernel space. There is also a full set of (counting/binary) semaphores, condition variables and locks available inside the kernel. The API of those objects closely resembles what you would find in a user space implementation of a typical OS.

There are a number of (unit) tests. However you currently have to manually invoke them because there's no automated unit testing framework yet. But then - manual testing is better than no testing.

Finally there is a set of fundamental routines for manipulating byte and bit ranges, doing 32bit and 64bit arithmetic and general support for the C programming language.

The following kernel services are implemented at this time:

* Kernel and user space separation in the sense of code privilige separation (not memory space separation)
* Dispatch queues with execution priorities
* Virtual processors
* Simple memory management (no virtual memory support)
* Floppy disk driver
* Monotonic clock
* Repeating timers
* Binary and counting semaphores, condition variables and locks (mutexes)
* Expansion board detection and enumeration
* Event driver with support for keyboard, mouse, digital Joystick, analog joystock (paddles) and light pens
* Simple graphics driver (not taking advantage of the Blitter yet)
* Console driver
* Beginnings of a syscall interface
* Basic 32bit and 64bit math routines
* Interrupt handling

Note that there is no support for a file system or shell at this time. Also the level of completness and correctness varies substantially. Things are generically planned to improve over time :)

## Getting Started

Setting the project up for development and running the OS is a bit involved. The instructions below are for macOS. It should be relatively straight forward to make development work on Linux and Windows. However I haven't spent any time looking into this.

### Prerequisites

The first thing you will need is an Amiga computer emulator. I'm using FS-UAE which you can find at [https://fs-uae.net/download#macosx](https://fs-uae.net/download#macosx)

Download the FS-UAE and FS-UAE-Launcher apps and place them at `Emulators/Amiga` inside the `Applications` folder inside your home directory. The FS-UAE application should be at this file system location:

```sh
   $HOME/Applications/Emulators/Amiga/FS-UAE.app
   ```

Next download and install the C compiler and assembler that you need to build the operating system. You can find the compiler and assembler at [http://www.compilers.de/vbcc.html](http://www.compilers.de/vbcc.html)

The version that I'm using for my development and that I know works correctly on macOS 12.6 is 0.9f.

Next make sure that you have Python 3 installed and that it can be invoked from the command line with the "python" command.

Create a `SDK` folder inside the `Emulators/Amiga` folder and then place the `vbcc` folder inside of that. The final result should look like this:

```sh
   $HOME/Applications/Emulators/Amiga/SDK/vbcc
   ```

Check out the Apollo code and place it inside a `Developer` folder inside your home directory:

```sh
   git clone https://github.com/dplanitzer/Apollo.git
   ```

The final result should look like this:

```sh
   $HOME/Developer/Apollo
   ```
   
The reason for this is that the FS-UAE configuration files contain a home directory relative path to the boot ROM generated by the make file.

### Building Apollo

Open the Xcode project inside the `Apollo/Kernel` folder and select `Product/Run` from the menubar. This should build Apollo and then run it inside of the FS-UAE Amiga emulator.

At least in theory. In actual practice Xcode has a strong tendency to produce bogus error messages when you try to build the project inside of it. It seems that newer Xcode versions are getting worse in that respect.

If Xcode refuses to build the project because of some obscure error then build and run it from the command line like this:

1. Open Terminal.app and cd inside the Sources folder:
```sh
   cd Apollo/Kernel/Sources
   ```
2. Use make to build Apollo:
```sh
   make
   ```
The make file creates a `build` folder inside the sources directory where it places all object files and the final boot ROM file named `Boot.rom`.

3. Finally run Apollo inside the Amiga emulator with the help of the run.sh shell script:
```sh
   ../run.sh
   ```
   
The run.sh shell script will automatically fire up the FS-UAE Amiga emulator and configure it to emulate an A4000 machine.

It may be that if you invoke the makefile or run.sh script that it looks like as if nothing is happening. A likely reason for this effect is that the Gatekeeper process in macOS is blocking one of the tools that the makefile or run.sh is trying to invoke from running. You will have to give permission to macOS to run those tools. Go to the General tab in the Security & Privacy pane in the macOS System Preferences application to grant permission.

## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

## Contact

Dietmar Planitzer - [@linkedin](https://www.linkedin.com/in/dplanitzer)

Project link: [https://github.com/dplanitzer/Apollo](https://github.com/dplanitzer/Apollo)
