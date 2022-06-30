#pragma once

#include <cstdint>
#include <vector>

using Cell = uint8_t;
using Board = std::vector<std::vector<Cell>>;

class Life {
public:
    Life() = default;

    const Board& GenerateBoard(int width, int height);

    const Board& NextStep();

    int GetWidth() const;

    int GetHeight() const;

    void Clear();

private:
    int CountNeighbors(int row, int col) const;

    int GetNeighborValue(int row, int col) const;

private:
    Board pool_;
    Board buffer_;

    int width_ {};
    int height_;
};
