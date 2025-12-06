#include <iostream>
#include <set>
#ifdef _WIN32
#include <conio.h>
#else
static inline void _getch() {}
#endif

#include "../shared/Point.h"

using namespace std;

set<Point2> rolls;

int main()
{
    int part1 = 0;
    int part2 = 0;

    int y = 0;
	while (!std::cin.eof()) {
        std::string s;
        std::cin >> s;
        for (int x = 0; x < s.size(); ++x) {
            if (s[x] == '@') {
                Point2 p({ x, y });
				rolls.insert(p);
            }
        }
        ++y;
    }

	for (auto& roll : rolls) {
        int adj = 0;
        roll.DoNeighbours([&](const Point2& n) {
            if (rolls.find(n) != rolls.end()) {
                ++adj;
            }
			});

        if (adj < 4) {
            ++part1;
        }
    }

	bool done = false;
    while (!done) {
        set<Point2> next;
		done = true;
        for (auto& roll : rolls) {
            int adj = 0;
            roll.DoNeighbours([&](const Point2& n) {
                if (rolls.find(n) != rolls.end()) {
                    ++adj;
                }
                });

            if (adj < 4) {
                ++part2;
				done = false;
            }
            else {
				next.insert(roll);
            }
        }
        rolls.swap(next);
    }

	cout << "Part 1: " << part1 << endl;
    cout << "Part 2: " << part2 << endl;
    _getch();
	return 0;
}
