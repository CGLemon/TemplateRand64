#include <iostream>
#include "../src/Random.h"

int main(int argc, char **argv) {

  auto rng = Random<random_t::SplitMix_64>::get_Rng();

  for (int i = 0; i < 10; i++) {
    auto a = rng.randuint64();
    printf("%lx\n", a);
  }

  return 0;
}
