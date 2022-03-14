
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
