# F: Functions

## F.50: Use a lambda when a function won't do (to capture local variables, or to write a local function)
- Functions can't **capture local variables** or be **defined at local scope**; if you need those things, prefer a lambda where possible, and a handwritten function object where not.
- On the other hand, lambdas and function objects don't overload; **if you need to overload, prefer a function** (the workarounds to make lambdas overload are ornate).
- If either will work, prefer writing a function; use the simplest tool necessary.
```cpp
// writing a function that should only take an int or a string
// -- overloading is natural
void f(int);
void f(const string&);

// writing a function object that needs to capture local state and appear
// at statement or expression scope -- a lambda is natural
vector<work> v = lots_of_work();
for (int tasknum = 0; tasknum < max; ++tasknum) {
    pool.run([=, &v] {
        /*
        ...
        ... process 1 / max - th of v, the tasknum - th chunk
        ...
        */
    });
}
pool.join();
```
- Generic lambdas offer a concise way to write function templates and so can be useful even when a normal function template would do equally well with a little more syntax.
- This advantage will probably disappear in the future once all functions gain the ability to have Concept parameters.

## F.51: Where there is a choice, prefer default arguments over overloading
- Default arguments simply provide alternative interfaces to a single implementation. There is no guarantee that a set of overloaded functions all implement the same semantics. The use of default arguments can avoid code replication.
- For example:
```cpp
void print(const string& s, format f = {});

// over

void print(const string& s);  // use default format
void print(const string& s, format f);
```
- There is not a choice when a set of functions are used to do a semantically equivalent operation to a set of types. For example:
```cpp
void print(const char&);
void print(int);
void print(zstring);
```

## F.52: Prefer capturing by reference in lambdas that will be used locally, including passed to algorithms
- For efficiency and correctness, you nearly always want to capture by reference when using the lambda locally. This includes when writing or calling parallel algorithms that are local because they join before returning.
  - The efficiency consideration is that most types are cheaper to pass by reference than by value.
  - The correctness consideration is that many calls want to perform side effects on the original object at the call site. Passing by value prevents this.
```cpp
void send_packets(buffers& bufs) {
    stage encryptor([](buffer& b) { encrypt(b); });
    stage compressor([&](buffer& b) {
        compress(b);
        encryptor.process(b);
    });
    stage decorator([&](buffer& b) {
        decorate(b);
        compressor.process(b);
    });
    for (auto& b : bufs) {
        decorator.process(b);
    }
} // automatically blocks waiting for pipeline to finish
```
- Unfortunately, **there is no simple way to capture by reference to const** to get the efficiency for a local call but also prevent side effects.

## F.53: Avoid capturing by reference in lambdas that will be used non-locally, including returned, stored on the heap, or passed to another thread
- Pointers and references to locals shouldn't outlive their scope. Lambdas that capture by reference are just another place to store a reference to a local object, and shouldn't do so if they (or a copy) outlive the scope.
```cpp
// Bad!
int local = 42;

// Want a reference to local.
// Note, that after program exits this scope,
// local no longer exists, therefore
// process() call will have undefined behavior!
thread_pool.queue_work([&]{ process(local); });
```
```cpp
// Good
int local = 42;
// Want a copy of local.
// Since a copy of local is made, it will
// always be available for the call.
thread_pool.queue_work([=] { process(local); });
```