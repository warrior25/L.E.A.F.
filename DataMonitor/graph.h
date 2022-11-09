#ifndef GRAPH_H
#define GRAPH_H

#include "mainwindow.h"
#include "vector"
#include "map"

class Graph : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Graph
     */
    Graph();

    void test();

    /**
     * @brief drawChart
     * @param chartTitle
     * @param yaxisLabel
     * @param data Variable data to display in chart
     * @param window MainWindow
     * @param times Smear timepoint data
     * @param years Stat.fi timeframe in years
     */
    void drawChart(QString chartTitle, QString yaxisLabel, QVector<double> data, class MainWindow* window, QVector<QDateTime> times = {}, QVector<int> years = {});
};


#endif // GRAPH_H
