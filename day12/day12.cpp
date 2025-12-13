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

// As for day 10
#include "glpk.h"
#pragma comment(lib, "glpk_4_65.lib")

#include "../shared/Point.h"

using namespace std;
const regex INPUT_R(R"((\d+)x(\d+): (\d+) (\d+) (\d+) (\d+) (\d+) (\d+))");

typedef set<Point2> tile_t;
struct tile_orientation_t {
	int tileId;
	tile_t cells;
};
struct tile_placement_t {
	int tileId;
	vector<Point2> cells;
};

void rotate90(tile_t& tile)
{
	tile_t rotated;
	for (const auto& p : tile) {
		rotated.insert(Point2({ 2 - p.at(1), p.at(0) }));
	}
	tile = rotated;
}

void flipX(tile_t& tile)
{
	tile_t flipped;
	for (const auto& p : tile) {
		flipped.insert(Point2({ 2 - p.at(0), p.at(1) }));
	}
	tile = flipped;
}


bool buildGLPKold(const std::vector<tile_placement_t>& placements, int M, int N, const std::vector<int>& requiredCount)
{
	glp_prob* lp = glp_create_prob();
	glp_set_obj_dir(lp, GLP_MAX); // or GLP_MIN if you prefer

	int numVars = placements.size();
	glp_add_cols(lp, numVars);
	for (int i = 1; i <= numVars; i++) {
		glp_set_col_kind(lp, i, GLP_BV); // binary variable
		glp_set_obj_coef(lp, i, 0.0);    // no objective, just feasibility
	}

	// Count rows: one per cell + one per shape
	int numRows = M * N + requiredCount.size();
	glp_add_rows(lp, numRows);

	// --- Overlap constraints (<=1 per cell) ---
	int rowIndex = 1;
	std::vector<int> ia, ja;
	std::vector<double> ar;
	ia.push_back(0); ja.push_back(0); ar.push_back(0); // GLPK is 1-based

	for (int r = 0; r < M; r++) {
		for (int c = 0; c < N; c++) {
			Point2 pos = { r, c };
			glp_set_row_bnds(lp, rowIndex, GLP_UP, 0.0, 1.0);
			for (int i = 0; i < numVars; i++) {
				for (auto cell : placements[i].cells) {
					if (pos == cell) {
						ia.push_back(rowIndex);
						ja.push_back(i + 1);
						ar.push_back(1.0);
					}
				}
			}
			rowIndex++;
		}
	}

	// --- Tile count constraints (exact count per shape) ---
	for (int sid = 0; sid < requiredCount.size(); sid++) {
		glp_set_row_bnds(lp, rowIndex, GLP_FX,
			requiredCount[sid], requiredCount[sid]);
		for (int i = 0; i < numVars; i++) {
			if (placements[i].tileId == sid) {
				ia.push_back(rowIndex);
				ja.push_back(i + 1);
				ar.push_back(1.0);
			}
		}
		rowIndex++;
	}

	// Load constraint matrix
	glp_load_matrix(lp, ia.size() - 1, ia.data(), ja.data(), ar.data());

	// Solve
	glp_iocp parm;
	glp_init_iocp(&parm);
	parm.presolve = GLP_ON;
	bool solved = false;
	int err = glp_intopt(lp, &parm);
	if (err == 0) {
		int status = glp_mip_status(lp);
		solved = (status == GLP_OPT);
	}

	glp_delete_prob(lp);

	return solved;
}


bool buildGLPK(const std::vector<tile_placement_t>& placements,
	int tx, int ty,
	const std::vector<int>& requiredCount)
{
	glp_prob* lp = glp_create_prob();
	glp_set_obj_dir(lp, GLP_MIN); // pure feasibility

	const int numVars = (int)placements.size();
	const int K = (int)requiredCount.size();

	// Index placements per cell and per tile
	std::vector<std::vector<int>> cellVars(tx * ty);
	std::vector<std::vector<int>> tileVars(K);

	auto cell_id = [ty](int x, int y) { return x * ty + y; };

	for (int j = 0; j < numVars; ++j) {
		for (const auto& p : placements[j].cells) {
			int x = p.at(0), y = p.at(1);
			if (x >= 0 && x < tx && y >= 0 && y < ty)
				cellVars[cell_id(x, y)].push_back(j);
		}
		int t = placements[j].tileId;
		if (t >= 0 && t < K) tileVars[t].push_back(j);
	}

	// Columns
	glp_add_cols(lp, numVars);
	for (int j = 1; j <= numVars; ++j) {
		glp_set_col_kind(lp, j, GLP_BV);
		glp_set_obj_coef(lp, j, 0.0);
	}

	// Rows: coverage (<=1 per cell) + exact count per tile
	const int numRows = tx * ty + K;
	glp_add_rows(lp, numRows);

	int rowIndex = 1;
	// Coverage constraints
	for (int x = 0; x < tx; ++x) {
		for (int y = 0; y < ty; ++y, ++rowIndex) {
			glp_set_row_bnds(lp, rowIndex, GLP_UP, 0.0, 1.0);
		}
	}
	// Tile count constraints
	for (int t = 0; t < K; ++t, ++rowIndex) {
		glp_set_row_bnds(lp, rowIndex, GLP_FX,
			requiredCount[t], requiredCount[t]);
	}

	// Assemble sparse matrix
	std::vector<int> ia(1), ja(1);
	std::vector<double> ar(1);

	// Coverage rows
	rowIndex = 1;
	for (int x = 0; x < tx; ++x) {
		for (int y = 0; y < ty; ++y, ++rowIndex) {
			for (int j : cellVars[cell_id(x, y)]) {
				ia.push_back(rowIndex);
				ja.push_back(j + 1);
				ar.push_back(1.0);
			}
		}
	}
	// Tile count rows
	for (int t = 0; t < K; ++t, ++rowIndex) {
		for (int j : tileVars[t]) {
			ia.push_back(rowIndex);
			ja.push_back(j + 1);
			ar.push_back(1.0);
		}
	}

	// --- Symmetry-breaking: placement ordering ---
	for (int t = 0; t < K; ++t) {
		auto& cols = tileVars[t];
		// Sort placements by lexicographic anchor (min cell)
		std::sort(cols.begin(), cols.end(), [&](int a, int b) {
			auto minA = *std::min_element(placements[a].cells.begin(),
				placements[a].cells.end());
			auto minB = *std::min_element(placements[b].cells.begin(),
				placements[b].cells.end());
			if (minA.at(0) != minB.at(0)) return minA.at(0) < minB.at(0);
			if (minA.at(1) != minB.at(1)) return minA.at(1) < minB.at(1);
			return a < b;
			});
		// Add prefix constraints: x_{j+1} <= x_j
		for (int k = 0; k + 1 < (int)cols.size(); ++k) {
			int row = glp_add_rows(lp, 1);
			glp_set_row_bnds(lp, row, GLP_LO, 0.0, 0.0);
			int ia2[3] = { 0, row, row };
			int ja2[3] = { 0, cols[k] + 1, cols[k + 1] + 1 };
			double ar2[3] = { 0, 1.0, -1.0 };
			glp_set_mat_row(lp, row, 2, ja2, ar2);
		}
	}

	glp_load_matrix(lp, (int)ia.size() - 1, ia.data(), ja.data(), ar.data());

	glp_iocp parm;
	glp_init_iocp(&parm);
	parm.presolve = GLP_ON;

	bool solved = false;
	int err = glp_intopt(lp, &parm);
	if (err == 0) {
		int status = glp_mip_status(lp);
		solved = (status == GLP_OPT || status == GLP_FEAS);
	}

	glp_delete_prob(lp);
	return solved;
}

