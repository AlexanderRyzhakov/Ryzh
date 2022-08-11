#pragma once

#include <QString>

#include <array>
#include <regex>
#include <utility>

static const QString kSerialFile {"serial_numbers.txt"};

static const std::regex kFileFormat ("([0-9]{3})([.][d][a][t])");

static const uint8_t kBandAmount {7};

static const uint8_t kFirstIKnumber {4};

static const double kEpsilon {.001};

static const double kMinFreqToAnalise {0.15};

static const double kMaxFreqToAnalise {0.95};

// ------------------------------------------- PSP
struct PSP_64 {
    PSP_64() {
        assert(nums.size() > 0);
        for (size_t i = 0; i < nums.size(); ++i) {
            *(const_cast<uint64_t*>(&value)) = value | nums[i];
            if (i < nums.size() - 1) {
                *(const_cast<uint64_t*>(&value)) <<= 8;
            }
        }
    }
    static constexpr std::array<uint8_t, 8> nums { 0, 0, 0, 0, 0, 0, 0, 0 };
    const uint64_t value { };
};

static const PSP_64 kPsp;

// ------------------------------------------- File Structure
struct FileFormatOffsets {                      // data in single file
    uint16_t dist_from_line_small_bit_to_ten_bit_data {289};
    uint16_t data_block {100}; // ik + vd
    uint16_t vd_block {60};
    uint16_t data_lenghth {4891};
};

static const FileFormatOffsets kOffsets {}; // Change here for new file formats

static const uint8_t kBandWidth8Bit {240};



