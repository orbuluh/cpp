# Debugging Linux C++ - Greg Law

## There are many tools, use them

Two main classes of debugging tool today:

:one: Checkers (static and dynamic analysis)

- "Did my code do bad thing x, y, z"?
- Examples: address sanitizer, Valgrind, Coverity

:two: Debuggers

- "What exactly did my code do?"
- Examples: GDB, LLDB, rr, UndoDB, Live Recorder

## GDB - more than you knew

- GDB may not be intuitive but it is very powerful
  - It is easy to use but not easy to learn

### GDB TUI (Test User Interface) top tips:
  - `ctrl-x-a`: toggle to/from TUI mode
  - `ctrl-l`: refresh the screen
  - `ctrl-p`: prev, next, commands
  - `ctrl-x-2`: second window, cycle through
### GDB has Python!

- GDB has full Python interpreter with access to standard modules
- The gdb python module gives most access to gdb:
  - `(gdb) python gdb.execute()` to do gdb commands
  - `(gdb) python gdb.parse_and_eval()` to get data from inferior
  - `(gdb) python help('gdb')` to see online help

### Python pretty printers

```python
class MyPrinter(object):
    def __init__(self, val):
        self.val = vall
    def to_string(self):
        return (self.val['member'])
import gdb.printing
pp = gdb.printing.RegexpCollectionPrettyPrinter('mystruct')
pp.add_printer('mystruct', '^mystruct$', MyPrinter)
gdb.printing.register_pretty_printer(gdb.current_objfile(), pp)
```

### In-built pretty printers for STL

- GDB will (try to) pretty print most STL container classes
- Note that this relies on Python pretty printers installed on the target system.
  - Compiling/linking with a different version of libstdc++ (e.g. executable built on a different host than the one being used to debug), then pretty printing might give strange results.
  - Generally, gdb will debug arbitrary binaries that you've made anywhere. If you start trying to debug things like the STL using its pretty-printers, you need to have used a similar, probably the same, distribution, to have compiled your program on which you are debugging.
  - If you start to take GDB and run that gdb binary on another distribution, oftentimes it will kind of work, and even the Python integration will appear to work, but then it will try to use some of the Python libraries and find that there's version mismatch between the GDB binary and the Python library.
