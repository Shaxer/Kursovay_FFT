#ifndef APPMODEL_H
#define APPMODEL_H

#include <vector>
#include "FFTTransformer.h"


namespace models {

class AppModel {
public:
    void SetTimeSeries(const std::vector<std::pair<long double, long double>>& series);
    AppModel(size_t discretization) : transformer_(discretization) {}

    std::vector<long double> GetAmplitudes(size_t second);
    std::vector<long double> GetPhases(size_t second);

    bool Empty();
private:
    FastFourierTransform transformer_;
    std::vector<std::pair<long double, long double>> time_series_;
};

}


#endif // APPMODEL_H
