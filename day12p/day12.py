import re
import numpy as np
from cuopt.linear_programming import SolverSettings
from cuopt.linear_programming.problem import *
from cuopt.linear_programming.solver.solver_parameters import *
from cuopt.linear_programming.solver.solver_wrapper import *

INPUT_R = re.compile(r"(\d+)x(\d+): (\d+) (\d+) (\d+) (\d+) (\d+) (\d+)")

class Point2(tuple):
    def __new__(cls, iterable):
        return super().__new__(cls, iterable)

def rotate90(tile):
    return {Point2((2 - p[1], p[0])) for p in tile}

def flipX(tile):
    return {Point2((2 - p[0], p[1])) for p in tile}

def build_cuopt(placements, tx, ty, requiredCount):
    numVars = len(placements)
    K = len(requiredCount)

    # --- cuOpt Problem API ---
    problem = Problem()

    # Create variables
    vars = []
    for t in range(len(placements)):
        var = problem.addVariable(lb=0., ub=1., vtype=INTEGER, name="p"+str(t))
        vars.append(var)

    # Build constraints

    # Coverage constraints
    for x in range(tx):
        for y in range(ty):
            cellexpr = sum(vars[j] for j, pl in enumerate(placements) if Point2((x, y)) in pl["cells"])
            problem.addConstraint(cellexpr <= 1.)

    # Tile count constraints
    for t in range(K):
        cellexpr = sum(vars[j] for j, pl in enumerate(placements) if pl["tileId"] == t)
        problem.addConstraint(cellexpr == requiredCount[t])

    # Symmetry breaking: force tile 0 to cover (0,0), if we need tile 0
    anchor_vars = []
    if requiredCount[0] != 0:
        anchor_vars = [vars[j] for j, pl in enumerate(placements)
                       if pl["tileId"] == 0 and Point2((0,0)) in pl["cells"]]
        if anchor_vars:
            problem.addConstraint(sum(anchor_vars) >= 1.)

    # Objective: trivial feasibility
    if anchor_vars:
        problem.setObjective(sum(anchor_vars))
    else:
        problem.setObjective(sum(vars))

    # Options - feasibility only
    settings = SolverSettings()
    settings.set_parameter(CUOPT_FIRST_PRIMAL_FEASIBLE, "true")
    settings.set_parameter(CUOPT_CROSSOVER, "false")
    settings.set_parameter(CUOPT_TIME_LIMIT, "60")

    # Solve
    problem.solve(settings)
    if problem.Status in (MILPTerminationStatus.FeasibleFound, MILPTerminationStatus.Optimal):
        solution = [var.Value for var in vars]
        chosen = []
        for j, val in enumerate(solution):
            if val > 0.5:  # selected
                chosen.append(placements[j])
        board = [["." for _ in range(tx)] for _ in range(ty)]
        for pl in chosen:
            for (x, y) in pl["cells"]:
                board[y][x] = str(pl["tileId"])
        for row in board:
            print("".join(row))

        return True
    else:
        return False


def main():
    tiles = []
    tilecounts = []
    for t in range(6):
        tile = set()
        input()  # consume index line
        for y in range(3):
            s = input().strip()
            for x, ch in enumerate(s):
                if ch == "#":
                    tile.add(Point2((x, y)))

        tilecounts.append(len(tile))

        # Generate orientations
        orients = set()
        tmp = tile.copy()
        for flip in range(2):
            for rot in range(4):
                orients.add(frozenset(tmp))
                tmp = rotate90(tmp)
            tmp = flipX(tmp)

        for orient in orients:
            tiles.append({"tileId": t, "cells": orient})

        input()  # consume blank line

    part1 = 0
    while True:
        try:
            s = input().strip()
        except EOFError:
            break
        m = INPUT_R.match(s)
        if not m:
            continue
        tx, ty = int(m[1]), int(m[2])

        nums = []
        tilecount = 0
        pixelcount = 0
        for i in range(6):
            n = int(m[i+3])
            nums.append(n)
            tilecount += n
            pixelcount += tilecounts[i] * n

        # Check trivial tiling (everything on 3x3 grid)
        if (tx // 3 * ty // 3 > tilecount):
            print(f"{s} : Trivial tiling found")
            part1 += 1
        elif (pixelcount > tx * ty):
            print(f"{s} : Trivially invalid tiling found")
        else:
            # Generate placements
            placements = []
            for orient in tiles:
                for x in range(tx - 2):
                    for y in range(ty - 2):
                        offset_cells = [Point2((c[0]+x, c[1]+y)) for c in orient["cells"]]
                        placements.append({"tileId": orient["tileId"], "cells": offset_cells})

            if build_cuopt(placements, tx, ty, nums):
                print(f"{s} : Valid tiling found")
                part1 += 1
            else:
                print(f"{s} : No valid tiling found")

    print("Part 1:", part1)
    print("Part 2:", 0)

if __name__ == "__main__":
    main()
