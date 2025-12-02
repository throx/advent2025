#include <iostream>
#include <string>
#include <regex>
#include <sstream>
#ifdef _WIN32
#include <conio.h>
#else
static inline void _getch() {}
#endif

using namespace std;

const regex INPUT_R(R"((\d+)-(\d+))");

string plus1(const string& s) {
    string r = s;
    for (int i = s.length() - 1; i >= 0; --i) {
        if (s[i] != '9') {
            r[i] = s[i] + 1;
            return r;
        }

        r[i] = '0';
    }
    return r;
}

long long check_part1(string n1, string n2)
{
    if (n1.length() != n2.length()) {
        throw "BOOM";
    }

    if (n1.length() % 2 == 1) {
        return 0;
    }

    long long sum = 0;

    long long in1 = stoll(n1);
    long long in2 = stoll(n2);

    string h1 = n1.substr(0, n1.length() / 2);
    string h2 = n2.substr(0, n2.length() / 2);

    h2 = plus1(h2);

    while (h1 != h2) {
        string f = h1 + h1;
        long long n = stoll(f);
        if (n >= in1 && n <= in2) {
            sum += n;
        }

        h1 = plus1(h1);
    }

    return sum;
}

long long check_part2(string n1, string n2)
{
    const regex TEST_R(R"(^(.+)\1+$)");

    long long sum = 0;

    long long in1 = stoll(n1);
    long long in2 = stoll(n2);

    for (long long i = in1; i <= in2; ++i) {
        stringstream ss;
        ss << i;
        if (regex_match(ss.str(), TEST_R)) {
            sum += i;
            cout << i << endl;
        }
    }

    return sum;
}

int main()
{
    string s;
    cin >> s;

    long long part1 = 0;
    long long part2 = 0;

    auto match_iterator = sregex_iterator(s.begin(), s.end(), INPUT_R);
    auto end_iterator = sregex_iterator();

    while (match_iterator != end_iterator) {
        smatch sm = *match_iterator;
        ++match_iterator;

        cout << sm[0] << endl;

        string n1 = sm[1];
		string n2 = sm[2];
        part2 += check_part2(n1, n2);

        if (n1.length() < n2.length()) {
            part1 += check_part1(n1, string(n1.length(), '9'));
            n1 = "1"s + string(n1.length(), '0');
        }

        part1 += check_part1(n1, n2);


    }

    // RESULTS...

    cout << "Part 1: " << part1 << endl;
    cout << "Part 2: " << part2 << endl;

    cout << "Press a key to continue" << endl;
    _getch();
    return 0;
}
