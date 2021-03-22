#include <algorithm>
#include <future>
#include <iostream>
#include <random>
#include <set>

std::set<int> make_sorted_random(const size_t num_elems) {
  std::set<int> retval;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, num_elems);

  // 随机生成num_elems个(包括重复)
  std::generate_n(std::inserter(retval, retval.end()), num_elems,
                  [&]() { return dis(gen); });

  // throw std::runtime_error("hello");
  std::cout << "random ok " << num_elems << '\n';

  return retval;
}

int main() {
  // future;
  auto f = std::async(make_sorted_random, 1000001);
  auto f1 = std::async(make_sorted_random, 1000002);

  try {
    f.get();
  } catch (std::exception &e) {
    std::cout << "The exception:" << e.what();
  }

  std::cout << f1.get().size() << '\n';
}