- There are many (list with `info pretty-printers), including:
  - `std::string`, `std::bitset`, `std::list`, `std::multimap`, `std::queue`, `std::set`, `std::shared_ptr`, `std::stack`, `std::tuple`, `std::unique_ptr`, `std::vector`, `std::weak_ptr` and iterators.

### General advise on `.gdbinit`

- only contains below:

```bash
set history save on
set print pretty on
set pagination off
set confirm off
```

- Hint: try to have a project `gdbinit` with lots of stuff in it and source that. It's easy for weird stuff to happen.


## GDB is built on ptrace and signals

- GDB is built atop ptrace (an API from Linux kernel).
- When you are running the inferior program in GDB (or you've attached GDB to a running process), it's under the control of ptrace.
- And the way ptrace works is when the inferior process receives a signal, it is suspended/stops at that point. Control is returnned to the tracing process where the tracer gets notified (vis `waitpid`).
- In a GDB's context, GDB is the tracer. So when the inferior receives a signal, the inferior program stops and GDB gets control.
- Then GDB can decide what to do. It can just continue the program and throw the signal away. Or feed the signal in.
- Usually gdb returns to the prompt, but what it will do depends on the signal and how it is configured

```bash
gdb> info signals
```

- For example, when the inferior get a SIGHUP, it'll stop, control returns to GDB prompt, and it will say, got a SIGHUP, and you can press continue, and if you do, the SIGHUP will then be passed into the program. And if it has a handler, that handler will run. Or if it doesn't, then it will do whatever the default action for that signal is. Usually terminate, sometimes ignore, etc.
- But some of the signal don't. For example, SIGINT wil lbe treated with special rule. Say when you're in your debuggee, e.g. your inferior process are inside GDB and you've launched the program. And you hit Ctrl-C from GDB prompt itself to get control back - GDB itself isn't doing anything special with that, it will generate a SIGINT and deliver that to every process inside the process group of which the terminal is the controlling terminal. The normal thing then happens, the process stops, GDB gets the notification that SIGINT has arrived and it returns to the prompt. At this point, say, the "Pass to program" column in `info signals` is `No`, and you type continue, the SIGINT will NOT be passed into the program accordingly. So if your program you're debugging has a handler for SIGINT and relies on SIGINT being called, then you'll need to change that inside gdb configuration like:

```bash
gdb> handle SIGINT stop print pass
```

- SIGTRAP is similar to SIGINT - when you hit a breakpoint, it will generate a SIGTRAP, and what GDB will do, when it's a breakpoint, is it'll change the code. For x86, it will write the opcode to generate a trap (e.g. it's injecting an instruction). When the program run to that injected opcode, the programmer receives a SIGTRAP, GDB stops, and returns to prompt again. So by default, SIGTRAP also has a "Pass to program" being "No".


## Breakpoints and watchpoints

```bash
watch foo             # stop when foo is modified
watch -l foo          # watch address
rwatch                # stop when foo is read
watch foo thread 3    # stop when thread 3 modified foo
watch foo if foo > 10 # stop when foo > 10
```

- GDB is actually quite smart about what your are watching. If it's a local variable and it's out of scope, gdb will set a breakpoint and notify you that it's no long under watch.
- And if you are debugging compiled code, usually what you care about is watching an address - I've got some other pointer somewhere that's stamping on this or something. So with `-l`, you can do so. Fo this, when it goes out of scope, it won't be smart enough to not watching anymore.

## thread apply

```bash
thread apply 1-4 print $sp  # sp is stack pointer
thread apply all backtrace
thread apply all bactrace full
```

## Dynamic printf (dprintf)

- Use dprintf to put printf's in your code without recompiling:

```bash
dprintf some_fnc, "m is %p m-> magic is %u\n", m, m->magic
# this will trigger GDB so that when it hits the breakpoint
# on some_fnc, control will return to GDB, it's then running
# printf commands like calling those inside the inferior to do the
# printing you specified. Then the inferior got the control back,
# remove that breakpoint and continue. All of which is very slow
```

- This control how the printfs happen:

```bash
set dprintf-style gdb|call|agent
set dprintf-function fprintf
set dprintf-channel mylog
```

- Note that you still need to put the `dprintf` before the actual bug has happened. But at least you don't have to change your code and recompile your code to get more info printed out.


## Calling inferior functions

- `call foo()` will call foo in your inferior, but beware, print may well do too:

```bash
print foo()
print foo+bar #(if C++)
print errno # errno is a thread local and it's actually defined as a function
            # call, and GDB will just call that function when you call print

```

- And beware, below will call `strcpy()` and hence `malloc()`

```bash
call strycpy(buffer, "Hello\n")
```

## Catchpoints

- Catchpoints are like breakpoints but catch certain events, such as C++ exceptions

  - `catch catch` to stop when C++ exceptions are caught
  - `catch syscall nanosleep` to stop at nanosleep system call
  - `catch syscall 100` to stop at system call number 100

## Remote debugging

- Debug over serial/sockets to a remote server. GDB server will debug the inferior using ptrace
- Start: `gdbserver localhost:2000 ./a.out`
- Then connect from a gdb with `target remote localhost:2000`

## Multiprocess debugging

- Debug multiple 'inferiors' simultaneously - e.g. debug multi-process at the same time. It looks very similar to debug a multi-threaded app.
- Add new inferiors
- Follow fork/exec

```bash
set follow-fork-mode child|parent

# by default, gdb will detach on a fork from one of the parent of the child
# process, depending on what you set the follow-fork-mode to. But if you define
# detach-on-fork off, then it will continue to debug both the parent and the
# child process after a fork.
set detach-on-fork off

# once you set the above and both child and parent process are in GDB, you can
# basically list all the running process through this:
info inferiors

# then you can basically switch around the process you want to focus
inferior N
set follow-exec-mode new|same
add-inferior <count> <name>
remove-inferior N
clone-inferior
print $_inferior
```

## More python

```python
# You can also create your own commands:
class my_command(gdb.Command):
  def __init_(self):
    gdb.Command.__init__(self, 'my-command', gdb.COMMAND_NONE)
  def invoke(self, args, from_tty):
    do_bunch_of_python()

# or hook certain kinds of events
def stop_handler(ev):
  print('stop events')
  if isinstance(ev, gdb.SignalEvent):
    print('its a signal: ' + ev.stop_signal)
gdb.events.stop.connect(stop_handler)
```

## Other cool things

- `tbreak`: temporary breakpoint
- `rbreak`: reg-ex breakpoint
- `command`: list of commands to be executed when breakpoint hit
- `silent`: special command to suppress output on breakpoint hit
- `save breakpoints`: save a list of breakpoints to a script
- `save history`: save history of executed gdb commands
- `info line foo.c:42`: show program counter for line
- `info line * $pc`: show line begin/end for current program counter

## Common misunderstanding: gcc's `-g` and `-O` are orthogonal

- For example, `gcc -Og` is optimized but doesn't mess up debug.
- There will be absolutely no runtime performance impact. The only thing you'll use is a bit more disk. And if you are not using gdb, you won't even page in the debug info sections from disk.
- `-ggdb3` is better than `-g`

## Valgrind

- Valgrind is actually a platform, on which many different checkers are built
  - `memcheck` is the default and the most used/known
  - also, there is: `cachegrind`, `callgrind`, `helgrind`, `drd`, `massif`, `lackey`, `none`
- Can be a bit slow, but very easy to use. No need to relink or rebuild -- it generally just works
  - Can be as simple as `valgrind ./a.out`
- What it's doing is translating the machine code/binary as it runs in a sort of JIT fashion and doing analysis on that code. So it's not simulated, but it is (legited?)
  - For example, if you try to print an undefined value, the first thing you notice is there's a conditional jump, based on the uninitialized data. Cause printf is trying to turn your number into a string. So this is easily captured by valgrind accordingly.
- Can be used in conjunction with gdb
  - `valgrind -vgdb=full --vgdb-error 0 ./a.out`
  - What's the implication? Remember we mention what valgrind do is translating your binary machine code. The code you are executing is under valgrind. The code the CPU is executing is functionally identical to the original program. It's just got extra stuff in it. But still, it's different code. So if you try to debug it through gdb in the normal way, you'll just see nonsense. Because once GDB tries to look through ptrace, what it'll see is what the CPU sees, which is not what it was expecting to see. But valgrind has built into it, a GDB server, which you can connect to and then you can start to do all the GDBs.
  - The `--vgdb-error 0` means stops after zero error, which is gonna stops at the beginning

- Note, you can't combine valgrind with any kind of reversible debugging. You can only do this with AddressSanitizer etc.

## Valgrind tools

- Memcheck: the default tool when you run valgrind
- Cachegrind: cache profiler, simulates l1, D1, L2 caches
- Callgrind: like cachegrind, but also with call-graphs
- Massif: heap profiler
- Helgrind: find race conditions in multithreaded programs
- DRD: Data race detector, like Helgrind, but uses less memory
- Lackery / None: demo/unit test of valgrind itself

## Sanitizer

- AddressSanitizer, MemorySanitizer, ThreadSanitizer, LeakSanitizer
- Overall sanitizers are kind of like valgrind but different, so unlike valgrind which will work on an unmodified program, the sanitizer is built into the compiler. Originally came in clang, and has been available in GCC as well. It's much faster than valgrind.
- Requires a special build of your program

```bash
clang -g -fsanitize=address foo.c
gcc -g -fsanitize=address -static-libasan foo.c
```

- `-static-libsasan`:
  - instructs the linker to statically link the AddressSanitizer (ASan) runtime library (`libasan.a`) into the executable or shared library being built.
  - By statically linking `libasan.a` into the executable, it becomes self-contained and doesn't rely on any dynamic libraries to be present on the system where the executable is being run. This ensures that the ASan runtime is always available and avoids any issues with library versioning or availability.

```bash
$ cat sanitizer_demo.c 
#include <cstdlib>

int main(int argc, char* argv[]) {
    int array[4];
    array[std::atoi(argv[1])]= 1;
    return 0;
}

$ g++ -g -fsanitize=address -static-libasan sanitizer_demo.c
$ ./a.out 1
$ ./a.out 4 # check the result!
```

- Unlike valgrind, this isn't doing the JIT binary translation stuff, this is really relying on the hardware - you can combine this with other debugging tools just fine. Particularly with reverse debugging, which is something just works.


## Reversible debugging

- GDB inbuilt reversible debugging: works well, but is very slow
- GDB in-build 'record btrace': Uses Intel brance trace
  - Not really reversible, no data
  - Also quite slow
- `rr`: very good at what it does, through somewhat limited features/platform support

- What reverse debugging? some demos around 44:10 -> 50:55 ... few key words used in demo as follows
- Tricks to use gdb to keep rerunning some program:

```bash
gdb) b main #put breakpoint on main
gdb) b _exit #put breakpoint on exit
gdb) command 1 # every time it hit breakpoint 1
gdb) # then type the command when it hit breakpoint 1 like ...
gdb) record
gdb) continue # or some other command, here we continue when hitting main
gdb) end # end of the command (s) triggered at breakpoint 1
gdb) command 2
gdb) run # so basically run again when exit as breakpoint 2 is at _exit
gdb) end # end of the command (s) triggered at breakpoint 2
gdb) set confirm off # so it won't keep asking you
gdb) r # run the executable, now it will keep rerunning

