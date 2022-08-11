#include "analisator.h"

#include <algorithm>
#include <optional>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <cstdlib>

#include "fft.h"
#include "fft_settings.h"

#include <QIODevice>

struct Base8Bit {
    uint8_t a { 0 };
    uint8_t b { 0};
    uint8_t c { 0 };
    uint8_t d { 0 };
    uint8_t e { 0 };
};

struct Base16Bit {
    uint16_t A { 0 };
    uint16_t B { 0 };
    uint16_t C { 0 };
    uint16_t D { 0 };
};

class AnalisatorThrower : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

#define DEBUG_THROWER_POSITION std::string(__FILE__) + ": " + std::to_string(__LINE__) + " debug terminator"

std::vector<uint16_t> Convert8BitTo10Bit (const std::vector<uint8_t>& src) {
    std::vector<uint16_t> result;

    size_t start {};
    for (; start < src.size(); ++start) {
        if (src[start] != 0) {
            break;
        }
    }

    result.reserve(0.8 * (src.size() - start) + 1);

    uint16_t cur {};
    for (size_t i = start; i < src.size(); i += 5) {
        // 0 10bit: 0 & 1 8bit
        cur = src[i];
        cur <<= 2;
        uint8_t bits = src[i + 1] & 0b1100'0000;
        bits >>= 6;
        cur = cur | bits;
        result.push_back(cur);

        // 1 10bit: 1 & 2 8bit
        cur = 0;
        cur = src[i + 1] & 0b0011'1111;
        cur <<= 4;
        bits = src[i + 2] & 0b1111'0000;
        bits >>= 4;
        cur = cur | bits;
        result.push_back(cur);

        // 2 10bit: 2 & 3 8bit
        cur = 0;
        cur = src[i +2] & 0b00000'1111;
        cur <<= 6;
        bits = src[i + 3];
        bits >>= 2;
        cur = cur | bits;
        result.push_back(cur);

        // 3 10bit: 3 & 4 8bit
        cur = 0;
        cur = src[i + 3] & 0b0000'0011;
        cur <<= 8;
        cur = cur | src[i + 4];
        result.push_back(cur);
    }

    return result;
}

template <typename T>
void Sobel(std::vector<T>& src) {
    if (src.size() < 3) {
        return;
    }
    std::vector<T> tmp(src.size());
    for (int i = 1; i < static_cast<int>(src.size()) - 1; ++i) {
        tmp[i] = -1. * (src[i - 1]) + 1. * (src[i + 1]);
    }
    src.swap(tmp);
}

template <typename ForwIt>
double CalculateMedium(ForwIt left, ForwIt right) {
    double sum {};
    double amount = 1. * std::distance(left, right);
    for (; left < right; ++left) {
        sum += (*left) / amount;
    }
    return sum;
}

template <typename Type>
void VectorToFile(const std::vector<Type>& src, const std::string& name) {
    std::ofstream out(name + ".txt");
    for (auto elem: src) {
        out << elem << '\n';
    }
}

template <typename Type>
double CalculateSigma(const std::vector<Type>& src) {
    auto medium = CalculateMedium(src.begin(), src.end());
    double sq_sum {};
    for (auto elem: src) {
        sq_sum += 1. * (elem - medium) * 1. * (elem - medium) / (1. * src.size());
    }
    return std::sqrt(sq_sum);
}

template <typename T>
void CutNoise(std::vector<T>& data) {
    auto sigma = CalculateSigma(data);
    for (int i = 0; i < static_cast<int>(data.size()); ++i) {
        if (std::abs(data[i]) < 3. * sigma) {
            data[i] = .0;
        }
    }
}

template<typename T>
T Mediana(T* left, T* right) {
    std::vector<T> vec;
    vec.reserve(right - left + 1);
    for (; left <= right; ++left) {
        vec.push_back(*left);
    }
    std::sort(vec.begin(), vec.end());
    return vec[vec.size() / 2];
}

template<typename T>
double MedianaAndNoiseCut(std::vector<T>& data, uint8_t radius) {
    if (data.size() < radius * 2 + 1u) {
        return {};
    }
    auto toltal_medium = CalculateMedium(data.begin(), data.end());
    auto sigma = CalculateSigma(data);
    std::vector<T> tmp(data.size(), T(toltal_medium));

    for (int i = radius; i < static_cast<int>(data.size()) - radius; ++ i) {
        auto mediana = Mediana(&data[i - radius], &data[i + radius]);
        tmp[i] = std::abs(mediana - toltal_medium) < 3 * sigma ? toltal_medium: mediana;
    }
    data.swap(tmp);
    return toltal_medium;
}


// **************************   CacheMiraLine
void CacheMiraLine::Clear() {
    init_ = false;
    MiraPos tmp;
    mira_pos_ = tmp;
}

