#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#ifdef _WIN32
#include <conio.h>
#else
static inline void _getch() {}
#endif

using namespace std;

struct button {
	vector<int> lights;
};
struct machine {
	int n;
	vector<int> answer;
	vector<int> joltage;
	vector<button> buttons;

};

int vec2bin(const vector<int>& v)
{
	int ans = 0;
	for (int i = 0; i < v.size(); ++i) {
		ans += 1 << v[i];
	}
	return ans;
}

int solve1(const machine& m)
{
	int ans = vec2bin(m.answer);
	vector<int> btn;
	for (auto b : m.buttons) {
		btn.push_back(vec2bin(b.lights));
	}
	int best = INT_MAX;
	vector<int> best_butts;
	int total = 1 << m.buttons.size();
	for (int i = 0; i < total; ++i) {
		int cur = 0;
		int presses = 0;
		vector<int> butts;
		for (int j = 0; j < m.buttons.size(); ++j) {
			if (i & (1 << j)) {
				cur ^= btn[j];
				butts.push_back(j);
				++presses;
			}
		}
		if (cur == ans) {
			if (presses < best) {
				best = presses;
				best_butts = butts;
			}
		}
	}

	return best;
}

string vec2str(const vector<int>& v) {
	string s = "";
	for (auto i : v) {
		s = s + to_string(i) + ",";
	}
	return s.substr(0, s.length() - 1);
}

int solve2(const machine& m)
{
	map<vector<int>, int> visited;
	queue<vector<int>> next;
	size_t bsz = m.buttons.size();
	next.push(vector<int>(m.n));
	visited[vector<int>(m.n)] = 0;

	while (true) {
		auto joltage = next.front();
		next.pop();

		int presses = visited[joltage];

		cout << "Joltage = " << vec2str(joltage) << " = " << presses << endl;

		++presses;

		for (int i = 0; i < bsz; ++i) {
			auto newjoltage = joltage;
			for (auto l : m.buttons[i].lights) {
				newjoltage[l] = newjoltage[l] + 1;
			}

			if (newjoltage == m.joltage) {
				return presses;
			}

			if (visited.find(newjoltage) != visited.end()) {
				continue;
			}

			bool good = true;
			for (int i = 0; i < m.n; ++i) {
				if (newjoltage[i] > m.joltage[i]) {
					good = false;
					break;
				}
			}

			if (good) {
				visited[newjoltage] = presses;
				next.push(newjoltage);
			}
		}
	}
}

int main()
{
	int64_t part1 = 0;
	int64_t part2 = 0;

	vector<machine> machines;

	while (!cin.eof()) {
		string s;
		getline(cin, s);

		int i = 1;
		machine m;
		while (s[i] != ']') {
			if (s[i] == '#') {
				m.answer.push_back(i - 1);
			}
			++i;
		}
		m.n = i - 1;

		++i;
		++i;

		while (s[i] == '(') {
			button b;
			while (s[i] != ')') {
				++i;
				b.lights.push_back(s[i] - '0');
				++i;
			}

			m.buttons.push_back(b);
			++i;
			++i;
		}

		int j = 0;
		while (s[i] != '}') {
			++i;
			m.joltage.push_back(0);
			while (isdigit(s[i])) {
				m.joltage[j] = m.joltage[j] * 10 + (s[i] - '0');
				++i;
			}
			++j;
		}

		machines.push_back(m);
	}

	int x = 0;
	for (const auto& m : machines) {
		cout << x++ << endl;
		part1 += solve1(m);
		part2 += solve2(m);
	}

	cout << "Part 1: " << part1 << endl;
	cout << "Part 2: " << part2 << endl;
	_getch();
}