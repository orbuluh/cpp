# The stream state
- From [Ch 1.3](https://www.amazon.com/Standard-IOStreams-Locales-Programmers-Reference/dp/0321585585)
- Each stream has a state reflecting failure of operations and loss of integrity.
- The stream state is stored in a state variable, which is a private data member of the stream base class `basic_ios<class charT, class Traits>`.
- The data member is of type `iostate` which is a nested bitmask type in class `ios_base`, which also defines the corresponding bits.

| iostate flag	  | error category  |
|---	          |---	            |
| `goodbit`       | good            |
| `eofbit`        | An input operation reached the end of an input sequence. |
| `failbit`       | An output operation failed to generate the desired characters, or an input operation failed to read the expected character, or any other kind of operation failed to produce the desired result. |
| `badbit`        | Indicates the loss of integrity of the stream. |

# `eofbit`
- It is set when there is an attempt to read past the end of an input sequence.
- Typical situation:
  - Characterwise extraction:
    - once the last character is read, the stream is still in good state; `eofbit` is not set yet. Any subsequent extraction, however, will try to extract a character from a stream that does not have any more characters available.
    - This extraction will be considered an attempt to read past the end of the input sequence. Thus, `eofbit` will be set as a result of the subsequent extraction.
  - Extraction of an item other than a single character:
    - if you do not read characterwise, you might read past the end of the input sequence.
    - This is because the input operators read characters until they find a separator, or hit the end of the input sequence.
  - Assume input sequence contains `....12345<eof>`
      - The respective extractor will read all digit characters until it receives the information that it hit the end of the input.
      - Then the extraction will be terminated and `eofbit` will be set, because there was an attempt to read past the end of the input sequnce.
# `failbit`
- The flag is generally set as the result of an operation that fails.
- For example, if you try to extract an integer from an input sequence that contains nothing but whitespace, the extraction of an integer fails, because no integral value can be produced. As a result, the `failbit` is set.
- operations like `open()` in file streams can set the `failbit` upon failure of opening the requested file.

# The architecture of iostream
- From [Ch 2.0](https://www.amazon.com/Standard-IOStreams-Locales-Programmers-Reference/dp/0321585585)
- `iostream` has two layers, one for parsing and formatting and another for buffering, code conversion, and transport of characters to and from the external device.
- STREAM AND STREAM BUFFER CLASSES: related to each layer is a separate hierarchy of classes:
  - Classes that belong to the formatting layer are often referred to as the **stream** classes.
  - Classes of the transport layer are often referred to as the **stream buffer** classes.
- CHARACTER TYPE AND TRAITS TYPE: Almost all stream and stream buffer classes are class templates that take two template arguments, the character type and the traits type.
- iword/pword AND STREAM CALLBACKS:
  - iostreams provides a hook for adding information to a stream that can be used for arbitrary purposes.
  - This additional stream storage is also known as iword/pword, which are the names of the respective data members holding the information.
  - The stream callbacks are often needed for proper maintenance of such additional stream storage. Stream callbacks are also relevant for imbuing locales.

# Formatted I/O: The `>>` operator
- From [File I/O in C++](https://www.math.ucla.edu/~akrieger/teaching/18f/pic10b/examples/file.html)
- The stream extraction operator >> is used for “formatted input”.
- Assuming there hasn’t been an error with the `std::istream` here’s what operator `>>` does when reading into an int variable:
  - Skip over whitespace;
  - If the next non-space character is a digit, start reading an integer, up to the next non-digit;-
  - Else, if the next non-space character is not a digit, don’t read anything and set an error bit in the std::istream;
- The behavior is similar when reading into a double variable, or any of the other numeric types (`short`, `float`, etc.).
- When reading into a `std::string`, the `>>` operator still skips over whitespace, then reads the next word—that is, **everything up to the next whitespace character**.

