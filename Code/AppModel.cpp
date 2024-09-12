#include "AppModel.h"

namespace models {

//Меняет временной ряд(вызывается когда пользователь меняет файл)
void AppModel::SetTimeSeries(const std::vector<std::pair<long double, long double>>& series) {
   time_series_ = series;
   transformer_.SetSeries(series);
}

//Функция, которая получает амплитудный спектр(вызывается при построение графика)
std::vector<long double> AppModel::GetAmplitudes(size_t second) {
    return transformer_.CalcAmplitudesSpectr(second);
}

//Функция, которая получает фазовый спектр(вызывается при построение графика)
std::vector<long double> AppModel::GetPhases(size_t second) {
    return transformer_.CalcPhasesSpectr(second);
}

//Функция, необходимая для реализации проверок
bool AppModel::Empty() {
    return time_series_.empty();
}

}
