#pragma once

#include "SimpleSharedPtr.h"
#include <gtest/gtest.h>
#include <memory>

namespace smart_pointer_ops {

TEST(SimpleSharedPtr, Copy) {
  playground::SimpleSharedPtr<int> a{new int{10}};
  auto b{a};
  playground::SimpleSharedPtr<int> c{nullptr};
  c = b;
}

TEST(SimpleSharedPtr, Move) {
  playground::SimpleSharedPtr<int> a{new int{10}};
  auto b{std::move(a)};
  playground::SimpleSharedPtr<int> c{nullptr};
  c = std::move(b);
}

}  // namespace smart_pointer_ops