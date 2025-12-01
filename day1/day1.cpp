#include <iostream>
#include <string>
#ifdef _WIN32
#include <conio.h>
#else
static inline void _getch() {}
#endif

using namespace std;

int main()
{
    int dial = 50;
    int result1 = 0;
    int result2 = 0;

    while (!cin.eof()) {
        string line;
        cin >> line;
		char dir = line[0];
		int move = stoi(line.substr(1));
        for (int i = 0; i < move; ++i) {
            if (dir == 'L') {
                --dial;
            }
            else {
				++dial;
            }
            if (dial == -1) {
                dial = 99;
			}
            if (dial == 100) {
                dial = 0;
            }
            if (dial == 0) {
                ++result2;
            }
        }
        if (dial == 0) {
            ++result1;
        }
    }

	std::cout << "Result 1: " << result1 << std::endl;
    std::cout << "Result 2: " << result2 << std::endl;
    _getch();
}
