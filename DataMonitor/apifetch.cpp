#include "apifetch.h"
#include "mainwindow.h"

APIfetch::APIfetch()
{
    netManager_ = new QNetworkAccessManager(this);
    QObject::connect(netManager_, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleReply(QNetworkReply*)));
}

void APIfetch::fetchSmearData(QStringList stations, QStringList gasses, QString from, QString to, MainWindow* window, QString aggregation, int interval)
{
    currentWindow_ = window;

    std::vector<QString> variables;

    for (unsigned int i = 0; i < stations.size(); ++i) {
        for (unsigned int j = 0; j < gasses.size(); ++j) {
            if (stations.at(i) == "Värriö") {
                auto mapIt = varrioVars_.find(gasses.at(j));
                if (mapIt == varrioVars_.end()) {
                    window->showError("gas");
                    return;
                }
                variables.push_back(mapIt.value());
                varsToText_.insert(mapIt.value(), stations.at(i) + " - " + gasses.at(j));
            }
            else if (stations.at(i) == "Hyytiälä") {
                auto mapIt = hyytialaVars_.find(gasses.at(j));
                if (mapIt == hyytialaVars_.end()) {
                    window->showError("gas");
                    return;
                }
                variables.push_back(mapIt.value());
                varsToText_.insert(mapIt.value(), stations.at(i) + " - " + gasses.at(j));
            }
            else if (stations.at(i) == "Kumpula") {
                auto mapIt = kumpulaVars_.find(gasses.at(j));
                if (mapIt == kumpulaVars_.end()) {
                    window->showError("gas");
                    return;
                }
                variables.push_back(mapIt.value());
                varsToText_.insert(mapIt.value(), stations.at(i) + " - " + gasses.at(j));
            }
            else {
                window->showError("station");
                return;
            }
        }
    }

    QUrl smearUrl;
    QString smearPath = "/search/timeseries?aggregation=" + aggregation + "&interval=" + QString::number(interval) + "&from=" + from + "&to=" + to;

    for (unsigned int i = 0; i < variables.size(); ++i) {
        smearPath +=  + "&tablevariable=" + variables.at(i);
    }

    smearUrl.setUrl(smearHost_ + smearPath);

    netRequest_.setUrl(smearUrl);
    netManager_->get(netRequest_);
}

void APIfetch::fetchStatfiData(QStringList gasses, int from, int to, MainWindow *window)
{
    currentWindow_ = window;

    QString jsonString = json1;
    for (unsigned int i = 0; i < gasses.size(); ++i) {
        auto gasVar = statfiVars_.find(gasses.at(i)).value();
        jsonString += gasVar;
        varsToText_.insert(gasVar, gasses.at(i));
        if (i != gasses.size() - 1) {
            jsonString += ",";
        }
    }

    jsonString += json2;

    for (int f = from; f <= to; ++f) {
        jsonString += "\"";
        jsonString += QString::number(f);
        jsonString += "\"";
        if (f != to) {
            jsonString += ",";
        }
    }

    jsonString += json3;

    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    QByteArray data = doc.toJson();

    netRequest_.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    netRequest_.setUrl(statfiUrl_);
    netManager_->post(netRequest_, data);
}

void APIfetch::handleReply(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError) {
        qDebug() << reply->errorString();
        if (reply->error() == QNetworkReply::HostNotFoundError) {
            currentWindow_->showError("connection", netRequest_.url().toString());
        }
        else {
            currentWindow_->showError("netreply", netRequest_.url().toString());
        }
        return;
    }

    QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
    QJsonObject rootObject = document.object();

    if (reply->url().toString().contains(statfiUrl_.toString())) {
        QStringList gasses = rootObject.value("dimension").toObject().value("Tiedot").toObject().value("category").toObject().value("index").toObject().keys();
        QStringList years = rootObject.value("dimension").toObject().value("Vuosi").toObject().value("category").toObject().value("index").toObject().keys();
        auto data = rootObject.value("value").toArray();

        if (data.isEmpty()) {
            qDebug() << "Empty data";
            currentWindow_->showError("netreply_empty", netRequest_.url().toString());
            return;
        }

        statfiDataHandler(gasses, years, data);
    }
    else if (reply->url().toString().contains(smearHost_)) {
        QJsonArray columns = rootObject.value("columns").toArray();

        QJsonArray data = rootObject.value("data").toArray();

        if (data.isEmpty()) {
            qDebug() << "Empty data";
            currentWindow_->showError("netreply_empty", netRequest_.url().toString());
            return;
        }

        smearDataHandler(columns, data);
    }
    else {
        qDebug() << "!!! Reply from unknown URL !!!";
        currentWindow_->showError("url", netRequest_.url().toString());
    }
}

void APIfetch::smearDataHandler(QJsonArray columns, QJsonArray data)
{
    QMap<QString, QStringList> formattedData;

    QStringList times;
    for (unsigned int k = 0; k < data.size(); ++k) {
        times.push_back(data.at(k).toObject().value("samptime").toString());
    }

    formattedData.insert("Time", times);

    for (unsigned int i = 0; i < columns.size(); ++i) {
        QStringList temp;

        for (unsigned int j = 0; j < data.size(); ++j) {
            temp.push_back(QString::number(data.at(j).toObject().value(columns.at(i).toString()).toDouble()));
        }
        QString columnName = varsToText_.find(columns.at(i).toString()).value();
        formattedData.insert(columnName, temp);
    }

    currentWindow_->receiveData(formattedData);
}

void APIfetch::statfiDataHandler(QStringList gasses, QStringList years, QJsonArray data)
{
    QMap<QString, QStringList> formattedData;

    formattedData.insert("Time", years);

    QStringList temp;
    for (int j = 0; j < data.size(); ++j) {
       temp.push_back(QString::number(data.at(j).toDouble()));
       if ((j + 1) % years.size() == 0) {
           QString gasName = varsToText_.find("\"" + gasses.at((j + 1)/years.size() - 1) + "\"").value();
           formattedData.insert(gasName, temp);
           temp.clear();
       }
    }

    currentWindow_->receiveData(formattedData);
}
