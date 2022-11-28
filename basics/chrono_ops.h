#pragma once
#include <gtest/gtest.h>

#include <chrono>
#include <ostream>
//#include <format> // gcc-12 not supported yet

namespace chrono_ops {

using namespace std::literals;

TEST(ChronoOps, ShowLocalDateTimeNow) {
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;

  // template< class CharT >
  // put_time( const std::tm* tmb, const CharT* fmt );
  //
  // When used in an expression `out << put_time(tmb, fmt)`, converts the date
  // and time information from a given calendar time tmb to a character string
  // according to format string fmt, as if by calling std::strftime,

  ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
  // example: 2022-11-24 22:08:43
  // std::cout << ss.str() << '\n';
}

TEST(ChronoOps, RatioMultiply) {
  using two_third = std::ratio<2, 3>;
  using one_sixth = std::ratio<1, 6>;
  using product = std::ratio_multiply<two_third, one_sixth>;
  EXPECT_EQ(1, product::num);
  EXPECT_EQ(9, product::den);
}

TEST(ChronoOps, DurationConversion) {
  auto twelve_mins = std::chrono::minutes(12);
  EXPECT_EQ(
      720,
      std::chrono::duration_cast<std::chrono::seconds>(twelve_mins).count());
  // minutes -> hours will be truncate to 0!
  EXPECT_EQ(
      0, std::chrono::duration_cast<std::chrono::hours>(twelve_mins).count());
  // to make it work, need to do something like:
  using sec_to_hour = std::ratio<3600, 1>;
  using sec_to_hour_duration = std::chrono::duration<double, sec_to_hour>;
  EXPECT_EQ(
      0.2,
      std::chrono::duration_cast<sec_to_hour_duration>(twelve_mins).count());
}

std::string secToHHMMSS(std::chrono::seconds secs) {
  using namespace std::chrono;
  auto h = duration_cast<hours>(secs);
  secs -= h;
  auto m = duration_cast<minutes>(secs);
  secs -= m;
  auto t2Str = [](auto tVal, auto unit) {
    return (tVal < 10 * unit ? "0" : "") + std::to_string(tVal / unit);
  };
  std::ostringstream os;
  os << t2Str(h, 1h) << ':' << t2Str(m, 1min) + ':' << t2Str(secs, 1s);
  return os.str();
}

TEST(ChronoOps, Hhmmss) {
  const auto sec_90 = std::chrono::seconds(90);
  EXPECT_EQ("00:01:30", secToHHMMSS(sec_90));
  EXPECT_EQ("00:00:17", secToHHMMSS(std::chrono::seconds(35) / 2));
  // C++20, gcc-12 not supported std::chrono::operator<<(std::chrono::hh_mm_ss)
  //os << std::chrono::hh_mm_ss<std::chrono::seconds>{sec_90};
  //EXPECT_EQ("00:01:30", os.str());
}

}  // namespace chrono_ops