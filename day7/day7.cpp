#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>

#include "../shared/Point.h"

#ifdef _WIN32
#include <conio.h>
#else
static inline void _getch() {}
#endif

using namespace std;

int main()
{
	int64_t part1 = 0;
	int64_t part2 = 0;

    string s;
	int64_t y = 0;
	set<Point2> splitters;
	map<int64_t, int64_t> beams;
	while (getline(cin, s)) {
		for (int64_t x = 0; x < s.length(); ++x) {
			if (s[x] == 'S') {
				beams[x] = 1;
			}
			if (s[x] == '^') {
				splitters.insert({ x, y });
			}
		}
		++y;
	}
	int64_t maxy = y;

	for (y = 1; y < maxy; ++y) {
		map<int64_t, int64_t> newbeams;
		for (auto& e : beams) {
			int64_t x = e.first;
			if (splitters.find({ x, y }) != splitters.end()) {
				newbeams[x - 1] += e.second;
				newbeams[x + 1] += e.second;
				++part1;
			}
			else {
				newbeams[x] += e.second;
			}
		}
		beams.swap(newbeams);
	}

	for (auto& e : beams) {
		part2 += e.second;
	}

	cout << "Part 1: " << part1 << endl;
	cout << "Part 2: " << part2 << endl;
	_getch();
}
