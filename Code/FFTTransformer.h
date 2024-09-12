#ifndef FFTTRANSFORMER_H
#define FFTTRANSFORMER_H

#define _USE_MATH_DEFINES
#include <cmath>

#include <iostream>
#include <complex>
#include <vector>

#include <algorithm>

using fft_type = std::complex<long double>;

class FastFourierTransform {
public:
    FastFourierTransform(size_t frequency_discretisation) : frequency_discretisation_(frequency_discretisation) {}
    //Метод, который устанавливает точки серии для графика
    void SetSeries(const std::vector<std::pair<long double, long double>>& data) {
        data_ = data;
    }
    //Метод, который вычисляет амплитудный спектр
    std::vector<long double> CalcAmplitudesSpectr(size_t second_index) {
        std::vector<long double> amplitudes;
        auto fft_output = FFTSec(second_index);
        for (auto& elem : fft_output) {
           amplitudes.push_back(2* std::abs(elem) / 1000.);
        }
        return amplitudes;
    }
    //Метод, который вычисляет фазовый спектр
    std::vector<long double> CalcPhasesSpectr(size_t second_index) {
        auto fft_output = FFTSec(second_index);
        std::vector<long double> phases;
        //Итерируется по вектору
        for (auto& elem : fft_output) {
            phases.push_back(std::arg(elem));
        }
        return phases;
    }


private:
    // Метод, который выполняет быстрое преобразование Фурье для заданной секунды (параметр метода)
    std::vector<fft_type> FFTSec(size_t second_index) {

        std::vector<fft_type> fft_input(frequency_discretisation_, std::complex<long double>{0, 0});
        size_t first = second_index * frequency_discretisation_;  //индекс первой точки
        size_t last = std::min(data_.size(), (second_index + 1) * frequency_discretisation_);   //индекс последний точки (не включая)
        //Заполняем вектор нужными нам точками для дальнейшего преобразования Фурье
        for (size_t i = first; i < last; ++i) {
            fft_input[i] = fft_type(data_[i].second);
        }
        std::vector<fft_type> fft_output;
        FFT(fft_input, fft_output);
        return fft_output;

    }
    // T(x) = a_0 + a_1 x + ... a_{n-1}x^{n-1}
    // T_1(x) = a_0 + a_2 x + ...
    // T_2(x) = a_1 + a_3 x + ...
    // T(x) = T_1(x*x) + xT_2(x*x)
    // Выполняет Дискретное преобразование Фурье с помощью Быстрого преобразования
    // Работает за nlogn
    // ФУНКЦИЯ УМЕЕТ РАБОТАТЬ ТОЛЬКО С МАССИВАМИ РАЗМЕРА СТЕПЕНИ 2
    void FFT(
      const std::vector<fft_type>& poly,
      std::vector<fft_type>& out
    ) {
        //Инициализируем ответ нулями
        out.assign(poly.size(), std::complex<long double>{0, 0});

      // случай когда остался один коэф
      if (poly.size() == 1) {
        out[0] = poly[0];
        return;
      }
      // половинки
      // T_1(x) = a_0 + a_2 x + ...
      std::vector<fft_type> poly1;
      // T_2(x) = a_1 + a_3 x + ...
      std::vector<fft_type> poly2;

      for (size_t i = 0; i < poly.size(); ++i) {
        if (i % 2 == 0) {
          poly1.push_back(poly[i]);
        } else {
          poly2.push_back(poly[i]);
        }
      }

      std::vector<fft_type> out1, out2;
      FFT(poly1, out1);
      FFT(poly2, out2);

      fft_type x = 1;
      long double pi = 3.1415926535897932384626433832795028841971;
      long double angle = 2 * pi / (poly.size());
      fft_type delta = {std::cos(angle), std::sin(angle)};

      for (size_t i = 0; i < poly.size() / 2; ++i) {
        // T(x) = T_1(x*x) + xT_2(x*x)
        out[i] = out1[i] + x * out2[i];
        // в силу симметрии комплексных корней
        out[i + poly.size() / 2] = out1[i] - x * out2[i];
        x *= delta;
      }
    }

    size_t frequency_discretisation_;
    std::vector<std::pair<long double, long double>> data_;
};
#endif // FFTTRANSFORMER_H
