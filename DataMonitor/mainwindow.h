#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QAbstractItemView>
#include <QDate>
#include <QChartView>
#include <QGridLayout>
#include <QResizeEvent>
#include <QLabel>
#include <QMovie>
#include <QMenu>
#include <QStackedLayout>
#include <QFileDialog>
#include <QLineEdit>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief MainWindow constructs mainwindow
     * @param parent
     */
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    /**
     * @brief receiveData receives data and adds it to selections
     * @param data
     */
    void receiveData(QMap<QString, QStringList> data);

    /**
     * @brief comboBoxSelections sets right settings on comboboxes
     * @param name Which combobox
     * @param type Tells what type of settings to be set
     */
    void comboBoxSelections(QString name, QString type);

    /**
     * @brief showChart presents charts in right form
     */
    void showChart();

    /**
     * @brief addChart adds new chart
     * @param chart Chart to be added
     */
    void addChart(QChartView* chart);

    /**
     * @brief addTabBarLine adds new line on tab
     * @param type What type of line
     * @param data What data type
     * @param station Which station
     * @param startDate Timeframe begin
     * @param endDate Timeframe end
     * @param bookmarkName Name of bookmark
     */
    void addTabBarLine(QString type, QString data = "", QString station = "", QString startDate = "", QString endDate = "", QString bookmarkName = "");

    /**
     * @brief requestCustomContextMenu creates menu to open when user clicks three dots in charts
     * @param chart What chart is under inspection
     * @param pos Where menu is positioned
     * @param size Menu's sizes
     */
    void requestCustomContextMenu(QChartView* chart, QPoint pos, QSize size);

    /**
     * @brief addChartButton creates layout with buttons and chart
     * @param chart
     */
    void addChartButton(QChartView* chart);

    /**
     * @brief showError shows error message in case of error
     * @param msg Error message
     * @param extraData Additions to message
     */
    void showError(QString msg, QString extraData = "Unknown error");

    /**
     * @brief showBookmarkWidget shows bookmark saving pop-up-window
     * @param value If True window is showed if False it is hidden
     */
    void showBookmarkWidget(bool value);

    /**
     * @brief showBookmarkError shows error if saving bookmark isnt succesful
     * @param type Error message
     */
    void showBookmarkError(QString type = "");

    /**
     * @brief clearButtonEnabled set clear button enabled
     * @param value If True set enable if False set disabled
     */
    void clearButtonEnabled(bool value);

    /**
     * @brief deleteAllBookmarks deletes all bookmarks
     */
    void deleteAllBookmarks();

    /**
     * @brief clearCharts removes charts
     */
    void clearCharts();

    /**
     * @brief clearHistory
     */
    void clearHistory();

    /**
     * @brief getSaveFileLocation dispalys file path select dialog
     * @param title placeholder filename
     * @return file path
     */
    QString getSaveFileLocation(QString title);

    /**
     * @brief setButtonState Changes button style on/off
     * @param button which button style to modify
     * @param value if true then button enabled, false to disable
     */
    void setButtonState(QWidget* button, bool value);

    /**
     * @brief hideChartButton hides buttons on charts
     * @param value If True buttons are hidden or if False buttons are shown
     */
    void hideChartButton(bool value);

private slots:
    /**
     * @brief addChartButtonClicked connects addChartButton with chart adding functions
     */
    void addChartButtonClicked();

    /**
     * @brief on_settingsButton_clicked opens setting tabs
     */
    void settingsButtonToggled(bool value);

    /**
     * @brief dateBeginEditingFinished uses begin date to set maximum date
     */
    void dateBeginEditingFinished();

    /**
     * @brief leftArrowClicked changes page to one on left
     */
    void leftArrowClicked();

    /**
     * @brief rightArrowClicked changes page to one on right
     */
    void rightArrowClicked();

    /**
     * @brief tabChanged changes the tab
     * @param index Tells what tab is
     */
    void tabChanged(int index);

    /**
     * @brief deleteChart removes one chart
     * @param widget Chart to be removed
     */
    void deleteChart(QWidget* widget);

    /**
     * @brief saveButtonClicked saves bookmark
     */
    void saveButtonClicked();

    /**
     * @brief timeout creates timeout error
     * @param manual
     */
    void timeout(bool manual = false);

    /**
     * @brief errorTimeout gives timeout error
     */
    void errorTimeout();

    /**
     * @brief openChartInNewWindow shows chart in new window
     * @param chart What chart is shown
     */
    void openChartInNewWindow(QChartView* chart);

private:
    Ui::MainWindow *ui;

    /**
     * @brief deleteHistoryLines deletes lines on history tab
     */
    void deleteHistoryLines();

    /**
     * @brief resizeEvent when window size is changed
     * @param event
     */
    void resizeEvent(QResizeEvent* event);

    /**
     * @brief Closes opened chart
     * @param chart what was opened
     */
    void hideChartPopup(QChartView* chart);

    // Window initial size
    int windowHeight_ = 800;
    int windowWidth_ = 1200;

    // Text that is shown if no charts are present
    QLabel* placeHolder_;

    QStringList data_types_
    {"Smear", "CO2", "SO2", "NOX", "Stat.fi", "CO2 indexed", "CO2 intensity", "CO2 intensity indexed", "CO2 tonnes"};

    QStringList data_places_ {"Hyytiälä", "Kumpula", "Värriö"};

    // Dates
    QDate today_ = QDate().currentDate();
    QDate yearAgo_ = today_.addYears(-1);

    // Current data that is displayed in the window
    QVector<QWidget*> charts_;
    QVector<QWidget*> currentLines_;
    QVector<QWidget*> history_;

    // Instance of the controller class
    class controller* ctrl_;

    // Loading data animation
    QMovie* movie_;

    // Pages to display charts in
    QVector<QGridLayout*> pages_;

    // Our software logo
    QLabel* icon_;

    // Smear of statfi
    QString apitype_;

    // Aborts loading from api if taking too long
    QTimer* timer_;

    // Timer for showing error message
    QTimer* errorTimer_;

    // To get chart back to page from popup
    QLayout* oldParent_;

    QMap<QString, QString> errors_ = {
        {"gas", "Internal error: Incorrect gas name"},
        {"station", "Internal error: Incorrect station"},
        {"netreply", "Bad reply from API"},
        {"netreply_empty", "Empty response from API"},
        {"url", "Reply from unknown URL"},
        {"timeout", "Connection to API timed out"},
        {"connection", "Please check your internet connection"}
    };
};
#endif // MAINWINDOW_H
