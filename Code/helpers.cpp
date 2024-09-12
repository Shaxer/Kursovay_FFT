#include "helpers.h"
#include <fstream>

namespace helpers {

//Функция считывания серии из файла
bool ReadTimeSeriesFromFile(
    const std::string& path,
    std::vector<std::pair<long double, long double>>& data,
    size_t& discretization
) {
    std::ifstream in(path);
    if (!in) {
        return false;
    }
    long double time, signal;
    data.clear();
    if (!(in >> discretization)) {
        return false;
    }
    while (in >> time >> signal) {
        data.emplace_back(time, signal);
    }
    return true;
}

}
