# Quick fact
- Pimpl is actually a good illustration of the Bridge pattern.



# Pimpl
- The pimpl opaque pointer (opaque is opposite of transparent, i.e., you have no idea what’s behind it) acts as a bridge, gluing the members of a public interface with their underlying implementation that’s hidden away in the .cpp file.
- Pimpl allows us to preserve a clean, non-changing header file.
    - A larger proportion of the class implementation is actually hidden. If
    - Modifying the data members of the hidden Impl class does not affect binary compatibility.
    - The header file only needs to include the header files needed for the declaration, not the implementation.
- Pimpl helps to optimize the speed of compilation
    - compilers are getting more and more incremental, so that instead of rebuilding the entire translation unit, the compiler can actually only rebuild the definitions that have changed, and reuse the rest.