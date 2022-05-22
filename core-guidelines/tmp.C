



class Immutable_string { // Short-string optimization
    // Slightly larger than the size of a pointer (so we can use union)
  static constexpr size_t buffer_size = 16;
  public:
    Immutable_string(const char* str) : size(strlen(str)) {
        if (size < buffer_size)
            strcpy_s(string_buffer, buffer_size, str);
        else {
            string_ptr = new char[size + 1];
            strcpy_s(string_ptr, size + 1, str);
        }
    }

    ~Immutable_string() {
        if (size >= buffer_size)
            delete[] string_ptr;
    }

    const char* get_str() const {
        return (size < buffer_size) ? string_buffer : string_ptr;
    }

  private:
    // If the string is short enough, we store the string itself
    // instead of a pointer to the string.
    union {
        char* string_ptr;
        char string_buffer[buffer_size];
    };

    const size_t size;
};