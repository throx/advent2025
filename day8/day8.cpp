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

struct PointSetAdaptor {
    const vector<Point3>& pts;

    PointSetAdaptor(const vector<Point3>& points) : pts(points) {}

    inline size_t kdtree_get_point_count() const { return pts.size(); }

    inline double kdtree_get_pt(const size_t idx, int dim) const {
        return static_cast<double>(pts[idx][dim]);
    }

    template <class BBOX>
    bool kdtree_get_bbox(BBOX&) const { return false; }
};


vector<pair<Point3, Point3>> find_pairs(const vector<Point3>& points)
{
    // Store backwards so we can erase at beginning
    map<double, pair<Point3, Point3>, greater<>> closest_pairs;

    for (int i = 0; i < points.size() - 1; ++i) {
        for (int j = i + 1; j < points.size(); ++j) {

            double d = L2Dist(points[i], points[j]);
            closest_pairs[d] = make_pair(points[i], points[j]);
        }
    }

    vector<pair<Point3, Point3>> ret;
    for (const auto& e : closest_pairs) {
        ret.push_back(e.second);
    }
    reverse(ret.begin(), ret.end());

    return ret;
}

int main()
{
    int64_t part1 = 0;
    int64_t part2 = 0;

    vector<Point3> breakers;
    map<Point3, int> circuits;
    int next_circuit = 1;

    string s;
    while (!cin.eof()) {
        Point3 p;
        cin >> p;
        breakers.push_back(p);
        circuits[p] = 0;
    }

    int num;
    if (breakers.size() < 30) {
        num = 10;
    }
    else {
        num = 1000;
    }

    auto closest_pairs = find_pairs(breakers);
    int n = 0;

    for (; n < num; ++n) {
        Point3 p1 = closest_pairs[n].first;
		Point3 p2 = closest_pairs[n].second;

        if (circuits[p1] == 0 && circuits[p2] == 0) {
            circuits[p1] = next_circuit;
            circuits[p2] = next_circuit;
            ++next_circuit;
        }
        else if (circuits[p1] == 0) {
            circuits[p1] = circuits[p2];
        }
        else if (circuits[p2] == 0) {
            circuits[p2] = circuits[p1];
        }
        else {
            int old_c = circuits[p2];
			int c = circuits[p1];
            for (auto& e : circuits) {
                if (e.second == old_c) {
                    e.second = c;
                }
            }
        }
    }

    // Count number in each circuit
    map<int, int> circuit_counts;
    for (const auto& e : circuits) {
        circuit_counts[e.second]++;
    }

    // Sort counts big to little
    multiset<int, greater<>> counts;
    for (const auto& e : circuit_counts) {
        if (e.first != 0) {
            counts.insert(e.second);
        }
    }

    part1 = 1;
    int i = 0;
    for (int e : counts) {
        part1 *= e;
        ++i;
        if (i == 3) break;
    }

	cout << "Part 1: " << part1 << endl;

    // Keep going!
    while (true) {
        Point3 p1 = closest_pairs[n].first;
        Point3 p2 = closest_pairs[n].second;

        if (circuits[p1] == 0 && circuits[p2] == 0) {
            circuits[p1] = next_circuit;
            circuits[p2] = next_circuit;
            ++next_circuit;
        }
        else if (circuits[p1] == 0) {
            circuits[p1] = circuits[p2];
        }
        else if (circuits[p2] == 0) {
            circuits[p2] = circuits[p1];
        }
        else {
            int old_c = circuits[p2];
            int c = circuits[p1];
            for (auto& e : circuits) {
                if (e.second == old_c) {
                    e.second = c;
                }
            }
        }

		int c = circuits.begin()->second;
		bool all = true;
		for (auto& e : circuits) {
            if (e.second != c) {
                all = false;
                break;
            }
        }
        if (all) {
            part2 = 1LL * p1[0] * p2[0];
            break;
        }
        ++n;
    }



	cout << "Part 2: " << part2 << endl;
    _getch();
}
