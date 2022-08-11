#ifndef ANALISATOR_H
#define ANALISATOR_H

#include <string>
#include <vector>
#include <optional>
#include <fstream>

#include <QFile>
#include <QTextBrowser>

#include "global_consts.h"
#include "fft_settings.h"

static const uint8_t kMiraOoffset_ {120};
static const uint8_t kMiraSize {200};

#pragma pack(push, 1)
struct MiraPos {
    uint8_t line_begin_big_bit {};
    uint8_t line_begin_small_bit {};
    uint8_t scan {};

    uint16_t LineToInt16() {
        uint16_t res{};
        res = res | line_begin_big_bit;
        res <<= 8;
        res = res | line_begin_small_bit;
        return res;
    }

    bool operator==(const MiraPos& other) {
        return line_begin_big_bit == other.line_begin_big_bit
                && line_begin_small_bit == other.line_begin_small_bit
                && scan == other.scan;
    }
};
#pragma pack(pop)

class CacheMiraLine {
public:
    void Clear();

    void SetMiraCenter(uint8_t line_big_bit, uint8_t line_small_bit, uint8_t scan);

    const MiraPos& GetStartPos() const;

    bool Empty();

private:
    MiraPos mira_pos_;
    bool init_ {false};
};

[[maybe_unused]] static CacheMiraLine gMiraCache;

class Analisator
{
public:
    explicit Analisator(QFile& file);

    std::optional<std::vector<double>> CalculateMod();

    void SetOutput(QTextBrowser *browser);

    void SetSaveImg(bool save);

    void SetSaveFFT(bool save);

private:
    using Slice = std::vector<std::vector<uint16_t>>;
    void Print(const QString& text) const;

    template <typename ElemType>
    void PrintVector(const std::vector<ElemType> vec) const {
        QString res;
        for (auto num : vec) {
            if constexpr (std::is_same_v<ElemType, complex_type>) {
                res += "(";
                res += QString::number(num.real());
                res += ", ";
                res += QString::number(num.imag());
                res += ")";
            } else {
                res += QString::number(num);
            }
            res += ' ';
        }
        Print(res);
    }

    bool NextPSP ();

    MiraPos LookForMira();

    std::vector<uint16_t> ExtractTenBitData(size_t bytes_to_read);

    Slice MakeCommonSlice();

    std::vector<Slice> MakeSlices (const Slice& src);

    template <typename T>
    bool SavePPM(const std::string& file, const std::vector<std::vector<T>>& src) {
        Slice slice;
        slice.reserve(src.size());

        double max {};
        for (const auto& row : src) {
            for (auto el : row) {
                if constexpr (std::is_same_v<T, complex_type>) {
                    max  = std::abs(el) > max ? std::abs(el) : max;
                } else {
                    max = el > max ? el : max;
                }
            }
        }

        for (const auto& row : src) {
            std::vector<uint16_t> line;
            line.reserve(row.size());
            for (auto el : row) {
                double value {};
                if constexpr (std::is_same_v<T, complex_type>) {
                    value  = std::abs(el);
                } else {
                    value = el;
                }
                line.push_back(static_cast<uint16_t>(65536. * value / max));
            }
            slice.push_back(std::move(line));
        }
        return SavePPM(file, slice);
    }

    bool SavePPM(const std::string& file, const Slice& image);

    double CalculateModInSlice(const Slice& src, int indx);

private:
    QTextBrowser *browser_;
    std::ifstream file_;
    std::string file_name_;

    bool save_img_ {false};
    bool save_fft_ {false};
};

#endif // ANALISATOR_H
