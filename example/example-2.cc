#include <iostream>
#include <thread>
#include <vector>
#include "../src/Random.h"

int main(int argc, char **argv) {
  std::vector<std::thread> threads;

  for (int id = 0; id < 10; ++id) {
    threads.emplace_back([=](){
       auto rng = Random<random_t::SplitMix_64>::get_Rng();
       for (int i = 0; i < 10; i++) {
         auto a = rng.randuint64();
         printf("thread [%d] : %lx\n", id, a);
       }
    });
  }

  for (auto &t: threads) {
    t.join();
  }

  return 0;
}
