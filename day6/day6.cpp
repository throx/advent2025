#include <iostream>
#include <string>
#ifdef _WIN32
#include <conio.h>
#else
static inline void _getch() {}
#endif

#include "../shared/split.h"

using namespace std;

int main()
{
    int64_t part1 = 0;
    int64_t part2 = 0;

    vector<vector<int64_t>> lines;
    vector<string> slines;
    string s;
    getline(cin, s);
    while (isdigit(ltrim(s)[0])) {
        vector<int64_t> line = split_int64s(s, ' ');
        lines.push_back(line);
        slines.push_back(s);

        getline(cin, s);
    }

    vector<string> ops = split(s, ' ', true);

    for (int i = 0; i < ops.size(); ++i) {
        int64_t tally;
        if (ops[i] == "+") {
            tally = 0;
        }
        else {
            tally = 1;
        }
        for (int j = 0; j < lines.size(); ++j) {
            if (ops[i] == "+") {
                tally = tally + lines[j][i];
            }
            else {
                tally = tally * lines[j][i];
            }
        }

        part1 += tally;
    }

    char op;
    int64_t tally;
    for (int i = 0; i < s.length(); ++i) {
        if (!isspace(s[i])) {
            op = s[i];
            tally = (op == '+') ? 0 : 1;

            cout << op << " ";
        }

        string num;
        for (int j = 0; j < slines.size(); ++j) {
            if (!isspace(slines[j][i])) {
                num = num + slines[j][i];
            }
        }

        if (num.empty()) {
            part2 += tally;
            tally = 0;
            cout << endl;
        }
        else {
            cout << num << ", ";
            if (op == '+') {
                tally += stoll(num);
            }
            else {
                tally *= stoll(num);
            }
        }
    }

    if (tally != 0) {
        part2 += tally;
        cout << endl;
    }

    cout << "Part 1: " << part1 << endl;
    cout << "Part 2: " << part2 << endl;

    cout << "Press a key to continue" << endl;
    _getch();
    return 0;
}
