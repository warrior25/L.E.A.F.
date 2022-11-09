#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QDate>
#include <QStringList>
#include <QMap>
#include <apifetch.h>
#include <QPushButton>

#include "mainwindow.h"
#include "graph.h"
#include "filehandler.h"

class controller: public QObject
{
    Q_OBJECT

public:
    /**
     * @brief controller
     */
    controller(MainWindow* window);

    /**
     * @brief fetchData Interface for making API calls
     * @param window MainWindow
     * @param from Timeframe start
     * @param to Timeframe end
     * @param gasses Requested gas data
     * @param stations Requested measuring stations
     */
    void fetchData(MainWindow* window, QDate from, QDate to, QStringList gasses, QStringList stations = {});

    /**
     * @brief createChart Interface for creating charts
     * @param data Chart data
     * @param window MainWindow
     */
    void createChart(QMap<QString, QStringList > data, MainWindow* window);

    /**
     * @brief formatISO8601 Formats date to ISO8601 with milliseconds
     * @param time
     * @return QDate with format yyyy-MM-ddT00:00:00.000
     */
    QString formatISO8601(QDate time);

    /**
     * @brief formatForChart Formats timepoint data for use with charts
     * @param times
     * @return Vector of QDateTimes with Qt::ISODate format
     */
    QVector<QDateTime> formatForChart(QStringList times);

    /**
     * @brief loadData uses param type to determine what data to load and loads it from file
     * @param type is either "history" or "bookmarks" and it is used to determine what file to load
     * @param window mainwindow
     */
    void loadData(QString type, MainWindow *window);

    /**
     * @brief clearHistory clears file
     */
    void clearHistory();

    /**
     * @brief clearBookmarks clears bookmarks from file
     * @param name Determindes what bookmarks are cleared, if empty string all bookmarks are cleared
     */
    void clearBookmarks(QString name = "");

    /**
     * @brief saveBookmark saves bookmarks on file and adds them on bookmarks tab
     * @param name Gives bookmark a name
     */
    void saveBookmark(QString name);

    /**
     * @brief loadBookmark loads bookmark from file
     * @param name Bookmark to be loaded
     */
    void loadBookmark(QString name);

    /**
     * @brief clearCurrentSelections clears currentSelections_
     * @param i If it is -1 everything is cleared otherwise it is used as index
     */
    void clearCurrentSelections(int i = -1);

    /**
     * @brief addCurrentSelection adds data to currentSelections_
     * @param data Datatype
     * @param station Which station is used
     * @param startDate Timeframe start
     * @param endDate Timeframe end
     */
    void addCurrentSelection(QString data, QString station, QString startDate, QString endDate);

    /**
     * @brief writeErrorLog adds messages to errorlog
     * @param msg Errormessage
     */
    void writeErrorLog(QString msg);

    /**
     * @brief downloadChart saves chart as image
     * @param chart Chart to be saved as image
     * @param title Name for image
     */
    void downloadChart(QWidget* chart, QString title);

public slots:

    /**
     * @brief clearButtonClicked clears charts and current selections
     */
    void clearButtonClicked();

    /**
     * @brief bookmarkDeleteButtonClicked connects delete button and clearBookmarks()
     * @param widget What bookmark to delete
     */
    void bookmarkDeleteButtonClicked(QWidget* widget);

    /**
     * @brief bookmarkClearButtonClicked connects clear button and deleteAllBookmarks()
     */
    void bookmarkClearButtonClicked();



private:
    /**
     * @brief saveHistory adds data to history tab and file
     * @param from Timeframe begin
     * @param to Timeframe end
     * @param gasses What data types are selected
     * @param stations What stations are selected
     * @param window Mainwindow
     */
    void saveHistory(QDate from, QDate to, QStringList gasses, QStringList stations, MainWindow *window);

    /**
     * @brief checkBookmarkName checks what bookmark names are used
     * @param name
     * @return True if bookmark name exists and false if not
     */
    bool checkBookmarkName(QString name);

    // Class objects
    APIfetch* APIfetcher_;
    Graph* graph_;
    FileHandler* fh_;
    QString historyFile_ = "history.txt";
    QString bookmarksFile_ = "bookmarks.txt";
    QString errorLog_ = "errors.txt";

    MainWindow* window_;

    QStringList currentSelections_;
    QStringList currentBookmarks_;

    QMap<QString, QString> units = { {"CO2", "ppm"}, {"SO2", "ppb"}, {"NOX","ppb"},
                                     {"CO2 tonnes","tonnes"}, {"CO2 indexed","index, year 1990 = 100"},
                                     {"CO2 intensity indexed","index, year 1990 = 100"}, {"CO2 intensity","intensity"} };

};

#endif // CONTROLLER_H
