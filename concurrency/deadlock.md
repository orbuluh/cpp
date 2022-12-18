# Deadlock

> From [wiki](https://en.wikipedia.org/wiki/Deadlock)

Deadlock is any situation in which **no member of some group of entities can proceed** because **each waits for another member, including itself, to take action**

Deadlocks are a common problem in multiprocessing systems, parallel computing, and distributed systems, because **in these contexts systems often use software or hardware locks to arbitrate shared resources and implement process synchronization**


In an **operating system**

- a deadlock occurs when **a process or thread enters a waiting state because a requested system resource is held by another waiting process, which in turn is waiting for another resource held by another waiting process.**
- If a process remains indefinitely unable to change its state because resources requested by it are being used by another process that itself is waiting, then the system is said to be in a deadlock.

In a **communications system**

- deadlocks occur mainly due to loss or corruption of signals rather than contention for resources

## Coffman conditions

A deadlock situation on a resource can arise only if all of the following conditions occur simultaneously in a system:

**Mutual exclusion**:

- At least one **resource must be held in a non-shareable mode**
- that is, only one process at a time can use the resource. Otherwise, the processes would not be prevented from using the resource when necessary.
- Only one process can use the resource at any given instant of time.

**Hold and wait or resource holding:**

- a process is currently holding at least one resource and requesting additional resources which are being held by other processes.


**No preemption**

- a resource **can be released only voluntarily by the process holding it.**

**Circular wait**

- each process must be waiting for a resource which is being held by another process, which in turn is waiting for the first process to release the resource.
- In general, there is a set of waiting processes, P = {P1, P2, …, PN}, such that P1 is waiting for a resource held by P2, P2 is waiting for a resource held by P3 and so on until PN is waiting for a resource held by P1.
