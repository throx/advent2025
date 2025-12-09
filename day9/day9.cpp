#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <numeric>

#include "../shared/Point.h"
#include "../shared/Rect.h"

// Get this yourself from github.com/jlblancoc/nanoflann
#include "../shared/nanoflann.hpp"

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

    vector<Point2> red;

    string s;
    while (!cin.eof()) {
        Point2 p;
        cin >> p;
        red.push_back(p);
    }

	for (int i = 0; i < red.size() - 1; ++i) {
        for (int j = i + 1; j < red.size(); ++j) {
			Rect2 r = Rect2::Inclusive(red[i], red[j]);
            int64_t sz = r.Size();
			part1 = max(part1, sz);
        }
    }

    auto path = red;
    path.push_back(red[0]);
	set<Point2> border;
    DoLines(path.begin(), path.end(), [&](const Point2& p) {
        border.insert(p);
    });

    for (int i = 0; i < red.size() - 1; ++i) {
        for (int j = i + 1; j < red.size(); ++j) {

			// Check if this rectangle is valid
            Rect2 r = Rect2::Inclusive(red[i], red[j]);
			Point2 l = r.Length();
            if (l[0] > 1 && l[1] > 1) {
                Rect2 bad = r.ExpandBy(-1);

                bool valid = true;
                for (auto it = border.lower_bound(bad.start()); it != border.upper_bound(bad.end()); ++it) {
                    if (bad.Contains(*it)) {
                        valid = false;
                        break;
                    }
                }
                if (!valid) continue;
            }

            int64_t sz = r.Size();
            part2 = max(part2, sz);
        }
		cout << i << "/" << red.size() - 1 << endl;
    }



    cout << "Part 1: " << part1 << endl;
    cout << "Part 2: " << part2 << endl;
    _getch();
}
