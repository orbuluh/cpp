struct S {
  S() { puts("S()"); }
  S(const S&) { puts("S(const S&)"); }
  S(S&&) noexcept { puts("S(S&&)"); }
  ~S() { puts("~S()"); }
  S& operator=(const S&) {
    puts("operator=(const S &)");
    return *this;
  }
  S& operator=(S&&) noexcept {
    puts("operator=(S&&)");
    return *this;
  }
};