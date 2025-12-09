#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <numeric>

#include "../shared/Point.h"
#include "../shared/Rect.h"

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

	// Build a range tree of the border points
	map<int64_t, vector<int64_t>> border;
    DoLines(path.begin(), path.end(), [&](const Point2& p) {
        border[p[0]].push_back(p[1]);
    });
    for (auto& [x, ys] : border) {
        sort(ys.begin(), ys.end());
	}

    for (int i = 0; i < red.size() - 1; ++i) {
        for (int j = i + 1; j < red.size(); ++j) {

			// Check if this rectangle is valid
            Rect2 r = Rect2::Inclusive(red[i], red[j]);
			Point2 l = r.Length();
            if (l[0] > 1 && l[1] > 1) {
                Rect2 bad = r.ExpandBy(-1);

                // Range tree lookup
                bool valid = true;
				auto lower = border.lower_bound(bad.start()[0]);
				auto upper = border.upper_bound(bad.end()[0]);
                for (auto it = lower; it != upper; ++it) {
					const auto& ys = it->second;

					// Binary search for any y in range
					auto ystart = lower_bound(ys.begin(), ys.end(), bad.start()[1]);
					if (ystart != ys.end() && *ystart < bad.end()[1])    {
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
