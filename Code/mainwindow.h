#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include "AppModel.h"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

using models::AppModel;
using QtCharts::QChart;
using QtCharts::QChartView;
using QtCharts::QLineSeries;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void handleClick();

    void handleShowPhases();
    void handleShowAmplitudes();

private:
    void HandleModeChange();

    enum SelectOptions {
        Nothing,
        Amplitudes,
        Phases
    };

    int selected{Nothing};

    Ui::MainWindow *ui;
    AppModel* model;
    QPushButton* file_selector;
    QPushButton* build_phases_spectr;
    QPushButton* build_amplitudes_spectr;

    QLabel* discretization;

    QLineSeries* series{nullptr};
    QChart* ch;
    QChartView* chartView;

    QChart* bar_ch;
    QChartView* barView;


    QLineSeries* curr_series{nullptr};
};

#endif // MAINWINDOW_H
