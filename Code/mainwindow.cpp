#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QLabel>
#include <QtCharts/QBarSeries>
#include "helpers.h"
#include <QMessageBox>
#include <QValueAxis>

using QtCharts::QBarSeries;
using QtCharts::QValueAxis;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , model(nullptr)
{
    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

    ui->setupUi(this);
    resize(900, 500);
    setMinimumSize(900, 500);
    setMaximumSize(900, 500);
    file_selector = new QPushButton("Select file", this);
    file_selector->move(5, 5);
    file_selector->resize(200, 40);

    connect(file_selector, &QPushButton::released, this, &MainWindow::handleClick);

    build_phases_spectr = new QPushButton("Show phase spectrum", this);
    connect(build_phases_spectr, &QPushButton::released, this, &MainWindow::handleShowPhases);
    build_phases_spectr->move(210, 5);
    build_phases_spectr->resize(200, 40);

    build_amplitudes_spectr = new QPushButton("Show amplitudes spectrum", this);
    connect(build_amplitudes_spectr, &QPushButton::released, this, &MainWindow::handleShowAmplitudes);
    build_amplitudes_spectr->move(420, 5);
    build_amplitudes_spectr->resize(200, 40);

    ch = new QChart();
    ch->resize(420, 420);
    ch->setTitle("Signal");

    chartView = new QChartView(ch, this);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->move(5, 50);

    chartView->resize(420, 420);

    bar_ch = new QChart();
    bar_ch->resize(420, 420);
    bar_ch->setTitle("Spectrum");

    barView = new QChartView(bar_ch, this);
    barView->setRenderHint(QPainter::Antialiasing);
    barView->move(450, 50);
    barView->resize(420, 420);

    discretization = new QLabel(this);
    discretization->resize(200, 40);
    discretization->move(640, 5);
    discretization->setText("Sample rate:");

}

void MainWindow::handleClick() {
    auto file_name = QFileDialog::getOpenFileName(this,
       tr("Select file with time series"), "C:\\", tr("Image Files (*.txt)"));
    auto str = file_name.toStdString();
    std::vector<std::pair<long double, long double>> time_series;
    double max_val = 0.0;
    double min_val = 0.0;
    size_t discretization;
    if (helpers::ReadTimeSeriesFromFile(str, time_series, discretization)) {
        if (model != nullptr) {
            delete model;
        }

        model = new AppModel(discretization);
        model->SetTimeSeries(time_series);

        double last_time = 0;
        if (series != nullptr) {
            delete series;
        }
        series = new QLineSeries;
        for (auto& data : time_series) {
            double time = data.first;
            last_time = time;
            double val = data.second;
            series->append(time, val);
            max_val = std::max(max_val, val);
            min_val = std::min(min_val, val);
        }

        ch->addSeries(series);

        ch->createDefaultAxes();
        ch->axes(Qt::Horizontal).first()->setRange(0, last_time);
        min_val *= 1.1;
        max_val *= 1.1;
        min_val = ceil(min_val);
        max_val = ceil(max_val);
        ch->axes(Qt::Vertical).first()->setRange(min_val, max_val);
        ch->axes(Qt::Horizontal).first()->setTitleText("Time, sec");
        ch->axes(Qt::Vertical).first()->setTitleText("Amplitude, V");
        std::string new_msg = "Sample rate: " + std::to_string(discretization);
        this->discretization->setText(QString::fromStdString(new_msg));
    } else {
        QMessageBox msgBox;
        msgBox.setText("Incorrect file format!");
        msgBox.exec();
        return;
    }

}

MainWindow::~MainWindow()
{
    delete file_selector;
    delete ui;
    delete model;
    delete series;
    delete build_phases_spectr;
    delete build_amplitudes_spectr;
    delete discretization;
    delete series;
    delete ch;
    delete chartView;
    delete bar_ch;
    delete barView;
    delete curr_series;
}

void MainWindow::handleShowPhases() {
    if (model == nullptr || model->Empty()) {
        QMessageBox msgBox;
        msgBox.setText("Firstly select file with time series!");
        msgBox.exec();
        return;
    }
    auto amplitudes = model->GetAmplitudes(0);
    auto phases = model->GetPhases(0);
    auto series = new QLineSeries();
    amplitudes.erase(amplitudes.begin());
    phases.erase(phases.begin());
    for (size_t i = 0; i < phases.size(); ++i) {
        if (amplitudes[i] < 0.01) {
            series->append(i, 0);
        } else {
            series->append(i, phases[i]);
        }
    }
    if (curr_series != nullptr) {
        bar_ch->removeSeries(curr_series);
        delete curr_series;
    }
    curr_series = series;

    bar_ch->addSeries(series);
    std::string s = "Phases spectrum";
    bar_ch->setTitle(QString::fromStdString(s));
    bar_ch->createDefaultAxes();
    bar_ch->axes(Qt::Horizontal).first()->setRange(0, amplitudes.size() / 2);
    bar_ch->axes(Qt::Horizontal).first()->setTitleText("Frequency, Hz");
    bar_ch->axes(Qt::Vertical).first()->setRange(-2 * 3.14, 2 * 3.14);
    bar_ch->axes(Qt::Vertical).first()->setTitleText("Phase, Rad");

    selected = Phases;
    HandleModeChange();
}


void MainWindow::handleShowAmplitudes() {
    if (model == nullptr || model->Empty()) {
        QMessageBox msgBox;
        msgBox.setText("Firstly select file with time series!");
        msgBox.exec();
        return;
    }
    auto amplitudes = model->GetAmplitudes(0);
    amplitudes.erase(amplitudes.begin());
    size_t i = 0;
    double max_amp = 0;
    i = 0;
    auto series = new QLineSeries();
    for (double ampl : amplitudes) {
        series->append(i, ampl);
        max_amp = std::max(max_amp, ampl);
        i+=1;
    }

    if (curr_series != nullptr) {
        bar_ch->removeSeries(curr_series);
        delete curr_series;
    }
    curr_series = series;

    bar_ch->addSeries(series);
    std::string s = "Amplitudes spectrum";
    bar_ch->setTitle(QString::fromStdString(s));
    bar_ch->createDefaultAxes();
    bar_ch->axes(Qt::Horizontal).first()->setTitleText("Frequency, Hz");
    bar_ch->axes(Qt::Horizontal).first()->setRange(0, amplitudes.size()/2);
    max_amp *= 1.1;
    max_amp = ceil(max_amp);
    bar_ch->axes(Qt::Vertical).first()->setRange(0, max_amp);
    bar_ch->axes(Qt::Vertical).first()->setTitleText("Amplitude, V");
   // ((QValueAxis*)bar_ch->axes(Qt::Vertical).first())->setTickCount(int(max_amp) + 1);

    selected = Amplitudes;
    HandleModeChange();
}

void MainWindow::HandleModeChange() {
    if (selected == Amplitudes) {
        build_amplitudes_spectr->setDisabled(true);
        build_phases_spectr->setDisabled(false);
    } else if (selected == Phases) {
        build_phases_spectr->setDisabled(true);
        build_amplitudes_spectr->setDisabled(false);
    } else {
        build_amplitudes_spectr->setDisabled(false);
        build_phases_spectr->setDisabled(false);
    }
}
