#include "controller.h"

controller::controller(MainWindow* window)
{
    APIfetcher_ = new APIfetch;
    graph_ = new Graph;
    fh_ = new FileHandler;
    window_ = window;
}

void controller::fetchData(MainWindow *window, QDate from, QDate to, QStringList gasses, QStringList stations)
{
    if (stations.at(0) != "Finland") {
        QString formattedFrom = formatISO8601(from);
        QString formattedTo = formatISO8601(to);
        APIfetcher_->fetchSmearData(stations, gasses, formattedFrom, formattedTo, window);
    }
    else {
        int formattedFrom = from.year();
        int formattedTo = to.year();
        APIfetcher_->fetchStatfiData(gasses, formattedFrom, formattedTo, window);
    }

    saveHistory(from, to, gasses, stations, window);
}

void controller::createChart(QMap<QString, QStringList> data, MainWindow* window)
{
    auto timelist = data.find("Time").value();
    QList<QDateTime> times;
    QVector<int> years;

    if (timelist.at(0).size() == 4) {
        for (unsigned int j = 0; j < timelist.size(); ++j) {
            years.push_back(timelist.at(j).toInt());
        }
    }
    else {
        times = formatForChart(data.find("Time").value());
    }

    auto keys = data.keys();

    for (unsigned int i = 0; i < keys.size(); ++i) {
        if (keys.at(i) != "Time") {

            QVector<double> vect;
            foreach(QString str, data.find(keys.at(i)).value()) {
                vect.push_back(str.toDouble());
            }

            if (timelist.at(0).size() == 4) {
                graph_->drawChart(keys.at(i), units[keys.at(i)], vect, window, {}, years);
            }
            else {
                graph_->drawChart(keys.at(i), units[keys.at(i).split(" - ").at(1)], vect, window, times);
            }
        }
    }
}

QString controller::formatISO8601(QDate time)
{
    return time.toString(Qt::ISODate) + "T00:00:00.000";
}

QVector<QDateTime> controller::formatForChart(QStringList times)
{
    QVector<QDateTime> timevector;

    for (int i = 0; i < times.size(); ++i) {
        auto time = QDateTime::fromString(times.at(i), Qt::ISODate);
        timevector.push_back(time);
    }

    return timevector;
}

void controller::loadData(QString type, MainWindow* window)
{
    if (type == "history") {
        auto historyData = fh_->load(historyFile_);
        for (auto dataPoint: historyData) {
            auto splitted = dataPoint.split(",");
            QString data = splitted.at(2);
            QString stations = splitted.at(3);
            QString from = splitted.at(0);
            QString to = splitted.at(1);
            window->addTabBarLine("history", data, stations, from, to);
        }
    }
    else if (type == "bookmarks") {
        auto bookmarkData = fh_->load(bookmarksFile_);
        for (auto dataPoint: bookmarkData) {
            auto splitted = dataPoint.split(";");
            window->addTabBarLine("bookmarks", "", "", "", "", splitted.at(0));
            currentBookmarks_.push_back(splitted.at(0));
        }
    }

}

void controller::clearHistory()
{
    fh_->clearFile(historyFile_);
    window_->clearHistory();
}

void controller::clearBookmarks(QString name)
{
    if (name == "") {
        fh_->clearFile(bookmarksFile_);
        currentBookmarks_.clear();
        return;
    }

    QStringList lines = fh_->load(bookmarksFile_);
    for (auto line: lines) {
        if (line.split(";").at(0) == name) {
            fh_->removeLine(line, bookmarksFile_);
        }
    }

    currentBookmarks_.removeAt(currentBookmarks_.indexOf(name));
}

void controller::saveBookmark(QString name)
{
    window_->showBookmarkError("");
    if (name == "") {
        window_->showBookmarkError("Name cannot be empty");
        return;
    }

    if (!checkBookmarkName(name)){
        QString data = name;
        for (QString point: currentSelections_) {
            data.push_back(";" + point);
        }
        data.push_back("\n");

        fh_->save(bookmarksFile_, data);

        window_->addTabBarLine("bookmarks", "", "", "", "", name);

        currentBookmarks_.push_back(name);
        window_->showBookmarkWidget(false);
        window_->clearButtonEnabled(true);
    } else {
        window_->showBookmarkError("Name is already in use");
    }

}

void controller::loadBookmark(QString name)
{
    window_->clearCharts();

    auto bookmarkData = fh_->load(bookmarksFile_);
    for (auto dataPoint: bookmarkData) {
        auto splitted = dataPoint.split(";");
        if (splitted.at(0) == name) {
            for (auto i = 1;i < splitted.size();i++) {
                auto datasplit = splitted.at(i).split(",");
                if (datasplit.at(1) == "Finland") {
                    fetchData(window_, QDate::fromString(datasplit.at(2), "yyyy"),
                              QDate::fromString(datasplit.at(3), "yyyy"),
                              QStringList({datasplit.at(0)}), QStringList({datasplit.at(1)}));
                } else {
                    fetchData(window_, QDate::fromString(datasplit.at(2), "dd.MM.yyyy"),
                              QDate::fromString(datasplit.at(3), "dd.MM.yyyy"),
                              QStringList({datasplit.at(0)}), QStringList({datasplit.at(1)}));
                }
            }
        }
    }
}

void controller::clearCurrentSelections(int i)
{
    if (i == -1) {
        currentSelections_.clear();
    }
    else {
        currentSelections_.removeAt(i);
    }
}

void controller::addCurrentSelection(QString data, QString station, QString startDate, QString endDate)
{
    currentSelections_.append(data+","+station+","+startDate+","+endDate);
}

void controller::writeErrorLog(QString msg)
{
    QString error = "[" + QDateTime().currentDateTime().toString() + "] " + msg + "\n";
    fh_->save(errorLog_, error);
}

void controller::downloadChart(QWidget *chart, QString title)
{
    QString fileLocation = window_->getSaveFileLocation(title + ".png");
    window_->hideChartButton(true);
    fh_->saveImage(chart, fileLocation);
    window_->hideChartButton(false);
}

void controller::clearButtonClicked()
{
    window_->clearCharts();
    clearCurrentSelections();
}

void controller::bookmarkDeleteButtonClicked(QWidget *widget)
{
    clearBookmarks(widget->findChild<QPushButton*>()->text());
    delete widget;

    if (currentBookmarks_.size() == 0){
        window_->clearButtonEnabled(false);
    }
}

void controller::bookmarkClearButtonClicked()
{
    window_->deleteAllBookmarks();
    currentBookmarks_.clear();
    fh_->clearFile(bookmarksFile_);

}


void controller::saveHistory(QDate from, QDate to, QStringList gasses, QStringList stations, MainWindow* window)
{
    QString data;
    QString gasData;
    QString stationData;

    data.append(from.toString() + ","+ to.toString() + ",");
    for (auto gas: gasses) {
        if (gas == gasses.at(gasses.size() - 1)) {
            data.append(gas);
            gasData.append(gas);
            break;
        }
        data.append(gas + "/");
        gasData.append(gas + "/");
    }
    data.append(",");
    for (auto station: stations) {
        if (station == stations.at(stations.size() - 1)) {
            data.append(station);
            stationData.append(station);
            break;
        }
        data.append(station + "/");
        stationData.append(station + "/");
    }
    //currentSelections_.append(data);
    data.append("\n");
    fh_->save(historyFile_, data);

    window->addTabBarLine("history", gasData, stationData, from.toString(), to.toString());
}

bool controller::checkBookmarkName(QString name)
{
    return currentBookmarks_.contains(name);
}
