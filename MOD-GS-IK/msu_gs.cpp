#include "msu_gs.h"

#include <QString>

#include <stdexcept>

void CheckBandNum(int number)
{
    if (number < 0 || number >= kBandAmount) {
        throw std::runtime_error("invalid channel number");
    }
}

double StrToDouble(const QString& text)
{
    bool ok  {false};
    double number = text.toDouble(&ok);
    if (!ok || number < 0 || number > 30'000) {
        throw std::runtime_error("Invalid numeric value");
    }
    return number;
}

// *** MSU GS
MsuGs::MsuGs () {
    bands_.resize(kBandAmount);
}

const QString& MsuGs::GetSerialNumber() const
{
    return s_n_;
}

const QString& MsuGs::GetRhNumber() const
{
    return rh_s_n_;
}

void MsuGs::SetSerialNumber(QString&& text)
{
    s_n_ = std::move(text);
}

void MsuGs::SetRhNumber(QString&& text)
{
    rh_s_n_ = std::move(text);
}

void MsuGs::AddDots(int channel, Dots&& dots)
{
    CheckBandNum(channel);
    bands_[static_cast<uint8_t>(channel)].pos_to_mod = std::move(dots);
}

void MsuGs::AddDot(int channel, double pos, double mod)
{
    CheckBandNum(channel);
    bands_[static_cast<uint8_t>(channel)].pos_to_mod[pos] = mod;
}

void MsuGs::ClearBands()
{
    for (auto& band : bands_) {
        band.pos_to_mod.clear();
    }
}


Band& MsuGs::GetBand(int channel)
{
    CheckBandNum(channel);
    return bands_[static_cast<uint8_t>(channel)];
}

// *** Focus Manager
double FocusManager::GetGSFocus() const
{
    return focus_gs_;
}

double FocusManager::GetCollimatorFocus() const
{
    return focus_collimator_;
}

double FocusManager::GetTargetCollimatorFocus() const
{
    return focus_target_;
}

void FocusManager::SetGSFocus(const QString& text)
{
    focus_gs_ = StrToDouble(text);
}

void FocusManager::SetCollimatorFocus(const QString& text)
{
    focus_collimator_ = StrToDouble(text);
}

void FocusManager::SetTargetCollimatorFocus(const QString& text)
{
    focus_target_ = StrToDouble(text);
}

PlotRange FocusManager::GetFocusRange(MsuGs& gs) const
{
    PlotRange range;
    auto& band = gs.GetBand(0);
    if (band.pos_to_mod.size() != 0) {
        range.min_pos = band.pos_to_mod.begin()->first;
        range.max_pos = band.pos_to_mod.rbegin()->first;
        for (const auto& [pos, _] : band.pos_to_mod) {
            range.positions.push_back(pos);
        }
    }
    return range;
}

