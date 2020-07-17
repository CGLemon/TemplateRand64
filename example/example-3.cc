#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include "../src/Random.h"

int main(int argc, char **argv) {
  std::uniform_real_distribution<float> normal_dist(0.0,1.0);
  auto rng = Random<random_t::SplitMix_64>::get_Rng();
    
    
  for (int i = 0; i < 10; i++) {
    float rand = normal_dist(rng);
    printf("%f\n", rand);
  }

  return 0;
}
