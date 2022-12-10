# CPU caches

> from [here](http://www.nic.uoregon.edu/~khuck/ts/acumem-report/manual_html/ch03s02.html)

- The allocation of space in the cache is managed by the processor.
- When the processor accesses a part of memory that is **not already in the cache it loads a chunk of the memory around the accessed address into the cache, hoping that it will soon be used again.**
- The chunks of memory handled by the cache are called **cache line**s.
- The size of these chunks is called the **cache line size**.
- **Common cache line sizes are 32, 64 and 128 bytes.**
- A cache can only hold **a limited number of lines, determined by the cache size.** For example, a 64 kilobyte cache with 64-byte lines has 1024 cache lines.


> from [https://www.makeuseof.com/](https://www.makeuseof.com/tag/what-is-cpu-cache/)

Your computer has multiple types of memory inside it.

- There is **primary storage**, like a hard disk or SSD, which stores the bulk of the data—the operating system and programs.
- Next up, we have **Random Access Memory,** commonly known as the RAM.
  - This is much faster than the primary storage but is **only a short-term storage medium.** Your computer and the programs on it use RAM to store frequently accessed data, helping to keep actions on your computer nice and fast.
- Lastly, the CPU has even faster memory units **within itself**, known as the CPU memory cache.
  - It is also the closest to where the central processing occurs, being a part of the CPU itself.
  - Cache memory is a form of Static RAM (SRAM), while your regular system RAM is known as Dynamic RAM (DRAM). Static RAM can **hold data without needing to be constantly refreshed,** unlike DRAM, which makes SRAM ideal for cache memory.

How Does CPU Cache Work?

- Programs and apps on your computer are designed as **a set of instructions that the CPU interprets and runs.**
- When you run a program, the instructions make their way **from the primary storage (your hard drive) to the CPU**. This is where the memory hierarchy comes into play.
- The data **first gets loaded up into the RAM and is then sent to the CPU**.
- CPUs these days are capable of carrying out a gigantic number of instructions per second. To make full use of its power, the CPU needs access to super-fast memory, which is where the CPU cache comes in.
- The memory controller **takes the data from the RAM and sends it to the CPU cache.** Depending on your CPU, the controller is found on the CPU, or the Northbridge chipset found on your motherboard.
- The **memory cache then carries out the back and forth of data within the CPU**.

Memory hierarchy exists within the CPU cache, too.

-  CPU Cache memory is divided into three "levels": L1, L2, and L3
-  The memory hierarchy is again according to the **speed and, thus, the size of the cache.**

**L1 Cache**

- L1 (Level 1) cache is the fastest memory that is present in a computer system.
- In terms of priority of access, the L1 cache has the data the CPU is most likely to need while completing a certain task.
- The size of the L1 cache depends on the CPU. Some top-end consumer CPUs now feature a **1MB L1 cache**, like the Intel i9-9980XE, but these cost a huge amount of money and are still few and far between. Some server chipsets, like Intel's Xeon range, also feature a **1-2MB L1 memory cache**.
- There is no "standard" L1 cache size, so you must check the CPU specs to determine the exact L1 memory cache size.

The L1 cache is usually split into two sections: the **instruction cache** and the **data cache**.

- The instruction cache deals with the information about **the operation that the CPU must perform**, while
- the data cache holds **the data on which the operation is to be performed.**


**L2 Cache**

- L2 (Level 2) cache is slower than the L1 cache but bigger in size.
- Where an **L1 cache may measure in kilobytes, modern L2 memory caches measure in megabytes.** For example, AMD's highly rated Ryzen 5 5600X has a **384KB L1 cache and a 3MB L2 cache (plus a 32MB L3 cache)**.
- The L2 cache size varies depending on the CPU, but its size is **typically between 256KB to 8MB**.
- Most modern CPUs will pack more than a 256KB L2 cache, and this size is now considered small. Furthermore, some of the most powerful modern CPUs have a larger L2 memory cache, exceeding 8MB.
- When it comes to speed, the L2 cache lags behind the L1 cache but is still much faster than your system RAM.
- **The L1 memory cache is typically 100 times faster than your RAM, while the L2 cache is around 25 times faster.**

**L3 Cache**

- The L3 cache in your CPU can be massive, with top-end consumer CPUs featuring L3 caches up to **32MB**.
- Some server CPU L3 caches can exceed this, featuring up to **64MB**.
- The L3 cache is the **largest but also the slowest cache memory unit**. Modern CPUs include the **L3 cache on the CPU itself**. But while the **L1 and L2 cache exist for each core on the chip itself**, the L3 cache is more akin to a general memory pool that the **entire chip can make use of.**

**How does CPU cache memory work?**

- In its most basic terms, the data flows from the RAM to the L3 cache, then the L2, and finally L1.
- When the processor is looking for data to carry out an operation, it first tries to find it in the L1 cache. **If the CPU finds it, the condition is called a cache hit**.
- It then proceeds to find it in L2 and then L3.
- If the CPU doesn't find the data in any of the memory caches, it attempts to access it from your system memory (RAM). When that happens, it is known as a **cache miss.**
- Now, as we know, the cache is designed to speed up the back and forth of information between the main memory and the CPU. **The time needed to access data from memory is called "latency."**
  - L1 cache memory has the lowest latency, being the fastest and closest to the core, and L3 has the highest.
  - Memory **cache latency increases when there is a cache miss as the CPU has to retrieve the data from the system memory.**
