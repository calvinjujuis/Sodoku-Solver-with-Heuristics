#include <string>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <time.h>
#include <cmath>
#include <random>

using namespace std;

#define BOARDSIZE 9
#define BOXSIZE 3

static int numNode = 0;

vector<vector<int>> findUnfilled(vector<vector<int>> &board) {
    vector<vector<int>> ret;
    for (int i = 0; i < BOARDSIZE; ++i) {
        for (int j = 0; j < BOARDSIZE; ++j) {
            if (board[i][j] == 0) {
                ret.push_back({i, j});
            }
        }
    }
    return ret;
}

unordered_set<int> getAvailableDigits(const vector<int> &slot, const vector<vector<int>> &board) {
    unordered_set<int> digits = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    int x = slot[0];
    int y = slot[1];
    // row
    for (auto r : board[x]) {
        digits.erase(r);
    }
    // col
    for (int i = 0; i < 9; ++i) {
        digits.erase(board[i][y]);
    }
    // box
    int rowIdx = x - x % BOXSIZE;
    int colIdx = y - y % BOXSIZE;
    for (int r = 0; r < BOXSIZE; ++r) {
        for (int c = 0; c < BOXSIZE; ++c) {
            digits.erase(board[rowIdx+r][colIdx+c]);
        }
    }
    return digits;
}

// forwardCheck returns false if placing digit on slot would cause another empty cell on board to have an empty domain
bool checkForwardSuccess(vector<int> slot, int digit, const vector<vector<unordered_set<int>>> &allAvailableDigits) {
    int x = slot[0];
    int y = slot[1];
    // row
    for (int i = 0; i < BOARDSIZE; ++i) {
        if (i == x) continue;
        unordered_set<int> availableDigits = allAvailableDigits[i][y];
        if (availableDigits.size() == 1 && availableDigits.find(digit) != availableDigits.end()) return false;
    }
    // col
    for (int i = 0; i < BOARDSIZE; ++i) {
        if (i == y) continue;
        unordered_set<int> availableDigits = allAvailableDigits[x][i];
        if (availableDigits.size() == 1 && availableDigits.find(digit) != availableDigits.end()) return false;
    }
    // box
    int rowIdx = x - x % BOXSIZE;
    int colIdx = y - y % BOXSIZE;
    for (int r = 0; r < BOXSIZE; ++r) {
        for (int c = 0; c < BOXSIZE; ++c) {
            if (rowIdx + r == x && colIdx + c == y) continue;
            unordered_set<int> availableDigits = allAvailableDigits[rowIdx+r][colIdx+c];
            if (availableDigits.size() == 1 && availableDigits.find(digit) != availableDigits.end()) return false;
        }
    }
    return true;
}

// order unfilled cells by their constrained factor and get the most constrained cell(s)
// order by the number of unique available digit from the row, col, and box that a cell belongs to
vector<vector<int>> getMostConstrained(const vector<vector<int>> &unfilled, const vector<vector<int>> &board) {
    unordered_map<int, int> order;
    for (int i = 0; i < unfilled.size(); ++i) {
        unordered_set<int> digits = getAvailableDigits(unfilled[i], board);
        order[i] = digits.size();
    }

    auto minNumDigit = min_element(order.begin(), order.end(), 
        [](const auto &slot1, const auto &slot2) {
            return slot1.second < slot2.second;
        });
    
    vector<vector<int>> mostConstrainedSlots;
    for (auto i : order) {
        if (i.second == minNumDigit->second) mostConstrainedSlots.push_back(unfilled[i.first]);
    }
    
    return mostConstrainedSlots;
}

