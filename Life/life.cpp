#include "life.h"

#include <stdexcept>
#include <string>
#include <random>
#include <functional>

using namespace std;

void Abort(const string &str) {
    throw std::runtime_error(str);
}

bool RandomBool() {
    static auto gen = std::bind(std::uniform_int_distribution<>(0, 1), std::default_random_engine());
    return gen();
}

void FillRandom(Board &board, int board_width, int board_height) {
    board.resize(board_height, vector<Cell>(board_width));
    for (auto &row : board) {
        for (auto &col : row) {
            col = RandomBool();
        }
    }
}

const Board& Life::GenerateBoard(int width, int height) {
    Clear();
    if (width > 0 && height > 0) {
        FillRandom(pool_, width, height);
        buffer_.resize(pool_.size(), vector<Cell>(pool_[0].size()));
        width_ = width;
        height_ = height;
    }
    return pool_;
}

void Life::Clear() {
    pool_.clear();
    buffer_.clear();
    width_ = 0;
    height_ = 0;
}

const Board& Life::NextStep() {
    bool dead_lock = true;
    for (int row = 0; row < static_cast<int>(pool_.size()); ++row) {
        for (int col = 0; col < static_cast<int>(pool_[row].size()); ++col) {
            auto neighbors = CountNeighbors(row, col);
            buffer_[row][col] = 0;
            buffer_[row][col] = neighbors == 3 ? 1 : 0;
            buffer_[row][col] = neighbors == 2 ? pool_[row][col] : buffer_[row][col];

            if (dead_lock && buffer_[row][col] != pool_[row][col]) {
                dead_lock = false;
            }
        }
    }
    if (dead_lock) {
        Abort("Dead lock");
    }
    pool_.swap(buffer_);
    return pool_;
}

int Life::GetWidth() const {
    return width_;
}

int Life::GetHeight() const {
    return height_;
}

int Life::CountNeighbors(int row, int col) const {
    int result = 0;
    for (int i = row - 1; i <= row + 1; ++i) {
        for (int j = col - 1; j <= col + 1; ++j) {
            if (row == i && col == j) {
                continue;
            }
            result += GetNeighborValue(i, j);
        }
    }
    return result;
}

int Life::GetNeighborValue(int row, int col) const {
    row = row < 0 ? static_cast<int>(pool_.size()) - 1 : row;
    row = row == static_cast<int>(pool_.size()) ? 0 : row;

    col = col < 0 ? static_cast<int>(pool_[row].size()) - 1 : col;
    col = col == static_cast<int>(pool_[row].size()) ? 0 : col;

    return pool_[row][col];
}

