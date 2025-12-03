#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#ifdef _WIN32
#include <conio.h>
#else
static inline void _getch() {}
#endif

using namespace std;

long long joltage(string s, int places)
{
    stringstream ss;
    auto it = s.begin();

	while ((--places) >= 0) {
        auto n_last = s.end() - places;
		it = max_element(it, n_last);
        ss << *it;
        ++it;
    }
    cout << ss.str() << endl;
	return stoll(ss.str());
}

int main()
{

    long long part1 = 0;
    long long part2 = 0;

    string s;
    while (!cin.eof()) {
        cin >> s;

        part1 += joltage(s, 2);
        part2 += joltage(s, 12);
    }

	cout << "Part 1: " << part1 << endl;
	cout << "Part 2: " << part2 << endl;
    _getch();
    return 0;
}
