/*modified from https://github.com/yuki-koyama/hello-tbb-cmake*/

#include <tbb/tbb.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <random>

int main() {
  std::random_device seed;
  std::default_random_engine engine(seed());
  std::uniform_real_distribution<double> uniform_dist(0.0, 10.0);

  constexpr int num_elements = 1000000;
  std::vector<double> x_optimal(num_elements);

  auto perform_gradient_descent = [&](int i) {
    constexpr double epsilon = 1e-15;
    constexpr double rate = 0.99;

    double x = uniform_dist(engine);
    double a = 0.5;
    while (true) {
      const double delta = a * std::cos(x);
      x -= delta;
      a *= rate;
      if (delta < epsilon) {
        break;
      }
    }
    x_optimal[i] = x;
  };

  const auto t_0 = std::chrono::system_clock::now();

  tbb::parallel_for(0, num_elements, perform_gradient_descent);

  const auto t_1 = std::chrono::system_clock::now();

  for (int i = 0; i < num_elements; ++i) {
    perform_gradient_descent(i);
  }

  const auto t_2 = std::chrono::system_clock::now();

  const auto elapsed_time_parallel =
      std::chrono::duration_cast<std::chrono::milliseconds>(t_1 - t_0).count();
  const auto elapsed_time_serial =
      std::chrono::duration_cast<std::chrono::milliseconds>(t_2 - t_1).count();

  std::cout << "parallel: " << elapsed_time_parallel << " [ms]\n";
  std::cout << "serial:   " << elapsed_time_serial << " [ms]\n";

  return 0;
}
