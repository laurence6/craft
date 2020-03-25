#ifndef UTIL_HPP
#define UTIL_HPP

#include <chrono>

template<typename T>
class NonCopy
{
protected:
    constexpr NonCopy()         = default;
    NonCopy(const NonCopy&)     = delete;
    NonCopy(NonCopy&&) noexcept = default;

    ~NonCopy() = default;

    NonCopy& operator=(const NonCopy&) = delete;
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

protected:
    Singleton()                     = default;
    Singleton(Singleton const&)     = delete;
    Singleton(Singleton&&) noexcept = delete;

    ~Singleton() = default;

    Singleton& operator=(Singleton const&) = delete;
    Singleton& operator=(Singleton&&) noexcept = delete;
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
