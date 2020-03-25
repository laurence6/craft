#ifndef UTIL_HPP
#define UTIL_HPP

#include <chrono>

template<typename T>
class NonCopy
{
public:
    NonCopy(const NonCopy&) = delete;
    NonCopy& operator=(const NonCopy&) = delete;

protected:
    constexpr NonCopy()         = default;
    NonCopy(NonCopy&&) noexcept = default;

    ~NonCopy() = default;

    NonCopy& operator=(NonCopy&&) noexcept = default;
};

template<typename T>
class Singleton
{
public:
    static T& ins()
    {
        static T ins;
        return ins;
    }

    Singleton(Singleton const&)     = delete;
    Singleton(Singleton&&) noexcept = delete;

    Singleton& operator=(Singleton const&) = delete;
    Singleton& operator=(Singleton&&) noexcept = delete;

protected:
    Singleton() = default;

    ~Singleton() = default;
};

inline uint64_t time_now_ms() noexcept
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

inline uint64_t time_now_s() noexcept
{
    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

#endif
