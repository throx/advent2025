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

// Grab library from https://github.com/Z3Prover/z3/releases
#include <z3++.h>
#pragma comment(lib, "libz3.lib")

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

int main()
{
	int64_t part1 = 0;
	int64_t part2 = 0;

	vector<tile_orientation_t> tiles;

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

		getline(cin, s);
	}

	while (getline(cin, s)) {
		smatch sm;
		if (!regex_match(s, sm, INPUT_R)) {
			continue;
		}
		int tx = stoi(sm[1]);
		int ty = stoi(sm[2]);

		int num[6];
		for (int i = 0; i < 6; ++i) {
			num[i] = stoi(sm[i + 3]);
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

		// z3 context + solver (use solver instead of optimize for boolean feasibility)
		z3::context ctx;

		// Create solver and set a timeout (ms). Tweak timeout as needed.
		z3::solver solver(ctx);

		// 5000 ms timeout example; change to fit your time/accuracy tradeoff.
		//z3::params p(ctx);
		//p.set("timeout", 5000);
		//solver.set(p);

		// Boolean variable for each placement
		std::vector<z3::expr> pvars;
		for (int i = 0; i < (int)placements.size(); i++) {
			pvars.push_back(ctx.bool_const(("p" + std::to_string(i)).c_str()));
		}

		int M = tx, N = ty; // grid size

		// No overlap: each cell covered at most once
		for (int r = 0; r < M; r++) {
			for (int c = 0; c < N; c++) {
				Point2 pos = { r, c };
				z3::expr_vector covering(ctx);
				for (int i = 0; i < placements.size(); i++) {
					for (const auto& cell : placements[i].cells) {
						if (pos == cell) covering.push_back(pvars[i]);
					}
				}
				if (!covering.empty()) {
					// At most one placement can cover this cell
					solver.add(z3::atmost(covering, 1));
				}
			}
		}

		// Tile count constraints (shape sid must be used exactly num[sid] times)
		for (int sid = 0; sid < 6; sid++) {
			z3::expr_vector vars(ctx);
			for (int i = 0; i < placements.size(); i++) {
				if (placements[i].tileId == sid) vars.push_back(pvars[i]);
			}
			if (!vars.empty()) {
				solver.add(z3::atmost(vars, num[sid]));
				solver.add(z3::atleast(vars, num[sid]));

			}
		}



		// Solve (feasibility)
		if (solver.check() == z3::sat) {
			z3::model m = solver.get_model();
			cout << s << " : Valid tiling found:\n";
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
