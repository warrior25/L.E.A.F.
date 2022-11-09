#ifndef APIFETCH_H
#define APIFETCH_H

#include <QNetworkReply>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "mainwindow.h"


class APIfetch: public QObject
{
    Q_OBJECT

public:
    /**
     * @brief APIfetch
     */
    APIfetch();

    /**
     * @brief fetchSmearData Fetches data from Smear API with selected parameters
     * @param stations Measuring stations
     * @param gasses Measured gasses
     * @param from Timeframe start
     * @param to Timeframe end
     * @param window MainWindow
     * @param aggregation Aggregation type
     * @param interval Aggregation interval
     */
    void fetchSmearData(QStringList stations, QStringList gasses, QString from, QString to, MainWindow* window, QString aggregation = "MEDIAN", int interval = 60);

    /**
     * @brief fetchStatfiData Fetches data from Stat.fi API with selected parameters
     * @param gasses Measured gasses
     * @param from Timeframe start
     * @param to Timeframe end
     * @param window MainWindow
     */
    void fetchStatfiData(QStringList gasses, int from, int to, MainWindow* window);

private slots:
    /**
     * @brief handleReply Processes the network response from either API
     * @param reply Network reply
     */
    void handleReply(QNetworkReply* reply);

private:
    /**
     * @brief smearDataHandler Formats the data from Smear API and sends it to MainWindow
     * @param columns Data column names
     * @param data
     */
    void smearDataHandler(QJsonArray columns, QJsonArray data);

    /**
     * @brief statfiDataHandler Formats the data from Stat.fi API and sends it to MainWindow
     * @param gasses Gas variable names
     * @param years Every year from selected timeframe
     * @param data
     */
    void statfiDataHandler(QStringList gasses, QStringList years, QJsonArray data);

    // Reference to send data back to MainWindow
    MainWindow* currentWindow_;

    QNetworkAccessManager* netManager_;
    QNetworkReply* netReply_;
    QNetworkRequest netRequest_;

    // Human readable gas names converted to variables for API calls
    QMap<QString, QString> varrioVars_ = { {"CO2", "VAR_EDDY.av_c"}, {"SO2", "VAR_META.SO2_1"}, {"NOX", "VAR_META.NO_1"} };
    QMap<QString, QString> hyytialaVars_ = { {"CO2", "HYY_META.CO2icos168"}, {"SO2", "HYY_META.SO2168"}, {"NOX", "HYY_META.NO168"} };
    QMap<QString, QString> kumpulaVars_ = { {"CO2", "KUM_EDDY.av_c_ep"}, {"SO2","KUM_META.SO_2"}, {"NOX", "KUM_META.NO"} };
    QMap<QString, QString> statfiVars_ = { {"CO2 tonnes", "\"Khk_yht\""}, {"CO2 indexed", "\"Khk_yht_index\""}, {"CO2 intensity indexed", "\"Khk_yht_las_index\""}, {"CO2 intensity", "\"Khk_yht_las\""}};

    // Maps variable names back to human readable form
    QMap<QString, QString> varsToText_ = { };

    QString smearHost_ = "https://smear-backend.rahtiapp.fi";
    QUrl statfiUrl_ = QUrl("https://pxnet2.stat.fi:443/PXWeb/api/v1/en/ymp/taulukot/Kokodata.px");

    // Parts of json POST request for Stat.fi API calls
    QString json1 = "{\"query\": [{\"code\": \"Tiedot\",\"selection\": {\"filter\": \"item\",\"values\": [";
    QString json2 = "]}},{\"code\": \"Vuosi\",\"selection\": {\"filter\": \"item\",\"values\": [";
    QString json3 = "]}}],\"response\": {\"format\": \"json-stat2\"}}";

};

#endif // APIFETCH_H
