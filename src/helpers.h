#ifndef HELPERS_h
#define HELPERS_h

#include <string>
#include <map>
#include <memory>
#include <sstream>

#include "k.h"

#define Q(x, e) if(x) { krr((S)e); R 0; }
#define kip(x) kp((S)x.c_str())
#define kis(x) ks((S)x.c_str())
#define kts(x) ktj(-KP, pu(x))

inline F zu(I u) { return u / 8.64e4 - 10957; }  // kdb+ datetime from unix
inline I uz(F f) { return 86400 * (f + 10957); } // unix from kdb+ datetime
inline J pu(I u) { return 8.64e13 * (u / 8.64e4 - 10957); } // kdb+ timestamp from unix
inline I up(J f) { return (f / 8.64e13 + 10957) * 8.64e4; } // unix from kdb+ timestamp

template<typename ... Args>
std::string stringFormat(const std::string& format, Args ... args )
{
    size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

static std::string formatTime(const char *str, time_t time, int adjusted)
{
    static char buffer[4096];
    
    struct tm *timeinfo = localtime(&time);
    if (adjusted) timeinfo->tm_hour -= 1;
    strftime(buffer, sizeof(buffer), str, timeinfo);
    
    return stringFormat("%s", buffer);
}

static std::string getString(K x)
{
    if (abs(x->t) != KC)
        throw std::runtime_error("type");
    if (x->t == -KC)
        return x->s;
    
    std::stringstream ss;
    DO(xn, ss << kc(kC(x)[i])->g);
    R ss.str();
}

template<class F, class V>
auto partial(F&&f, V&&v)
{
    return
    [f=std::forward<F>(f), v=std::forward<V>(v)](auto&&... args)
    {
        return f(v, decltype(args)(args)...);
    };
}

template<class F, class A0, class...Args>
auto partial(F&&f, A0&&a0, Args&&...args)
{
    return partial(partial(std::forward<F>(f), std::forward<A0>(a0)), std::forward<Args>(args)...);
}

Z K createDictionary(std::map<std::string, K> map)
{
    K keys = ktn(KS, map.size());
    K vals = ktn(0, 0);
    I i = 0;
    for (auto it = map.begin(); it != map.end(); it++, i++) {
        kS(keys)[i] = ss((S)it->first.c_str());
        jk(&vals, it->second);
    }
    R xD(keys, vals);
}

Z K identity()
{
    K id = ka(101);
    id->g = 0;
    R id;
}

#endif
