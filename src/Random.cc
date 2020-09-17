#include "Random.h"

#include <chrono>

namespace random_utils {

static inline std::uint64_t splitmix64(std::uint64_t z) {
  /*
   * The parameter detail are from
   * https://github.com/lemire/testingRNG/blob/master/source/splitmix64.h
   */

  z += 0x9e3779b97f4a7c15;
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
  z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
  return z ^ (z >> 31);
}

static inline std::uint64_t rotl(const std::uint64_t x, const int k) {
  return (x << k) | (x >> (64 - k));
}

static inline std::uint64_t get_seed(std::uint64_t seed) {
  if (seed == THREADS_SEED) {
    auto thread_id = std::hash<std::thread::id>()(std::this_thread::get_id());
    seed = static_cast<std::uint64_t>(thread_id);
  } else if (seed == TIME_SEED) {
    auto get_time = std::chrono::system_clock::now().time_since_epoch().count();
    seed = static_cast<std::uint64_t>(get_time);
  }
  return seed;
}
} // namespace random_utils


#define __RANDOM_INIT(__TYPE, __CT)                  \
template<>                                           \
void Random<__TYPE>::seed_init(std::uint64_t seed) { \
  seed = random_utils::get_seed(seed);               \
  static_assert(SEED_SZIE >= __CT,                   \
      "The number of seeds is not enough?\n");       \
  for (auto i = size_t{0}; i < SEED_SZIE; ++i) {     \
    seed = random_utils::splitmix64(seed);           \
    Random<__TYPE>::m_seeds[i] = seed;               \
  }                                                  \
}


template<random_t T>
thread_local std::uint64_t 
    Random<T>::m_seeds[Random<T>::SEED_SZIE];

__RANDOM_INIT(random_t::SplitMix_64, 1);

__RANDOM_INIT(random_t::XoroShiro128Plus, 2);

__RANDOM_INIT(random_t::WyRand, 1);

__RANDOM_INIT(random_t::WyHash, 1);

template<>
void Random<random_t::Widynski>::seed_init(std::uint64_t seed) {
  seed = random_utils::get_seed(seed);
 
  static_assert(SEED_SZIE >= 3,
      "The number of seeds is not enough?\n");

  Random<random_t::Widynski>::m_seeds[0] = 0;
  Random<random_t::Widynski>::m_seeds[1] = 0;
  Random<random_t::Widynski>::m_seeds[2] = seed;
  Random<random_t::Widynski>::m_seeds[2] |= 1;

  if ((Random<random_t::Widynski>::m_seeds[2] >> 32) == 0) {
    Random<random_t::Widynski>::m_seeds[2]
        = Random<random_t::Widynski>::m_seeds[2] 
        | (Random<random_t::Widynski>::m_seeds[2] << 32);
  }
}

template<>
void Random<random_t::JenkinsSmall>::seed_init(std::uint64_t seed) {
  seed = random_utils::get_seed(seed);
 
  static_assert(SEED_SZIE >= 4,
      "The number of seeds is not enough\n");

  Random<random_t::JenkinsSmall>::m_seeds[0] = 0xf1ea5eed;
  Random<random_t::JenkinsSmall>::m_seeds[1] = seed;
  Random<random_t::JenkinsSmall>::m_seeds[2] = seed;
  Random<random_t::JenkinsSmall>::m_seeds[3] = seed;
}

#ifdef LINUX_KEY
template<random_t T>
thread_local __uint128_t Random<T>::m_seed128;
#endif 

template<>
void Random<random_t::Lehmer64>::seed_init(std::uint64_t seed) {
  seed = random_utils::get_seed(seed);
  static_assert(SEED_SZIE >= 1,
      "The number of seeds is not enough\n");

#ifdef LINUX_KEY
  Random<random_t::Lehmer64>::m_seed128 = 
      (((__uint128_t)random_utils::splitmix64(seed)) << 64) +
                     random_utils::splitmix64(seed + 1);
#else
  m_seeds[0] = seed;

#endif 
}

template<>
std::uint64_t Random<random_t::SplitMix_64>::randuint64() {
  /*
   * The parameter detail are from
   * https://github.com/lemire/testingRNG/blob/master/source/splitmix64.h
   */
  constexpr size_t seed_Idx = 0;

  Random<random_t::SplitMix_64>::m_seeds[seed_Idx] += 0x9e3779b97f4a7c15;
  auto z = Random<random_t::SplitMix_64>::m_seeds[seed_Idx];
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
  z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
  return z ^ (z >> 31);
}

