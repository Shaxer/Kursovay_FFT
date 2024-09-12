#ifndef HELPERS_H
#define HELPERS_H

#include <string>
#include <vector>

namespace helpers {

bool ReadTimeSeriesFromFile(
    const std::string& path,
    std::vector<std::pair<long double, long double>>& data,
    size_t& discretization
);

}

#endif // HELPERS_H
