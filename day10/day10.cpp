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

// Get the library from https://www.gnu.org/software/glpk
#include "glpk.h"
#pragma comment(lib, "glpk_4_65.lib")

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
	int nvars = m.buttons.size();
	int ncons = m.n;

	// I friggen give up.  Linear programming it is.

	// Set up the problem
	glp_prob* lp = glp_create_prob();
	glp_set_prob_name(lp, "Lights and Buttons");
	glp_set_obj_dir(lp, GLP_MIN);

	// (# buttons) variables
	glp_add_cols(lp, nvars);
	for (int i = 1; i <= nvars; i++) {
		glp_set_col_kind(lp, i, GLP_IV);      // integer variable
		glp_set_col_bnds(lp, i, GLP_LO, 0.0, 0.0); // x >= 0
		glp_set_obj_coef(lp, i, 1.0);         // objective: minimize sum (coeff 1.0 for each variable)
	}

	// (# joltages) constraints
	glp_add_rows(lp, ncons);

	// Set up the constraint matrix.  Remember GLPK is 1-based arrays because WTF!?
	vector<vector<int>> indexes;
	vector<vector<double>> values;
	for (int i = 0; i < ncons; ++i) {
		vector<int> index(nvars + 1);
		vector<double> value(nvars + 1);
		for (int j = 1; j <= nvars; ++j) {
			index[j] = j;
		}
		indexes.push_back(index);
		values.push_back(value);
	}

	// Store the light presses into the matrix
	for (int j = 0; j < nvars; ++j) {
		for (auto l : m.buttons[j].lights) {
			values[l][j + 1] = 1.0;
		}
	}

	// Tell GLPK about the matrix
	for (int i = 0; i < ncons; ++i) {
		glp_set_row_bnds(lp, i + 1, GLP_FX, m.joltage[i], m.joltage[i]);
		int* ind = &indexes[i][0];
		double* val = &values[i][0];
		glp_set_mat_row(lp, i + 1, indexes[i].size() - 1, ind, val);
	}

	// Solve
	glp_iocp parm;
	glp_init_iocp(&parm);
	parm.presolve = GLP_ON;
	int ret = glp_intopt(lp, &parm);

	int result = INT_MAX;
	if (ret == 0) {
		cout << "Optimal solution:\n";
		for (int i = 1; i <= nvars; i++) {
			cout << "Button " << i << " = " << glp_mip_col_val(lp, i) << endl;
		}
		result = glp_mip_obj_val(lp);
		cout << "Total presses = " << result << endl;
		cout << endl;
	}
	else {
		throw "Boom";
	}

	glp_delete_prob(lp);
	return result;

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

	for (const auto& m : machines) {
		part1 += solve1(m);
		part2 += solve2(m);
	}

	cout << "Part 1: " << part1 << endl;
	cout << "Part 2: " << part2 << endl;
	_getch();
}