template<>
std::uint64_t Random<random_t::XoroShiro128Plus>::randuint64() {
  /*
   * The parameter detail are from
   * https://github.com/lemire/testingRNG/blob/master/source/xoroshiro128plus.h
   */

  const std::uint64_t s0 = Random<random_t::XoroShiro128Plus>::m_seeds[0];
  std::uint64_t s1 = Random<random_t::XoroShiro128Plus>::m_seeds[1];
  const std::uint64_t result = s0 + s1;

  s1 ^= s0;
  Random<random_t::XoroShiro128Plus>::m_seeds[0] = 
      random_utils::rotl(s0, 55) ^ s1 ^ (s1 << 14);
  Random<random_t::XoroShiro128Plus>::m_seeds[1] = 
      random_utils::rotl(s1, 36);

  return result;
}

template<>
std::uint64_t Random<random_t::Widynski>::randuint64() {
  /*
   * https://github.com/lemire/testingRNG/blob/master/source/widynski.h
   */

  Random<random_t::Widynski>::m_seeds[1] *=
      Random<random_t::Widynski>::m_seeds[1];
 
  Random<random_t::Widynski>::m_seeds[1] +=
      (Random<random_t::Widynski>::m_seeds[0] +=
          Random<random_t::Widynski>::m_seeds[2]);
  
  Random<random_t::Widynski>::m_seeds[1] =
      (Random<random_t::Widynski>::m_seeds[1] >> 32) | 
      (Random<random_t::Widynski>::m_seeds[1] << 32);

  return Random<random_t::Widynski>::m_seeds[1];
}

template<>
std::uint64_t Random<random_t::JenkinsSmall>::randuint64() {
  /*
   * https://github.com/lemire/testingRNG/blob/master/source/jenkinssmall.h
   */
  const std::uint64_t e = 
      Random<random_t::JenkinsSmall>::m_seeds[0] - 
      random_utils::rotl(Random<random_t::JenkinsSmall>::m_seeds[1], 7);
  
  Random<random_t::JenkinsSmall>::m_seeds[0] =
      Random<random_t::JenkinsSmall>::m_seeds[1] ^ 
      random_utils::rotl(Random<random_t::JenkinsSmall>::m_seeds[2], 13);

  Random<random_t::JenkinsSmall>::m_seeds[1] =
      Random<random_t::JenkinsSmall>::m_seeds[2] + 
      random_utils::rotl(Random<random_t::JenkinsSmall>::m_seeds[3], 37);

  Random<random_t::JenkinsSmall>::m_seeds[2] =
      Random<random_t::JenkinsSmall>::m_seeds[3] + e;

  Random<random_t::JenkinsSmall>::m_seeds[3] =
      e + Random<random_t::JenkinsSmall>::m_seeds[0];

  return Random<random_t::JenkinsSmall>::m_seeds[3];
}

template<>
std::uint64_t Random<random_t::Lehmer64>::randuint64() {
  /**
  * D. H. Lehmer, Mathematical methods in large-scale computing units.
  * Proceedings of a Second Symposium on Large Scale Digital Calculating
  * Machinery;
  * Annals of the Computation Laboratory, Harvard Univ. 26 (1951), pp. 141-146.
  *
  * P L'Ecuyer,  Tables of linear congruential generators of different sizes and
  * good lattice structure. Mathematics of Computation of the American
  * Mathematical
  * Society 68.225 (1999): 249-260.
  */

#ifdef LINUX_KEY
  Random<random_t::Lehmer64>::m_seed128 *= 0xda942042e4dd58b5;
  return Random<random_t::Lehmer64>::m_seed128 >> 64;
#else
  printf("Do not support Windows\n");
  return m_seeds[0];
#endif
}

template<>
std::uint64_t Random<random_t::WyRand>::randuint64() {
  /*
   * https://github.com/lemire/testingRNG/blob/master/source/wyrand.h
   */

  constexpr size_t seed_Idx = 0;

#ifdef LINUX_KEY
  Random<random_t::WyRand>::m_seeds[seed_Idx] += 0xa0761d6478bd642f;
  __uint128_t t = 
      static_cast<__uint128_t>(Random<random_t::WyRand>::m_seeds[seed_Idx]) * 
      (Random<random_t::WyRand>::m_seeds[seed_Idx] ^ 0xe7037ed1a0b428db);

  return (t >> 64) ^ t;

#else
  printf("Do not support Windows\n");
  return m_seeds[seed_Idx];
#endif
}


template<>
std::uint64_t Random<random_t::WyHash>::randuint64() {
  /*
   * https://github.com/lemire/testingRNG/blob/master/source/wyhash.h
   */

  constexpr size_t seed_Idx = 0;

#ifdef LINUX_KEY
  Random<random_t::WyHash>::m_seeds[seed_Idx] += 0x60bee2bee120fc15;
  __uint128_t tmp = static_cast<__uint128_t>(
      Random<random_t::WyHash>::m_seeds[seed_Idx]) * 0x1b03738712fad5c9;
  std::uint64_t m2 = (tmp >> 64) ^ tmp;

  return m2;
#else
  printf("Do not support Windows\n");
  return m_seeds[seed_Idx];
#endif
}

