#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <regex>
#ifdef _WIN32
#include <conio.h>
#else
static inline void _getch() {}
#endif

using namespace std;

const regex INPUT_R(R"([a-z]{3})");
map<string, vector<string>> servers;

int64_t dfs(const string& start, const string& end, map<string, int64_t>& counts)
{
	if (start == end) {
		return 1;
	}

	auto it = counts.find(start);
	if (it != counts.end()) {
		return it->second;
	}

	int64_t count = 0;
	for (string& output : servers[start]) {
		count += dfs(output, end, counts);
	}
	counts[start] = count;
	return count;
}

int64_t dfs(const string& start, const string& end)
{
	map<string, int64_t> counts;
	return dfs(start, end, counts);
}

typedef tuple<string, bool, bool> state_t;

int64_t dfs2(const state_t& state, const string& end, map<state_t, int64_t>& counts)
{
	const string& start = get<0>(state);
	bool used_fft = get<1>(state);
	bool used_dac = get<2>(state);

	if (start == "fft") {
		if (used_fft) {
			return 0;
		}
		used_fft = true;
	}

	if (start == "dac") {
		if (used_dac) {
			return 0;
		}
		used_dac = true;
	}

	if (start == end) {
		if (!used_dac || !used_fft) {
			return 0;
		}
		return 1;
	}

	auto it = counts.find(state);
	if (it != counts.end()) {
		return it->second;
	}

	int64_t count = 0;
	for (string& output : servers[start]) {
		state_t next_state = make_tuple(output, used_fft, used_dac);
		count += dfs2(next_state, end, counts);
	}
	counts[state] = count;

	return count;
}

int64_t dfs2(const string& start, const string& end)
{
	map<state_t, int64_t> counts;
	state_t state = make_tuple(start, false, false);
	return dfs2(state, end, counts);
}

int main()
{
	int64_t part1 = 0;
	int64_t part2 = 0;

	string s;
	while (getline(cin, s)) {
		string server = s.substr(0, 3);
		auto match_iterator = sregex_iterator(s.begin() + 4, s.end(), INPUT_R);
		auto end_iterator = sregex_iterator();

		while (match_iterator != end_iterator) {
			smatch sm = *match_iterator;
			++match_iterator;

			string output = sm[0];
			servers[server].push_back(output);

			// Graphviz output
			// cout << "  " << server << " -> " << output << ";" << endl;
		}
	}

	part1 = dfs("you", "out");
	part2 = dfs2("svr", "out");

	// And I only found out later that there aren't paths from dac to fft
	int64_t p2a = dfs("svr", "fft");
	int64_t p2b = dfs("fft", "dac");
	int64_t p2c = dfs("dac", "out");

	cout << "Part 1: " << part1 << endl;
	cout << "Part 2: " << part2 << endl;
	cout << "Part 2 alt: " << (p2a * p2b * p2c) << endl;
	_getch();
}
