#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iomanip>

using namespace std;

// Constants
static const int BOARD_SIZE = 4;
static const int WIN_VALUE = 2048;
static const int CELL_WIDTH = 6;

// Tile Class
class Tile {
private:
    int value;
public:
    Tile(int v) : value(v) {}
    int getValue() const { return value; }
    void doubleValue() { value *= 2; }
};

// TileFactory Class
class TileFactory {
public:
    static Tile createTile() {
        return Tile(2); // Always spawn a 2
    }
};

// Abstract MoveCommand Class
class MoveCommand {
public:
    virtual ~MoveCommand() {}
    virtual void execute(vector<vector<Tile*>>& grid) = 0;
};

// MoveUpCommand Class
class MoveUpCommand : public MoveCommand {
private:
    vector<Tile*> compactLine(vector<Tile*> line) {
        vector<Tile*> filtered;
        for (auto& t : line) if (t && t->getValue() != 0) filtered.push_back(t);
        for (int i = 0; i < (int)filtered.size() - 1; ++i) {
            if (filtered[i]->getValue() == filtered[i + 1]->getValue()) {
                filtered[i]->doubleValue();
                delete filtered[i + 1];
                filtered[i + 1] = nullptr;
                for (int j = i + 1; j < (int)filtered.size() - 1; ++j) {
                    filtered[j] = filtered[j + 1];
                }
                filtered.pop_back();
            }
        }
        while ((int)filtered.size() < (int)line.size()) filtered.push_back(nullptr);
        return filtered;
    }
public:
    void execute(vector<vector<Tile*>>& grid) override {
        for (int c = 0; c < (int)grid.size(); ++c) {
            vector<Tile*> col;
            for (int r = 0; r < (int)grid.size(); ++r) col.push_back(grid[r][c]);
            col = compactLine(col);
            for (int r = 0; r < (int)grid.size(); ++r) grid[r][c] = col[r];
        }
    }
};

// MoveDownCommand Class
class MoveDownCommand : public MoveCommand {
private:
    vector<Tile*> compactLine(vector<Tile*> line) {
        vector<Tile*> filtered;
        for (auto& t : line) if (t && t->getValue() != 0) filtered.push_back(t);
        for (int i = 0; i < (int)filtered.size() - 1; ++i) {
            if (filtered[i]->getValue() == filtered[i + 1]->getValue()) {
                filtered[i]->doubleValue();
                delete filtered[i + 1];
                filtered[i + 1] = nullptr;
                for (int j = i + 1; j < (int)filtered.size() - 1; ++j) {
                    filtered[j] = filtered[j + 1];
                }
                filtered.pop_back();
            }
        }
        while ((int)filtered.size() < (int)line.size()) filtered.push_back(nullptr);
        return filtered;
    }
public:
    void execute(vector<vector<Tile*>>& grid) override {
        for (int c = 0; c < (int)grid.size(); ++c) {
            vector<Tile*> col;
            for (int r = 0; r < (int)grid.size(); ++r) col.push_back(grid[r][c]);
            reverse(col.begin(), col.end());
            col = compactLine(col);
            reverse(col.begin(), col.end());
            for (int r = 0; r < (int)grid.size(); ++r) grid[r][c] = col[r];
        }
    }
};

// MoveLeftCommand Class
class MoveLeftCommand : public MoveCommand {
private:
    void slideAndMergeLine(vector<Tile*>& line) {
        line = compactLine(line);
    }
    vector<Tile*> compactLine(vector<Tile*> line) {
        vector<Tile*> filtered;
        for (auto& t : line) if (t && t->getValue() != 0) filtered.push_back(t);
        for (int i = 0; i < (int)filtered.size() - 1; ++i) {
            if (filtered[i]->getValue() == filtered[i + 1]->getValue()) {
                filtered[i]->doubleValue();
                delete filtered[i + 1];
                filtered[i + 1] = nullptr;
                for (int j = i + 1; j < (int)filtered.size() - 1; ++j) {
                    filtered[j] = filtered[j + 1];
                }
                filtered.pop_back();
            }
        }
        while ((int)filtered.size() < (int)line.size()) filtered.push_back(nullptr);
        return filtered;
    }
public:
    void execute(vector<vector<Tile*>>& grid) override {
        for (int r = 0; r < (int)grid.size(); ++r) {
            slideAndMergeLine(grid[r]);
        }
    }
};

// MoveRightCommand Class
class MoveRightCommand : public MoveCommand {
private:
    void slideAndMergeLine(vector<Tile*>& line) {
        line = compactLine(line);
    }
    vector<Tile*> compactLine(vector<Tile*> line) {
        vector<Tile*> filtered;
        for (auto& t : line) if (t && t->getValue() != 0) filtered.push_back(t);
        for (int i = 0; i < (int)filtered.size() - 1; ++i) {
            if (filtered[i]->getValue() == filtered[i + 1]->getValue()) {
                filtered[i]->doubleValue();
                delete filtered[i + 1];
                filtered[i + 1] = nullptr;
                for (int j = i + 1; j < (int)filtered.size() - 1; ++j) {
                    filtered[j] = filtered[j + 1];
                }
                filtered.pop_back();
            }
        }
        while ((int)filtered.size() < (int)line.size()) filtered.push_back(nullptr);
        return filtered;
    }
public:
    void execute(vector<vector<Tile*>>& grid) override {
        for (int r = 0; r < (int)grid.size(); ++r) {
            reverse(grid[r].begin(), grid[r].end());
            slideAndMergeLine(grid[r]);
            reverse(grid[r].begin(), grid[r].end());
        }
    }
};

