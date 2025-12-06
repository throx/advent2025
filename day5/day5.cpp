#include <iostream>
#include <regex>
#include <string>
#include <vector>
#ifdef _WIN32
#include <conio.h>
#else
static inline void _getch() {}
#endif

using namespace std;

const regex INPUT_R(R"((\d+)-(\d+))");

struct range {
	long long start;
	long long end;

	range(long long s, long long e) : start(s), end(e) {}
	bool contains(long long v) const {
		return v >= start && v < end;
	}
};

int main()
{
	long long part1 = 0;
	long long part2 = 0;

	vector<range> ranges;

	while(true) {
		string s;
		getline(cin, s);
		if (s.empty()) {
			break;
		}

		smatch sm;
		if (regex_match(s, sm, INPUT_R)) {
			long long n1 = stoll(sm[1]);
			long long n2 = stoll(sm[2]);
			ranges.push_back(range(n1, n2 + 1));
		}
	}

	while (!cin.eof()) {
		string s;
		getline(cin, s);
		long long n = stoll(s);

		for (const auto& r : ranges) {
			if (r.contains(n)) {
				++part1;
				break;
			}
		}
	}

	bool done = false;
	while (!done) {
		done = true;
		vector<range> merged;
		for (const auto& r : ranges) {
			bool added = false;
			for (auto& mr : merged) {
				if (r.end > mr.start && r.start < mr.end) {
					mr.start = min(mr.start, r.start);
					mr.end = max(mr.end, r.end);
					added = true;
					done = false;
					break;
				}
			}
			if (!added) {
				merged.push_back(r);
			}
		}

		ranges.swap(merged);
	}

	for (const auto& r : ranges) {
		part2 += r.end - r.start;
	}

	cout << "Part 1: " << part1 << endl;
	cout << "Part 2: " << part2 << endl;
	_getch();
	return 0;
}
