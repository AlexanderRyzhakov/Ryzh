#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <map>

#include <QFile>
#include <QString>

#include "global_consts.h"

using Dots = std::map<double, double>;

struct Band {
    Dots pos_to_mod{};
};

struct File {
    double position {};
    QFile file;
};

class MsuGs {
public:
    MsuGs ();

    const QString& GetSerialNumber() const;
    const QString& GetRhNumber() const;

    void SetSerialNumber(QString&& text);
    void SetRhNumber(QString&& text);

    void AddDots(int channel, Dots&& dots);

    void AddDot(int channel, double pos, double mod);

    Band& GetBand(int channel);

    void ClearBands();

private:
    QString s_n_ {"init"};
    QString rh_s_n_{"init"};
    std::vector<Band> bands_ {};
};

struct PlotRange {
    double min_pos {};
    double max_pos{};
    std::vector<double> positions;
};

class FocusManager {
public:
    double GetGSFocus() const;
    double GetCollimatorFocus() const;
    double GetTargetCollimatorFocus() const;

    void SetGSFocus(const QString& text);
    void SetCollimatorFocus(const QString& text);
    void SetTargetCollimatorFocus(const QString& text);

    PlotRange GetFocusRange(MsuGs& gs) const;

private:
    double focus_gs_ {275.};
    double focus_collimator_ {3000.};
    double focus_target_ {375.};
};

static const Dots kDummyPositions { {10., .05}, {20., .1}, {30.,0.25}, {40., 0.5}, {60., .45}, {70., .3}, {80., .2}, {100., .1}, {110., 0.}};
