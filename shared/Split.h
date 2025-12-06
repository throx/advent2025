#pragma once
#include <stdint.h>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm> 
#include <cctype>
#include <locale>

// trim from start
[[nodiscard]]
static inline std::string ltrim(const std::string& s) {
    std::string r(s);
    r.erase(r.begin(), std::find_if(r.begin(), r.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));
    return r;
}

// trim from end
[[nodiscard]]
static inline std::string rtrim(const std::string& s) {
    std::string r(s);
    r.erase(std::find_if(r.rbegin(), r.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
        }).base(), r.end());
    return r;
}

// trim from both ends
[[nodiscard]]
static inline std::string trim(const std::string& s) {
    return ltrim(rtrim(s));
}

template<class _F>
void split(const std::string& s, _F fn, char tok = ',', bool ignore_consecutive = false)
{
    std::istringstream iss(s);
    std::string n;

    while (getline(iss, n, tok)) {
        if (!ignore_consecutive || !n.empty()) {
            fn(trim(n));
        }
    }
}

static inline std::vector<std::string> split(const std::string& s, char tok = ',', bool ignore_consecutive = false)
{
    std::vector<std::string> r;
    split(s, [&](std::string&& x) {
        r.push_back(x);
        }, tok, ignore_consecutive);
    return r;
}


static inline std::vector<int> split_ints(const std::string& s, char tok = ',')
{
    std::vector<int> r;
    split(s, [&](std::string&& x) {
        r.push_back(stoi(x));
        }, tok, true);
    return r;
}


static inline std::vector<int64_t> split_int64s(const std::string& s, char tok = ',')
{
    std::vector<int64_t> r;
    split(s, [&](std::string&& x) {
        r.push_back(stoll(x));
        }, tok, true);
    return r;
}


template<class _F>
void split(const std::string& s, _F fn, const std::string& tok = "->")
{
    std::string buf(s);
    auto p = buf.find(tok);
    while (p != -1) {
        fn(trim(buf.substr(0, p)));
        buf = buf.substr(p + tok.length());
        p = buf.find(tok);
    }

    fn(trim(buf));
}

static inline std::vector<std::string> split(const std::string& s, const std::string& tok)
{
    std::vector<std::string> r;
    split(s, [&](std::string&& x) {
        r.push_back(x);
        }, tok);
    return r;
}