// order slots by their constraing factor and get the most constraing slot(s)
// order by the number of empty cells from the row, col, and box that a cell belongs to
vector<vector<int>> getMostConstraing(const vector<vector<int>> &slots, const vector<vector<int>> &board) {
    unordered_map<int, int> order;

    for (int i = 0; i < slots.size(); ++i) {
        int x = slots[i][0];
        int y = slots[i][1];
        int total = -3;
        // row
        for (auto r : board[x]) {
            if (r == 0) ++total;
        }
        // col
        for (int i = 0; i < 9; ++i) {
            if (board[i][y] == 0) ++total;
        }
        // box
        int rowIdx = x - x % BOXSIZE;
        int colIdx = y - y % BOXSIZE;
        for (int r = 0; r < BOXSIZE; ++r) {
            for (int c = 0; c < BOXSIZE; ++c) {
                if (board[rowIdx+r][colIdx+c] == 0) ++total;
            }
        }
        order[i] = total;
    }

    auto maxNumEmpty = max_element(order.begin(), order.end(), 
        [](const auto &slot1, const auto &slot2) {
            return slot1.second < slot2.second;
        });

    vector<vector<int>> mostConstrainingSlots;
    for (auto i : order) {
        if (i.second == maxNumEmpty->second) mostConstrainingSlots.push_back(slots[i.first]);
    }
    
    return mostConstrainingSlots;
}

// order digits by their constraining factor on other empty cells within slot's row, col and box
vector<int> orderDigitsByLeastConstraing(vector<int> slot, unordered_set<int> digits, const vector<vector<unordered_set<int>>> &allAvailableDigits) {
    vector<pair<int, int>> order;
    for (auto i : digits) {
        order.push_back(make_pair(i, 0));
    }
    int x = slot[0];
    int y = slot[1];
    // row
    for (int i = 0; i < BOARDSIZE; ++i) {
        if (i == x) continue;
        // if a digit is not in the domain of another empty cell, increment its least constraining factor
        unordered_set<int> availableDigits = allAvailableDigits[i][y];
        for (int i = 0; i < order.size(); ++i) {
            if (availableDigits.count(order[i].first) == 0) { 
                ++order[i].second;
            }
        }
    }
    // col
    for (int i = 0; i < BOARDSIZE; ++i) {
        if (i == y) continue;
        unordered_set<int> availableDigits = allAvailableDigits[x][i];
        for (int i = 0; i < order.size(); ++i) {
            if (availableDigits.count(order[i].first) == 0) { 
                ++order[i].second;
            }
        }
    }
    // box
    int rowIdx = x - x % BOXSIZE;
    int colIdx = y - y % BOXSIZE;
    for (int r = 0; r < BOXSIZE; ++r) {
        for (int c = 0; c < BOXSIZE; ++c) {
            if (rowIdx + r == x && colIdx + c == y) continue;
            unordered_set<int> availableDigits = allAvailableDigits[rowIdx+r][colIdx+c];
            for (int i = 0; i < order.size(); ++i) {
            if (availableDigits.count(order[i].first) == 0) { 
                ++order[i].second;
            }
        }
        }
    }

    sort(order.begin(), order.end(), 
        [](const auto &digit1, const auto &digit2) {
            if (digit1.second != digit2.second) {
                return digit1.second > digit2.second;
            }
            // break tie randomly
            return std::rand() % 2 == 0;
        });
    
    vector<int> leastConstrainingInOrder;
    for (auto i : order) {
        leastConstrainingInOrder.push_back(i.first);
    }

    return leastConstrainingInOrder;
}

// get the available digits for every empty cell on the board
vector<vector<unordered_set<int>>> getAllAvailableDigits(const vector<vector<int>> &unfilled, const vector<vector<int>> &board) {
    vector<vector<unordered_set<int>>> allDigits(9, vector<unordered_set<int>>(9));
    for (int i = 0; i < unfilled.size(); ++i) {
        unordered_set<int> digits = getAvailableDigits(unfilled[i], board);
        allDigits[unfilled[i][0]][unfilled[i][1]] = digits;
    }
    return allDigits;
}


// ***** 3 methods *****


// solve with only Backtracking
bool solve1(vector<vector<int>> &board) {
    ++numNode;
    vector<vector<int>> unfilledCells = findUnfilled(board);
    if (!unfilledCells.size()) return true;

    int randomSlotIdx = rand() % unfilledCells.size();
    vector<int> slot = unfilledCells[randomSlotIdx];
    unordered_set<int> availableDigits = getAvailableDigits(slot, board);

    while (availableDigits.size()) {
        int randomDigitIdx = rand() % availableDigits.size();
        int randomDigit = *next(availableDigits.begin(), randomDigitIdx);
        availableDigits.erase(randomDigit);

        board[slot[0]][slot[1]] = randomDigit;

        if (solve1(board)) return true;

        board[slot[0]][slot[1]] = 0;
    }

    return false;
}