void CacheMiraLine::SetMiraCenter(uint8_t line_big_bit, uint8_t line_small_bit, uint8_t scan) {
    if (line_big_bit < 2) {
        mira_pos_.line_begin_big_bit = 0;
        mira_pos_.line_begin_small_bit = 50;
    } else {
        mira_pos_.line_begin_big_bit = line_big_bit - 1;
    }

    mira_pos_.line_begin_small_bit = line_small_bit;
    mira_pos_.scan = scan;
    init_ = true;
}

const MiraPos& CacheMiraLine::GetStartPos() const {
    return mira_pos_;
}

bool CacheMiraLine::Empty() {
    return !init_;
}


// **************************   Analisator
Analisator::Analisator(QFile& file) : file_(file.fileName().toLocal8Bit(), std::ios::binary), file_name_(file.fileName().toLocal8Bit())
{
    auto dot_pos = file_name_.find_last_of('.');
    file_name_.resize(dot_pos);
}

void Analisator::SetSaveImg(bool save) {
    save_img_ = save;
}

void Analisator::SetSaveFFT(bool save) {
    save_fft_ = save;
}

std::optional<std::vector<double>> Analisator::CalculateMod()
{
    if (!file_) {
        Print("Open file failed");
        return std::nullopt;
    }
    std::vector<double> result(kBandAmount);
    try {
        if (gMiraCache.Empty()) {
          auto mira_pos = LookForMira();
          gMiraCache.SetMiraCenter(mira_pos.line_begin_big_bit, mira_pos.line_begin_small_bit, mira_pos.scan);
          Print("Mira position cached:(center) Scan: " + QString::number(mira_pos.scan) +
            ". Line: " + QString::number(mira_pos.LineToInt16()) +
                " (Line big byte = " + QString::number(mira_pos.line_begin_big_bit) + " Line small byte = " + QString::number(mira_pos.line_begin_small_bit) + ")");
        }
        auto common_slice = MakeCommonSlice();
        std::vector<Slice> slices = MakeSlices(common_slice);

        for (size_t i = 0; i < slices.size(); ++i) {
            if (save_img_) {
                SavePPM(file_name_ + '-' + std::to_string(kFirstIKnumber + i), slices[i]);
            }
            result[i] = CalculateModInSlice(slices[i], i);
        }

    }
    catch (const AnalisatorThrower& deb_err) {
        Print(deb_err.what());
        return std::nullopt;
    }

    return result;
}

double Analisator::CalculateModInSlice(const Slice& source, int indx) {
    if (source.size() < 3) {
        throw AnalisatorThrower("Analisator::CalculateModInSlice: source to small");
    }

    std::vector<std::vector<complex_type>> destination;
    destination.resize(source.size(), std::vector<complex_type>(source[0].size()));

    const char* err = NULL;
    bool fft_comlete = simple_fft::FFT(source, destination, source.size(), source[0].size(), err);

    if (!fft_comlete) {
        Print("fft fail");
        return -0.1;
    }

    if (save_fft_) {
        SavePPM(file_name_ + "-FFT-" + std::to_string(kFirstIKnumber + indx), destination);
    }

        double max_wave {};
        for (const auto& pix : destination[2]) {
            max_wave += std::abs(pix);
        }
        size_t max_freq = destination.size() / 2;

        std::vector<double> spectre_one_dimension;
        spectre_one_dimension .reserve(max_freq);
        for (size_t i = max_freq * kMinFreqToAnalise; i < max_freq * kMaxFreqToAnalise; ++i) {
            double avrg_spectr {};
            for (const auto& pix : destination[i]) {
                avrg_spectr += std::abs(pix) / max_wave;
            }
            spectre_one_dimension.push_back(avrg_spectr);
        }

        auto max_iter = std::max_element(spectre_one_dimension.begin(), spectre_one_dimension.end());
        if (max_iter == spectre_one_dimension.end() || *max_iter >= 1.) {
            return .0;
        }
        return *max_iter;
}


std::vector<Analisator::Slice> Analisator::MakeSlices (const Slice& src) {
    if (src.empty()) {
        AnalisatorThrower("Attempt to slice emprty source");
    }
    uint16_t band_num_cur{kFirstIKnumber};
    std::vector<Slice> result(kBandAmount);

    std::vector<size_t> start_indx;
    start_indx.reserve(kBandAmount);

    for (size_t i = 0; i < src[0].size() && band_num_cur <= kBandAmount + 3u; ++i) {
        if (src[0][i] == band_num_cur) {
            start_indx.push_back(i);
            ++band_num_cur;
        }
    }

    for (size_t row = 0; row < src.size(); ++row) {
        for (size_t band = 0; band <  kBandAmount; ++band) {
//            result[band].emplace_back(&src[row][start_indx[band]] + 5, &src[row][start_indx[band]] + 187); // full line
            result[band].emplace_back(&src[row][start_indx[band]] + 32, &src[row][start_indx[band]] + 160); // 128 elemetns in line for fft

        }
    }

    return result;
}

