#include "graph.h"
#include <QChart>
#include <QLineSeries>
#include <QChartView>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QPushButton>
#include <QStackedLayout>
#include "mainwindow.h"


Graph::Graph()
{

}

void Graph::drawChart(QString chartTitle, QString yaxisLabel, QVector<double> data, MainWindow* window, QVector<QDateTime> times, QVector<int> years)
{
    QLineSeries* series = new QLineSeries();
    QChart *chart = new QChart();

    QDateTimeAxis* axisX = new QDateTimeAxis();
    QValueAxis* axisX2 = new QValueAxis();

    if (!years.isEmpty()) {
        chart->addAxis(axisX2, Qt::AlignBottom);
        axisX2->setRange(years.at(0), years.at(years.size()-1));
        axisX2->setLabelFormat("%i");
        axisX2->setTickAnchor(years.at(0));
        axisX2->setTickInterval(1);
        axisX2->setTickType(QValueAxis::TicksDynamic);
        axisX->deleteLater();
    }
    else {
        axisX->setFormat("dd.MM.yyyy");
        chart->addAxis(axisX, Qt::AlignBottom);
        axisX->setRange(times.at(0), times.at(times.size()-1));
        axisX->setTickCount(6);
        axisX2->deleteLater();
    }

    QValueAxis* axisY = new QValueAxis();
    axisY->setLabelFormat("%.1f");
    axisY->setTitleText(yaxisLabel);
    chart->addAxis(axisY, Qt::AlignLeft);

    for (long long i = 0; i < data.size(); ++i) {
        if (data.at(i) != 0) {
            if (years.isEmpty()) {
                series->append(times.at(i).toMSecsSinceEpoch(), data.at(i));
            }
            else {
                series->append(years.at(i), data.at(i));
            }
        }
    }

    chart->legend()->hide();
    chart->setTitle(chartTitle);
    chart->addSeries(series);

    if (years.isEmpty()) {
        series->attachAxis(axisX);
    }
    else {
        series->attachAxis(axisX2);
    }
    series->attachAxis(axisY);

    QPen pen(QRgb(0x4e008e));
    series->setPen(pen);


    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    window->addChart(chartView);

}
