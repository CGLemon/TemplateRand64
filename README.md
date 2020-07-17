# TemplateRand64
64 位元亂數生成庫

# 容易使用
使用模板編成，有通用統一的界面，例如

    auto rng_splitmix = Random<random_t::SplitMix_64>::get_Rng();
    std::uint64_t rand_a = rng_splitmix.randuint64();
    
    auto rng_widynski = Random<random_t::Widynski>::get_Rng();
    std::uint64_t rand_b = rng_widynski.randuint64();
    
    
# 支持多線程
可以自動對不同線程使用不同的亂數，請看 example/example-2

# 支持標準庫
可以和 c++11 的亂數庫一起使用

    std::uniform_real_distribution<float> normal_dist(0.0,1.0);
    auto rng = Random<random_t::SplitMix_64>::get_Rng();
    float rand = normal_dist(rng);
    
# 亂數種子
自定義亂數種子

    auto rng = Random<random_t::SplitMix_64>::get_Rng(123456);
    
由時間自動生成

    auto rng = Random<random_t::SplitMix_64>::get_Rng(TIME_SEED);
     
由線程自動生成

    auto rng = Random<random_t::SplitMix_64>::get_Rng(THREADS_SEED);

# 支持的方法
1. SplitMix_64

2. XoroShiro128Plus

3. JenkinsSmall

4. Widynski

5. WyHash (only suppurt for linux)

6. WyRand (only suppurt for linux)

7. Lehmer64 (only suppurt for linux)