Analisator::Slice Analisator::MakeCommonSlice() {
    Analisator::Slice result;
    file_.clear();
    file_.seekg(0);
    MiraPos position {};
    const auto start_pos = gMiraCache.GetStartPos();
    while (NextPSP()) {
        file_.readsome(reinterpret_cast<char*>(&position), 3);
        if (position == start_pos) {
            break;
        }
    }
    size_t lines_read{};
    while (NextPSP() && lines_read++ < 512) {
        file_.readsome(reinterpret_cast<char*>(&position), 3);
        file_.ignore(kOffsets.dist_from_line_small_bit_to_ten_bit_data);
        auto line = ExtractTenBitData(4550);
        result.push_back(std::move(line));
    }
    return result;
}

void Analisator::SetOutput(QTextBrowser *browser)
{
    browser_ = browser;
}

void Analisator::Print(const QString& text) const {
    if (browser_) {
        browser_->append(text);
    }
}

bool Analisator::NextPSP () {
    uint8_t psp_pos = 0;
    char c { };
    while (file_) {
        file_.get(c);
        if (uint8_t(c) == kPsp.nums[psp_pos]) {
            if (psp_pos == 7) {
                psp_pos = 0;
                return true;
            } else {
                ++psp_pos;
            }
        } else {
            psp_pos = 0;
        }
    }
return false;
}

std::vector<uint16_t> Analisator::ExtractTenBitData(size_t bytes_to_read) {
    std::vector<uint8_t> raw_data;

    int full_blocks = bytes_to_read / kOffsets.data_block;
    int partial_all = bytes_to_read - full_blocks * kOffsets.data_block;
    int partial_data_block = partial_all > (kOffsets.data_block - kOffsets.vd_block) ? (kOffsets.data_block - kOffsets.vd_block) : partial_all;

    raw_data.resize(full_blocks * (kOffsets.data_block - kOffsets.vd_block) + partial_data_block);

    for (int i = 0; i < full_blocks; ++i) {
        char* current_pos = reinterpret_cast<char*>(&raw_data[i * (kOffsets.data_block - kOffsets.vd_block)]);
        file_.read(current_pos, kOffsets.data_block - kOffsets.vd_block);
        file_.ignore(kOffsets.vd_block);
    }
    char* current_pos = reinterpret_cast<char*>(&raw_data[full_blocks * (kOffsets.data_block - kOffsets.vd_block)]);
    file_.read(current_pos, partial_data_block);

    return Convert8BitTo10Bit(raw_data);
}

MiraPos Analisator::LookForMira() {
    MiraPos mir_pos;
    std::vector<MiraPos> positions; // positions here
    positions.reserve(128 * 1024);
    std::vector<double> data; //data for positions
    data.reserve(128 * 1024);
    while (NextPSP ()) {
        file_.readsome(reinterpret_cast<char*>(&mir_pos), 3);
        if (!file_) {
            break;
        }
        if (mir_pos.LineToInt16() > 1024  && mir_pos.LineToInt16() < 3000 && mir_pos.scan > 0) {
            file_.ignore(kOffsets.dist_from_line_small_bit_to_ten_bit_data);
            auto line = ExtractTenBitData(620);
            file_.ignore(2000);
            auto medium_ine = CalculateMedium(std::next(line.begin(), 44), std::next(line.begin(), 144));
            if (!file_) {
                break;
            }
            data.push_back(medium_ine); // 100 elements in line
            positions.push_back(mir_pos);
        }
    }

    auto floor = MedianaAndNoiseCut(data, 100);

    size_t mira_start{};
    while (std::abs(data[mira_start] - floor) < kEpsilon && mira_start < data.size()) {
        ++mira_start;
    }
    size_t mira_end {mira_start};
    while  (std::abs(data[mira_end] - floor) > kEpsilon && mira_end < data.size()) {
        ++mira_end;
    }

    if (mira_end - mira_start < 10) {
        throw AnalisatorThrower("Mira not found");
    }

    size_t mira_middle = (mira_start + mira_end) / 2;
    return positions[mira_middle];
}

bool Analisator::SavePPM(const std::string& file, const Slice& image) {
    if (image.empty()) {
        Print("No image to save");
        return false;
    }

    std::ofstream out(file + ".ppm", std::ios::binary);
    if (!out) {
        Print("Write file failed");
        return false;
    }

    const int w = image[0].size();
    const int h = image.size();

    out << "P6" << '\n' << image[0].size() << ' ' << image.size() << '\n' << 255 << '\n';

    std::vector<char> buff(w * 3);

    for (int y = 0; y < h; ++y) {
        const auto* line = &image[y];
        for (int x = 0; x < w; ++x) {
            auto pix = uint8_t(line->operator[](x) / 4);
            buff[x * 3 + 0] = static_cast<char>(pix);
            buff[x * 3 + 1] = static_cast<char>(pix);
            buff[x * 3 + 2] = static_cast<char>(pix);
        }
        out.write(buff.data(), w * 3);
    }

    return out.good();
}