#.... when if finally cores, try:
gdb) p $pc #print program counter
gdb) x program_counter_address_from_above # exam the program counter pointer
gdb) reverse-stepi # go back 1 instruction before coring
gdb) disas # disassemble a specified section of code
gdb) p $sp # pop stack pointer
gdb) x stack_pointer_address_from_above # exam the stack pointer
gdb) watch * (long*) stack_pointer_address_from_above # watch the address
gdb) rc # run continue, which steps into the cored instruction, and should give
        # you some info why it cored
gdb) ptype local_array # print type of a variable local_array to further debug
```

- trick 2: using `rr` command (record and replay)

```bash
while rr record ./program_that_will_undeterministically_failed; done
# then when it cores, you should be able to some line like:
# rr: Saving execution to trace directory `/path_to_the_trace_file'.
# then you can do:
rr replay /path_to_the_trace_file
# then it launches gdb, and you should be able to do similar stuff as above
```

## `ftrace`

- `ftrance` means Function tracer: a fast way to trace various kernel functions
- This is one of part profiler part debugger tools
- Lots of pre-defined event (i.e. trace-points)
- Controlled through `/sys/kernel/debug/tracing`
- Or use the trace-cmd utility
- It allows users to trace function calls and other events in the kernel code.
  - `ftrace` works by hooking into the Linux kernel's function tracing infrastructure, which is built into the kernel itself.
  - The function tracing infrastructure records information about each function call as it occurs, including the function name, the time it was called, and the values of any arguments or return values.
  - Using the `ftrace` command, users can specify which functions they want to trace and how they want the trace information to be displayed. The tool allows users to filter the trace information based on various criteria, such as function name, process ID, or CPU core.
- some example commands in the slide

```bash
less /sys/kernel/debug/tracing/available_events
trace-cmd -e tcp:tcp_destroy_sock
```

- Some commands in the script of @52:50

```bash
echo 0 > $TRACING_DIR/tracing_on