// solve with Backtracking + Forward Checking
bool solve2(vector<vector<int>> &board) {
    ++numNode;
    vector<vector<int>> unfilledCells = findUnfilled(board);
    if (!unfilledCells.size()) return true;

    int randomSlotIdx = rand() % unfilledCells.size();
    vector<int> slot = unfilledCells[randomSlotIdx];
    unordered_set<int> availableDigits = getAvailableDigits(slot, board);

    vector<vector<unordered_set<int>>> allAvailableDigits = getAllAvailableDigits(unfilledCells, board);

    while (availableDigits.size()) {
        int randomDigitIdx = rand() % availableDigits.size();
        int randomDigit = *next(availableDigits.begin(), randomDigitIdx);
        availableDigits.erase(randomDigit);

        if (!checkForwardSuccess(slot, randomDigit, allAvailableDigits)) continue;

        board[slot[0]][slot[1]] = randomDigit;
        if (solve2(board)) return true;

        board[slot[0]][slot[1]] = 0;
    }

    return false;
}

// solve with Backtracking + Forward Checking + 3 Heuristics
bool solve3(vector<vector<int>> &board) {
    ++numNode;
    vector<vector<int>> unfilledCells = findUnfilled(board);
    if (!unfilledCells.size()) return true;

    vector<int> slot;
    // 1: get the most constrained slots (where #available digits is the smallest)
    vector<vector<int>> mostConstrainedSlots = getMostConstrained(unfilledCells, board);
    if (mostConstrainedSlots.size() == 1) {
        slot = mostConstrainedSlots[0];
    } else {
        // 2: break tie with finding the most constraining slots
        vector<vector<int>> mostContraining = getMostConstraing(mostConstrainedSlots, board);
        if (mostContraining.size() > 0) {
            // break tie by random
            slot = *next(mostContraining.begin(), rand() % mostContraining.size());
        }
    }

    unordered_set<int> availableDigits = getAvailableDigits(slot, board);

    vector<vector<unordered_set<int>>> allAvailableDigits = getAllAvailableDigits(unfilledCells, board);

    // 3: order available digits by least constraining value to most
    vector<int> digits = orderDigitsByLeastConstraing(slot, availableDigits, allAvailableDigits);

    while (digits.size()) {

        int digit = digits[0];
        digits.erase(digits.begin());
        
        if (!checkForwardSuccess(slot, digit, allAvailableDigits)) continue;

        board[slot[0]][slot[1]] = digit;
        if (solve3(board)) return true;

        board[slot[0]][slot[1]] = 0;
    }

    return false;
}