int main()
{
	int64_t part1 = 0;
	int64_t part2 = 0;

	vector<tile_orientation_t> tiles;
	vector<int> tilecounts;
	vector<int> tilexspacing;

	string s;
	for (int t = 0; t < 6; ++t) {
		getline(cin, s);

		tile_t tile;
		for (int64_t y = 0; y < 3; ++y) {
			getline(cin, s);
			for (int64_t x = 0; x < s.size(); ++x) {
				if (s.at(x) == '#') {
					tile.insert(Point2({ x, y }));
				}
			}
		}

		tilecounts.push_back(tile.size());

		// set to de-dupe orientations
		set<tile_t> orients;
		for (int flip = 0; flip < 2; ++flip) {
			for (int rot = 0; rot < 4; ++rot) {
				orients.insert(tile);
				rotate90(tile);
			}
			flipX(tile);
		}

		for (auto& orient : orients) {
			tiles.push_back({ t, orient });
		}

		// Check if any orients can be placed at distance 2 apart from original
		bool spacing_found = false;
		for (auto& orient : orients) {
			spacing_found = true;
			for (int y = 0; y < 3; ++y) {
				Point2 check({ 0, y });
				Point2 offset({ 2, y });
				if (tile.find(check) != tile.end() && orient.find(offset) != orient.end()) {
					spacing_found = false;
					break;
				}
			}
			if (spacing_found) {
				break;
			}
		}
		tilexspacing.push_back(spacing_found ? 2 : 3);

		getline(cin, s);
	}

	while (getline(cin, s)) {
		smatch sm;
		if (!regex_match(s, sm, INPUT_R)) {
			continue;
		}
		int tx = stoi(sm[1]);
		int ty = stoi(sm[2]);

		vector<int> nums;
		int totalpixels = 0;
		int totaltiles = 0;
		for (int i = 0; i < 6; ++i) {
			int n = stoi(sm[3 + i]);
			nums.push_back(n);
			totalpixels += n * tilecounts[i];
			totaltiles += n;
		}

		// Check trivial placement
		if (tx / 3 * ty / 3 > totaltiles) {
			cout << s << " : Trivial tiling" << endl;
			part1++;
			continue;
		}

		// Check if not enough pixels in grid
		if (tx * ty < totalpixels) {
			cout << s << " : Not enough pixels in grid" << endl;
			continue;
		}

		// Check compressed spacing
		int check_x = 0;
		int check_y = 0;
		for (int i = 0; i < 6; ++i) {
			for (int n = 0; n < nums[i]; ++n) {
				check_x += tilexspacing[i];
				if (check_x >= tx) {
					check_x = 0;
					check_y += 3;
				}
			}
		}
		if (check_y < ty) {
			cout << s << " : Compressed spacing tiling" << endl;
			part1++;
			continue;
		}

		// Generate placements for this grid
		vector<tile_placement_t> placements;
		for (const auto& orient : tiles) {
			for (int x = 0; x <= tx - 3; ++x) {
				for (int y = 0; y <= ty - 3; ++y) {
					Point2 offset = { x, y };
					tile_placement_t placement;
					placement.tileId = orient.tileId;
					for (const auto& cell : orient.cells) {
						placement.cells.push_back(cell + offset);
					}
					placements.push_back(placement);
				}
			}
		}

		// Solve (feasibility)
		if (buildGLPK(placements, tx, ty, nums)) {
			cout << s << " : Valid tiling found\n";
			part1++;
		}
		else {
			std::cout << s << " : No valid tiling found\n";
		}

	}

	cout << "Part 1: " << part1 << endl;
	cout << "Part 2: " << part2 << endl;
	return _getch();
}
