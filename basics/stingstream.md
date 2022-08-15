# [What is a stream exactly?](https://stackoverflow.com/a/25652115/4924135)
- The fundamental idea behind the metaphor of a "stream" is that it **provides or consumes data in a single-pass fashion**:
  - for example, for an input stream, data is produced precisely once.
  - You can ask the stream for some more data, and once the stream has given you the data, it will never give you that same data again.

- This is why in order to do anything meaningful with streams, **you will very usually want to attach a kind of buffer (a "stream buffer")** to the stream which stores some (usually small) amount of data that's been extracted from the stream in a random-access, inspectable and processable piece of memory. (There are similar, reversed ideas for output streams.)
  - Occasionally it makes sense to process streams without any buffering.
    - For example, if you have an input and an output stream and you read integers from the input and write the doubled value of each integer to the output, you can do that without buffering.

- So when thinking about ranges of data, **streams are things that you can traverse only once and never again.**
- Streams have another property, which is that **they may block**: an input stream may block when it has no data, and an output stream may block when it cannot accept any more data.
- That way, from within the program logic, you can imagine that a input stream always contains data until its end is reached, but a program relying on that may run for an arbitrary, unbounded amount of wall-clock time.

# Quick facts

- [ref](https://www.reddit.com/r/cpp_questions/comments/jq6pum/comment/gbkhr0t/?utm_source=share&utm_medium=web2x&context=3)
    - `istringstream` is an input stream and typically wraps some external source of text that you can extract with the >> operator. You cannot write to this stream.
    - `ostringstream` is an output stream, i.e. one that you can only write to.
    - `stringstream` is a stream that allows you to both read and write with the >> and << operators.
    - In general you should use the object that gives you the minimum permissions and functionality that is required to do the job.


# Using `istringstream` to load delimited data!
- `getline(istream& is, string& str, char delim)`
```
std::string input = "a|b|c";
std::istringstream iss(input);
std::string buf;
while (getline(iss, buf, '|')) {
    std::cout << buf << std::endl;
}
```