int main(int argc, char *argv[]) {

    if (argc != 4 ||(!strcmp(argv[1], "B") && !strcmp(argv[1], "FC") && !strcmp(argv[1], "H")) ||
        (!strcmp(argv[1], "easy") && !strcmp(argv[1], "medium") || !strcmp(argv[1], "hard") || !strcmp(argv[1], "evil"))) {
        cout << "Usage: ./program [B|FC|H] [easy|medium|hard|evil] [repeat]" << endl;
        return 1;
    }

    vector<vector<int>> easy = 
    {{0, 5, 8, 0, 6, 2, 1, 0, 0},
     {0, 0, 2, 7, 0, 0, 4, 0, 0},
     {0, 6, 7, 9, 0, 1, 2, 5, 0},
     {0, 8, 6, 3, 4, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 7, 6, 8, 9, 0},
     {0, 2, 9, 6, 0, 8, 7, 4, 0},
     {0, 0, 3, 0, 0, 4, 9, 0, 0},
     {0, 0, 5, 2, 9, 0, 3, 8, 0}
    };

    vector<vector<int>> medium = 
    {{8, 3, 0, 6, 0, 0, 0, 0, 7},
     {0, 0, 7, 0, 2, 0, 0, 5, 0},
     {0, 2, 1, 0, 0, 9, 0, 8, 0},
     {6, 0, 0, 0, 8, 0, 0, 0, 9},
     {0, 0, 0, 4, 6, 5, 0, 0, 0},
     {3, 0, 0, 0, 9, 0, 0, 0, 2},
     {0, 8, 0, 2, 0, 0, 3, 9, 0},
     {0, 5, 0, 0, 4, 0, 2, 0, 0},
     {2, 0, 0, 0, 0, 8, 0, 1, 6}
    };

    vector<vector<int>> hard =
    {{1, 0, 0, 0, 3, 0, 0, 0, 0},
     {0, 6, 2, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 7, 0, 2, 8, 0, 4},
     {0, 7, 0, 1, 4, 0, 0, 0, 2},
     {0, 4, 0, 0, 0, 0, 0, 9, 0},
     {8, 0, 0, 0, 5, 6, 0, 7, 0},
     {6, 0, 9, 8, 0, 7, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 2, 1, 0},
     {0, 0, 0, 0, 6, 0, 0, 0, 9}
    };

    vector<vector<int>> evil =
    {{0, 1, 0, 0, 0, 0, 0, 0, 6},
     {9, 0, 0, 2, 0, 0, 0, 0, 0},
     {7, 3, 2, 0, 4, 0, 0, 1, 0},
     {0, 4, 8, 3, 0, 0, 0, 0, 2},
     {0, 0, 0, 0, 0, 0, 0, 0, 0},
     {3, 0, 0, 0, 0, 4, 6, 7, 0},
     {0, 9, 0, 0, 3, 0, 5, 6, 8},
     {0, 0, 0, 0, 0, 2, 0, 0, 1},
     {6, 0, 0, 0, 0, 0, 0, 3, 0}
    };

    map<string, vector<vector<int>>> levels = {
        {"easy", easy},
        {"medium", medium},
        {"hard", hard},
        {"evil", evil}
    };
    vector<vector<int>> &level = levels[argv[2]];

    int REPEAT = atoi(argv[3]);

    vector<double> times;
    vector<int> nodes;

    for (int i = 0; i < REPEAT; ++i) {
        vector<vector<int>> copy = level;

        clock_t start = clock();
        bool result;
        if (!strcmp(argv[1], "B")) {
            result = solve1(copy);
        } else if (!strcmp(argv[1], "FC")) {
            result = solve2(copy);
        } else if (!strcmp(argv[1], "H")) {
            result = solve3(copy);
        }  
        clock_t end = clock();

        double time = double(end - start) / CLOCKS_PER_SEC;
        times.push_back(time);

        cout << "time: " << time << endl;
        cout << "#nodes: " << numNode << endl;
        nodes.push_back(numNode);
        numNode = 0;

        if (!result) { 
            cout << "Failed to solve" << endl;
            return 1;
        }

        cout << "REPEAT " << i + 1 << ":" << endl;
        for (auto r : copy) {
            for (auto c : r) {
                cout << c << " ";
            }
            cout << endl;
        }
    }

    double totalTime, mean, sd = 0;
    double totalNode, meanNode, sdNode = 0;

    cout << "nodes: ";
    for (auto n : nodes) {
        cout << n << " ";
    }
    cout << endl;

    for (int i = 0; i < REPEAT; ++i) {
        totalTime += times[i];
        totalNode += nodes[i];
    }

    mean = totalTime / REPEAT;
    meanNode = totalNode / REPEAT;

    for (int i = 0; i < REPEAT; ++i) {
        sd += pow(times[i] - mean, 2);
        sdNode += pow(nodes[i] - meanNode, 2);
    }

    sd = sqrt(sd / REPEAT);
    sdNode = sqrt(sdNode / REPEAT);

    cout << "avg time: " << mean << endl;
    cout << "sd: " << sd << endl;

    cout << "avg node: " << meanNode << endl;
    cout << "sd: " << sdNode << endl; 

    return 0;
}