// Board Class
class Board {
private:
    vector<vector<Tile*>> grid;

    void spawnTile() {
        vector<pair<int, int>> emptyPos;
        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                if (!grid[r][c]) emptyPos.push_back({r, c});
            }
        }
        if (emptyPos.empty()) return;
        int idx = rand() % (int)emptyPos.size();
        grid[emptyPos[idx].first][emptyPos[idx].second] = new Tile(TileFactory::createTile());
    }

    void printHorizontalBorder() const {
        for (int i = 0; i < BOARD_SIZE; i++) {
            cout << "+";
            for (int j = 0; j < CELL_WIDTH; j++) cout << "-";
        }
        cout << "+\n";
    }

    string formatValue(int value) const {
        string valStr = (value == 0) ? "" : to_string(value);
        int space = CELL_WIDTH - (int)valStr.size();
        int left = space / 2;
        int right = space - left;
        return string(left, ' ') + valStr + string(right, ' ');
    }

    vector<vector<Tile*>> copyGrid() const {
        vector<vector<Tile*>> copy(BOARD_SIZE, vector<Tile*>(BOARD_SIZE, nullptr));
        for (int r = 0; r < BOARD_SIZE; ++r) {
            for (int c = 0; c < BOARD_SIZE; ++c) {
                if (grid[r][c]) copy[r][c] = new Tile(grid[r][c]->getValue());
            }
        }
        return copy;
    }

    bool gridsEqual(const vector<vector<Tile*>>& g1, const vector<vector<Tile*>>& g2) const {
        for (int r = 0; r < BOARD_SIZE; ++r) {
            for (int c = 0; c < BOARD_SIZE; ++c) {
                int v1 = g1[r][c] ? g1[r][c]->getValue() : 0;
                int v2 = g2[r][c] ? g2[r][c]->getValue() : 0;
                if (v1 != v2) return false;
            }
        }
        return true;
    }

    bool canMerge() const {
        for (int r = 0; r < BOARD_SIZE; ++r) {
            for (int c = 0; c < BOARD_SIZE; ++c) {
                if (grid[r][c] == nullptr) return true;
                if (r > 0 && grid[r - 1][c] && grid[r][c]->getValue() == grid[r - 1][c]->getValue()) return true;
                if (c > 0 && grid[r][c - 1] && grid[r][c]->getValue() == grid[r][c - 1]->getValue()) return true;
            }
        }
        return false;
    }

public:
    Board() {
        grid.resize(BOARD_SIZE, vector<Tile*>(BOARD_SIZE, nullptr));
    }

    ~Board() {
        for (int r = 0; r < BOARD_SIZE; ++r) {
            for (int c = 0; c < BOARD_SIZE; ++c) {
                delete grid[r][c];
            }
        }
    }

    void init() {
        srand((unsigned int)time(nullptr));
        spawnTile();
        spawnTile();
    }

    bool move(unique_ptr<MoveCommand> command) {
        auto before = copyGrid();
        command->execute(grid);
        if (!gridsEqual(before, grid)) {
            spawnTile();
            return true;
        }
        return false;
    }

    bool checkGameOver() const {
        if (canMerge()) return false;
        for (int r = 0; r < BOARD_SIZE; ++r) {
            for (int c = 0; c < BOARD_SIZE; ++c) {
                if (grid[r][c] == nullptr) return false;
            }
        }
        return true;
    }

    void print() const {
        printHorizontalBorder();
        for (int r = 0; r < BOARD_SIZE; ++r) {
            for (int c = 0; c < BOARD_SIZE; ++c) {
                cout << "|";
                if (grid[r][c]) {
                    cout << formatValue(grid[r][c]->getValue());
                } else {
                    cout << formatValue(0);
                }
            }
            cout << "|\n";
            printHorizontalBorder();
        }
    }
};

int main() {
    Board board;
    board.init();
    board.print();

    char input;
    while (true) {
        if (board.checkGameOver()) {
            cout << "Game over. No more moves available.\n";
            break;
        }

        cout << "Enter move (w: up, s: down, a: left, d: right, q: quit): ";
        cin >> input;

        if (input == 'q') break;

        unique_ptr<MoveCommand> command;
        if (input == 'w') command = make_unique<MoveUpCommand>();
        else if (input == 's') command = make_unique<MoveDownCommand>();
        else if (input == 'a') command = make_unique<MoveLeftCommand>();
        else if (input == 'd') command = make_unique<MoveRightCommand>();
        else continue;



        if (!board.move(move(command))) {
            cout << "Invalid move!\n";
        }

        board.print();
    }

    cout << "Thanks for playing!\n";
    return 0;
}