# Clear out any old traces
echo 0 > $TRACING_DIR/trace

# Set some ftrace configuration values
echo nop > $TRACING_DIR/current_tracer
echo 0 > $TRACING_DIR/options/event-fork
echo 1 > $TRACING_DIR/events/signal/enable
echo "sig>=0" > $TRACING_DIR/events/signal/filter
echo 15000 > $TRACING_DIR/buffer_size_kb

# Run the target application and capture it's PID
./executable &
PID=$!
echo "PID is $PID"

# Give the process enough time to spin up the various components
sleep 2

# Get all the currently active PIDs, hopefully including the one that
# is killing the aux thread, and configure ftrace to record their signals
for pid in `ps aux | awk '{print $2}' | tail -n+2`; do echo $pid >> $TRACING_DIR/set_event_pid; done

# Finally turn on the trace to start recording kernel events
echo 1 > $TRACING_DIR/tracing_on
echo "recording started" > $TRACING_DIR/trace_marker

# Capture context for PID values
ps xao pid,ppid,pgid,sid,stat,comm,args | grep -i process_ke

## ...
```

## `strace`

- Trace all the system calls of a process
- `strace` also built on top of `ptrace` - `ptrace` is the process gets interrupted every time there's a signal. You can also configure `ptrace` to interrupt, to return when there's a system call, and it will output all the system calls being issued by the command.

```bash
strace cmd # print all system calls issued by cmd
strace -k cmd # show backtrace for each syscall  <- SUPER USEFUL!!!!
strace -t cmd # prefix each syscall with wallclock time
strace -o file cmd # write traced syscalls to file
strace -D cmd # strace process runs as detached grandchild of cmd
strace -f cmd # follow forks
strace -ff -o f cmd # follow forks, write output to f.%pid
strace -p 1234 # strace process-id 1234 (note: only the thread, not all threads in the process)
strace -p 1234 -f # attach to all threads in process-group 1234
```

## `ltrace`

- Trace all the dynamic library calls of a process

```bash
ltrace cmd # print all library calls issued by cmd
ltrace -w=4 cmd # show backtrace (up to 4 frames, but only if ltrace compiled with libunwind)
ltrace -t cmd # prefix each library call with wallclock time
ltrace -l libc.so* c # trace calls to libc library only
ltrace -e malloc+free-@libc.so* cmd # only trace malloc and free symbols from libc
```

## `perf trace`

- Like `strace`, but it's faster and more stable.
  - (`strace` will slow down the process quite a lot particularly when there are a lot of system calls)
- Worse than `strace` as it needs privileges, doesn't do as much decoding (e.g. string looks like pointer)

```bash
perf trace # trace everything - every syscall by every process
perf trace cmd # trace all syscalls issued by cmd
perf trace -p 1234 # trace all syscalls from process 1234
perf trace -e read* # trace all syscalls that start with read (e.g. read, readlink, readdir, ...etc)
perf trace -D 500 # wait 500 ms before tracing (skip all the startup gumf)
perf trace record # record into perf.data (this is really for profiling)
```

## Compiled with `-D_FORTIFY_SOURCE=1`

- Do bounds checking where they can, for example, when `memcpy`, `strcpy` ..etc
- Very low (typically impoosible to measure) peformance overheads
