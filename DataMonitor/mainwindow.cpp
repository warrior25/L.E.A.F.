#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "controller.h"

QString disabledStyle = "background-color: rgb(226,193,255); border-radius: 10px";
QString enabledStyle = "background-color: rgb(255,255,255); color: rgb(0,0,0); border-radius: 10px";
QString buttonEnabledStyle = "background-color: rgb(255,255,255); border-radius: 5px; padding: 5px";
QString buttonDisabledStyle = "background-color: rgb(226,193,255); border-radius: 5px; padding: 5px";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    windowWidth_ = this->width();
    windowHeight_ = this->height();

    // Set settings
    ui->tabWidget->setTabText(0, "Current selections");
    ui->tabWidget->setTabText(1, "History");
    ui->tabWidget->setTabText(2, "Bookmarks");
    ui->saveButton->setIcon(QIcon("../icons/floppy-disk.svg"));
    ui->clearButton->setIcon(QIcon("../icons/trash-can.svg"));
    ui->tabWidget->setVisible(false);

    ui->tabWidget->setCurrentIndex(0);

    setButtonState(ui->clearButton, false);
    setButtonState(ui->clearButton2, false);
    ui->clearButton2->setIcon(QIcon("../icons/trash-can.svg"));
    ui->clearButton3->setIcon(QIcon("../icons/trash-can.svg"));
    setButtonState(ui->clearButton3, false);
    setButtonState(ui->saveButton, false);

    ui->settingsButton->setIcon(QIcon("../icons/ellipsis-vertical.svg"));
    setButtonState(ui->settingsButton, true);

    ui->dataComboBox->setDataComboBox(true);
    ui->dataComboBox->addItems(data_types_);
    ui->dataComboBox->getMainWindow(this);
    ui->dataComboBox->setPlaceHolderText("Select data...");
    ui->stationComboBox->addItems(data_places_);
    ui->stationComboBox->setPlaceHolderText("Select stations...");
    ui->stationComboBox->setDisabled(true);
    ui->stationComboBox->getMainWindow(this);
    ui->stationComboBox->setState(false);
    ui->stationComboBox->setStyleSheet(disabledStyle);

    ui->dateBegin->setDate(yearAgo_);
    ui->dateEnd->setDate(today_);
    ui->dateBegin->setEnabled(false);
    ui->dateEnd->setEnabled(false);
    ui->dateBegin->setStyleSheet(disabledStyle);
    ui->dateEnd->setStyleSheet(disabledStyle);

    ui->pageSelector->hide();

    setButtonState(ui->addButton, false);
    ui->addButton->setIcon(QIcon("../icons/plus-solid.svg"));

    placeHolder_ = new QLabel;
    placeHolder_->setGeometry(0, 75, windowWidth_, (windowHeight_-75));
    placeHolder_->setParent(this);
    placeHolder_->setPixmap(QPixmap("../icons/leaf-solid.png"));
    placeHolder_->show();
    placeHolder_->setAlignment(Qt::AlignCenter);

    movie_ = new QMovie("../icons/loading.gif");

    ctrl_ = new controller(this);

    ui->tabWidget->tabBar()->setTabTextColor(0, QColor(237, 175, 200));

    ui->bookmarksDialogWidget->setParent(this);
    ui->bookmarksDialogWidget->setGeometry(windowWidth_/2, windowHeight_/2, windowWidth_/4, (windowHeight_-75)/4);
    ui->bookmarksDialogWidget->hide();

    ui->errorLabel->hide();

    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::tabChanged);
    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::addChartButtonClicked);
    connect(ui->settingsButton, &QPushButton::toggled, this, &MainWindow::settingsButtonToggled);
    connect(ui->clearButton, &QPushButton::clicked, ctrl_, &controller::clearButtonClicked);
    connect(ui->dateBegin, &QDateEdit::dateChanged, this, &MainWindow::dateBeginEditingFinished);
    connect(ui->leftArrow, &QPushButton::clicked, this, &MainWindow::leftArrowClicked);
    connect(ui->rightArrow, &QPushButton::clicked, this, &MainWindow::rightArrowClicked);
    connect(ui->clearButton2, &QPushButton::clicked, ctrl_, &controller::clearHistory);
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::saveButtonClicked);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, ctrl_, [=]() { ctrl_->saveBookmark(ui->lineEdit->text()); });
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, [=]() { showBookmarkWidget(false); });
    connect(ui->clearButton3, &QPushButton::clicked, ctrl_, &controller::bookmarkClearButtonClicked);

    ui->stackedWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ctrl_->loadData("history", this);
    ctrl_->loadData("bookmarks", this);
    ui->scrollArea->setStyleSheet(QString::fromUtf8("QScrollBar:vertical {"
                                                                  "    border: 1px solid #999999;"
                                                                  "    background:white;"
                                                                  "    width:10px;    "
                                                                  "    margin: 0px 0px 0px 0px;"
                                                                  "}"
                                                                  "QScrollBar::handle:vertical {"
                                                                  "    background: black;"
                                                                  "    min-height: 0px;"
                                                                  "}"
                                                                  "QScrollBar::add-line:vertical {"
                                                                  "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                                                                  "    stop: 0 rgb(32, 47, 130), stop: 0.5 rgb(32, 47, 130),  stop:1 rgb(32, 47, 130));"
                                                                  "    height: 0px;"
                                                                  "    subcontrol-position: bottom;"
                                                                  "    subcontrol-origin: margin;"
                                                                  "}"
                                                                  "QScrollBar::sub-line:vertical {"
                                                                  "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                                                                  "    stop: 0  rgb(32, 47, 130), stop: 0.5 rgb(32, 47, 130),  stop:1 rgb(32, 47, 130));"
                                                                  "    height: 0 px;"
                                                                  "    subcontrol-position: top;"
                                                                  "    subcontrol-origin: margin;"
                                                                  "}"
                                                                  ));
    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, [=]() { timeout(false); });

    errorTimer_ = new QTimer(this);
    connect(errorTimer_, &QTimer::timeout, this, &MainWindow::errorTimeout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::receiveData(QMap<QString, QStringList> data)
{
    timer_->stop();
    ctrl_->createChart(data, this);
    ui->dataComboBox->setState(true);
    ui->dataComboBox->setStyleSheet(enabledStyle);
    setButtonState(ui->leftArrow, false);
    ui->pageSelector->show();
    setButtonState(ui->settingsButton, true);

    QString dataType = "";
    QString station = "Finland";
    QString start = "";
    QString end = "";


    auto keys = data.keys();
    for (auto key:keys){
        if (key != "Time"){
            auto parts = key.split(" - ");
            if (parts.size() == 1 ){ //Statfi
                dataType = parts.at(0);
                station = "Finland";
                start = data.find("Time").value().at(0);
                end = data.find("Time").value().at(data.find("Time").value().size()-1);


            } else {
                station = parts.at(0);
                dataType = parts.at(1);


                start = data.find("Time").value().at(0);
                auto part = start.split("T");
                auto p = part.at(0).split("-");
                start = p.at(2) + "." + p.at(1) + "." + p.at(0);

                end = data.find("Time").value().at(data.find("Time").value().size()-1);
                part = end.split("T");
                p = part.at(0).split("-");
                end = p.at(2) + "." + p.at(1) + "." + p.at(0);
            }


            addTabBarLine("selection", dataType, station, start, end);
        }
    }



}

void MainWindow::comboBoxSelections(QString name, QString type)
{
    apitype_ = type;
    if (name == "dataComboBox") {
        if (type == "statfi") {
            ui->stationComboBox->setState(false);
            ui->stationComboBox->setStyleSheet(disabledStyle);

            ui->dateBegin->setStyleSheet(enabledStyle);
            ui->dateEnd->setStyleSheet(enabledStyle);
            ui->dateBegin->setEnabled(true);
            ui->dateEnd->setEnabled(true);

            ui->dateBegin->setDisplayFormat("yyyy");
            ui->dateBegin->setCalendarPopup(false);
            ui->dateEnd->setDisplayFormat("yyyy");
            ui->dateEnd->setCalendarPopup(false);

            ui->dateBegin->setDateRange(QDate::fromString("1990-01-01", "yyyy-MM-dd"), QDate::fromString("2016-01-01", "yyyy-MM-dd"));
            ui->dateEnd->setDateRange(QDate::fromString("1990-01-01", "yyyy-MM-dd"), QDate::fromString("2016-01-01", "yyyy-MM-dd"));

            ui->dateBegin->setDate(QDate::fromString("2005-01-01", "yyyy-MM-dd"));
            ui->dateEnd->setDate(QDate::fromString("2010-01-01", "yyyy-MM-dd"));

            ui->stationComboBox->setPlaceHolderText("Finland");

            setButtonState(ui->addButton, true);
        }
        else if (type == "smear") {
            ui->stationComboBox->setState(true);
            ui->stationComboBox->setStyleSheet(enabledStyle);

            ui->dateBegin->setDisplayFormat("dd.MM.yyyy");
            ui->dateBegin->setCalendarPopup(true);
            ui->dateEnd->setDisplayFormat("dd.MM.yyyy");
            ui->dateEnd->setCalendarPopup(true);

            ui->dateBegin->setDateRange(QDate::fromString("1990-01-01", "yyyy-MM-dd"), today_);
            ui->dateEnd->setDateRange(QDate::fromString("1990-01-01", "yyyy-MM-dd"), today_);

            ui->dateBegin->setDate(yearAgo_);
            ui->dateEnd->setDate(today_);
        }
        else {
            ui->stationComboBox->setState(false);
            ui->stationComboBox->setStyleSheet(disabledStyle);
            ui->dateBegin->setStyleSheet(disabledStyle);
            ui->dateEnd->setStyleSheet(disabledStyle);
            ui->dateBegin->setEnabled(false);
            ui->dateEnd->setEnabled(false);
            ui->stationComboBox->setPlaceHolderText("Select stations...");

            setButtonState(ui->addButton, false);
        }
    }
    else if (name == "stationComboBox") {
        if (type == "smear") {
            ui->dateBegin->setStyleSheet(enabledStyle);
            ui->dateEnd->setStyleSheet(enabledStyle);
            ui->dateBegin->setEnabled(true);
            ui->dateEnd->setEnabled(true);

            setButtonState(ui->addButton, true);
        }
        else {
            ui->dateBegin->setStyleSheet(disabledStyle);
            ui->dateEnd->setStyleSheet(disabledStyle);
            ui->dateBegin->setEnabled(false);
            ui->dateEnd->setEnabled(false);

            setButtonState(ui->addButton, false);
        }
    }
}

void MainWindow::showChart()
{
    int num = charts_.size();
    int row = 0;
    int column = 0;
    int pageCount = ceil((float)num/4.0);
    int pageCounter = 0;

    int currPageCount = ui->stackedWidget->count();

    for (int a = 0; a < currPageCount; ++a) {
        QWidget* widget = ui->stackedWidget->widget(0);
        ui->stackedWidget->removeWidget(widget);
        widget->deleteLater(); // delete the widget
    }
    pages_.clear();

    for (int k = 0; k < pageCount; ++k) {
        QWidget* page = new QWidget;
        QGridLayout* grid = new QGridLayout;
        page->setLayout(grid);
        page->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        pages_.push_back(grid);
        ui->stackedWidget->addWidget(page);
    }

    for (int i = 0; i < num; i++){
        if (row < 2) {
            pages_.at(pageCounter)->addWidget(charts_.at(i), row, column);
        }
        else {
            row = 0;
            column = 0;
            pageCounter++;
            pages_.at(pageCounter)->addWidget(charts_.at(i), row, column);
        }
        if (column == 1 && i != num-1){
            column = 0;
            row++;
        } else {
            column++;
        }
    }

    row++;
    for (int page = 0; page < pages_.size(); ++page) {
        for (int j = 0; j < 2; j++){
            pages_.at(page)->setRowMinimumHeight(j, (windowHeight_-100) / 2);
            pages_.at(page)->setColumnMinimumWidth(j, (windowWidth_-50) /2);
        }
    }

    ui->currPage->setText("1");
    ui->totalPages->setText(QString::number(ui->stackedWidget->count()));
    setButtonState(ui->leftArrow, false);
    if (ui->stackedWidget->count() == 1) {
        setButtonState(ui->rightArrow, false);
    }
    else {
        setButtonState(ui->rightArrow, true);
    }
    placeHolder_->hide();
    movie_->stop();
}

void MainWindow::addChart(QChartView *chart)
{
    addChartButton(chart);
    showChart();

    setButtonState(ui->clearButton, true);
    setButtonState(ui->saveButton, true);
}

void MainWindow::addTabBarLine(QString type, QString data, QString station, QString startDate, QString endDate, QString bookmarkName)
{
    QWidget* line = new QWidget;
    line->setMinimumHeight(45);
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    line->setMaximumHeight(45);
    QHBoxLayout* layout = new QHBoxLayout;

    if (type == "selection" || type == "history") {
        QLabel* dataL = new QLabel;
        QLabel* stationL = new QLabel;
        QLabel* startDateL = new QLabel;
        QLabel* endDateL = new QLabel;

        dataL->setText(data);
        dataL->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        stationL->setText(station);
        stationL->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        startDateL->setText(startDate);
        startDateL->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        endDateL->setText(endDate);
        endDateL->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        setButtonState(dataL, true);
        setButtonState(stationL, true);
        setButtonState(startDateL, true);
        setButtonState(endDateL, true);

        layout->addWidget(dataL);
        layout->addWidget(stationL);
        layout->addWidget(startDateL);
        layout->addWidget(endDateL);
        layout->setContentsMargins(9,0,9,18);
    }
    else if (type == "bookmarks") {
        QPushButton* name = new QPushButton;
        name->setText(bookmarkName);
        setButtonState(name, true);
        layout->addWidget(name);
        layout->setContentsMargins(9,0,9,18);
        connect(name, &QPushButton::clicked, ctrl_,
                [=]() { ctrl_->loadBookmark(bookmarkName); });
    }

    if (type == "selection" || type == "bookmarks") {
        QPushButton* clear = new QPushButton;
        setButtonState(clear, true);
        clear->setFixedWidth(30);
        clear->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        clear->setIcon(QIcon("../icons/trash-can.svg"));

        QSpacerItem* space = new QSpacerItem(30, 20);
        layout->addItem(space);
        layout->addWidget(clear);

        if (type == "selection") {
            connect(clear, &QPushButton::clicked, this,
                    [=]() { this->deleteChart(line); });
        }
        if (type == "bookmarks") {
            connect(clear, &QPushButton::clicked, ctrl_,
                    [=]() { ctrl_->bookmarkDeleteButtonClicked(line); });
        }
    }

    line->setLayout(layout);

    if (type == "selection") {
        ctrl_->addCurrentSelection(data, station, startDate, endDate);
        ui->selectionsLayout->addWidget(line);
        currentLines_.push_back(line);
    }
    else if (type == "history") {
        ui->historyLayout->addWidget(line);
        history_.push_back(line);
        setButtonState(ui->clearButton2, true);
    }
    else if (type == "bookmarks") {
        ui->bookmarksLayout->addWidget(line);
        setButtonState(ui->clearButton3, true);
    }

}

QString MainWindow::getSaveFileLocation(QString title)
{
    return QFileDialog::getSaveFileName(this, tr("Save File"),
                               title,
                                        tr("Images (*.png *.jpg *.jpeg)"));
}

void MainWindow::setButtonState(QWidget *button, bool value)
{
    button->setEnabled(value);
    if (value) {
        button->setStyleSheet(buttonEnabledStyle);
    }
    else {
        button->setStyleSheet(buttonDisabledStyle);
    }
}

void MainWindow::clearHistory()
{
    deleteHistoryLines();
    setButtonState(ui->clearButton2, false);
}

void MainWindow::saveButtonClicked()
{
    ui->bookmarksDialogWidget->show();
    ui->lineEdit->setText("");
}

void MainWindow::timeout(bool manual)
{
    timer_->stop();
    placeHolder_->hide();
    movie_->stop();

    if(charts_.size() == 0){
        placeHolder_->setPixmap(QPixmap("../icons/leaf-solid.png"));
        placeHolder_->show();
        placeHolder_->lower();
    }

    if (!manual) {
        showError("timeout");
    }
    ui->dataComboBox->setState(true);
    ui->dataComboBox->setStyleSheet(enabledStyle);

    setButtonState(ui->settingsButton, true);
}

void MainWindow::errorTimeout()
{
    errorTimer_->stop();
    ui->errorLabel->hide();
}

void MainWindow::openChartInNewWindow(QChartView *chart)
{
    oldParent_ = chart->parentWidget()->layout();
    int currPageCount = ui->stackedWidget->count();

    for (int a = 0; a < currPageCount; ++a) {
        QWidget* widget = ui->stackedWidget->widget(0);
        ui->stackedWidget->removeWidget(widget);
        //widget->deleteLater(); // delete the widget
    }
    pages_.clear();

    QWidget* page = new QWidget;
    QGridLayout* grid = new QGridLayout;
    page->setLayout(grid);
    page->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    ui->stackedWidget->addWidget(page);
    grid->addWidget(chart);
    chart->show();

    chart->findChild<QPushButton*>("close")->show();
    chart->findChild<QPushButton*>("save")->hide();
}

void MainWindow::showBookmarkWidget(bool value)
{
    ui->bookmarksDialogWidget->setVisible(value);
    ui->label_10->setText("");
}

void MainWindow::showBookmarkError(QString type)
{
    ui->label_10->setText(type);
}

void MainWindow::deleteHistoryLines()
{
    for(auto line : history_){
        line->deleteLater();
    }
    history_.clear();
}

void MainWindow::requestCustomContextMenu(QChartView* chart, QPoint pos, QSize size)
{
    QMenu menu(this);
    auto act1 = menu.addAction("Open in new window", this, [=]() { this->openChartInNewWindow(chart); });
    auto act2 = menu.addAction("Save as image", ctrl_, [=]() { ctrl_->downloadChart(chart, chart->chart()->title()); });
    act1->setIcon(QIcon("../icons/arrow-up-right-from-square-solid.svg"));
    act2->setIcon(QIcon("../icons/floppy-disk.svg"));
    pos.setY(pos.y() + size.height());
    pos.setX(pos.x() - menu.sizeHint().width() + size.width());
    menu.exec(chart->mapToGlobal(pos));
}

void MainWindow::addChartButton(QChartView *chart)
{
    QPushButton* save = new QPushButton;
    QWidget* picture = new QWidget;
    QStackedLayout* layout = new QStackedLayout();
    QPushButton* close = new QPushButton;
    save->setFixedSize(20,20);
    save->setContentsMargins(10, 10, 10, 10);
    save->setStyleSheet("border:1px solid #A0A0A0; border-radius: 3px");

    close->setFixedSize(20,20);
    close->setContentsMargins(10, 10, 10, 10);
    close->setStyleSheet("border:1px solid #A0A0A0; border-radius: 3px");

    layout->setStackingMode(QStackedLayout::StackAll);
    save->setIcon(QIcon("../icons/ellipsis-vertical.png"));
    close->setIcon(QIcon("../icons/xmark-solid.svg"));

    picture->setLayout(layout);
    layout->addWidget(chart);


    connect(save, &QPushButton::clicked, this,
            [=]() { requestCustomContextMenu(chart, save->pos(), save->size()); });

    connect(close, &QPushButton::clicked, this,
            [=]() { hideChartPopup(chart); });

    QVBoxLayout * vlayout_PB_under = new QVBoxLayout();
    QHBoxLayout * hlayout_PB_under = new QHBoxLayout();

    chart->setLayout(vlayout_PB_under);
    vlayout_PB_under->addSpacing(10);
    vlayout_PB_under->addLayout(hlayout_PB_under);
    hlayout_PB_under->addWidget(save);
    hlayout_PB_under->addWidget(close);
    close->hide();
    close->setObjectName("close");
    save->setObjectName("save");
    hlayout_PB_under->addSpacing(10);

    vlayout_PB_under ->setAlignment(Qt::AlignRight);
    hlayout_PB_under->setAlignment(Qt::AlignTop );
    charts_.push_back(picture);
}

void MainWindow::showError(QString msg, QString extraData)
{
    timeout(true);
    ui->errorLabel->setText(errors_[msg]);
    ui->errorLabel->show();
    errorTimer_->start(7000);
    ctrl_->writeErrorLog(errors_[msg] + " - " + extraData);
}


void MainWindow::addChartButtonClicked()
{
    timer_->start(15000);
    QDate beginDate = ui->dateBegin->date();
    QDate endDate = ui->dateEnd->date();

    QStringList data = ui->dataComboBox->currentText();
    QStringList stations = ui->stationComboBox->currentText();
    ui->stationComboBox->resetSelection();
    ui->dataComboBox->resetSelection();

    placeHolder_->setStyleSheet("font: 17pt; background-color: rgba(255,255,255,150)");
    placeHolder_->setMovie(movie_);
    movie_->start();
    placeHolder_->show();
    placeHolder_->raise();

    ctrl_->fetchData(this, beginDate, endDate, data, stations);
    ui->dataComboBox->setState(false);
    ui->dataComboBox->setStyleSheet(disabledStyle);

    setButtonState(ui->settingsButton, false);
}

void MainWindow::settingsButtonToggled(bool value)
{

    int index = ui->tabWidget->currentIndex();
    if (value) {
        ui->tabWidget->setVisible(true);
        ui->topBar->hide();
        ui->stackedWidget->hide();
        ui->pageSelector->hide();
        placeHolder_->hide();


        tabChanged(index);
        ui->settingsButton->setStyleSheet(buttonDisabledStyle);


    } else {
        ui->settingsButton->setStyleSheet(buttonEnabledStyle);
        ui->menuLayout->addWidget(ui->settingsButton);

        ui->tabWidget->setVisible(false);
        ui->topBar->show();
        ui->stackedWidget->show();
        if (charts_.size() == 0) {
            placeHolder_->setPixmap(QPixmap("../icons/leaf-solid.png"));
            placeHolder_->show();
        }
        else {
            ui->pageSelector->show();
        }


    }


}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    windowHeight_ = this->height();
    windowWidth_ = this->width();
    placeHolder_->setGeometry(0, 75, windowWidth_, windowHeight_-75);
    ui->bookmarksDialogWidget->setGeometry(windowWidth_/2-ui->bookmarksDialogWidget->width()/2, windowHeight_/2-ui->bookmarksDialogWidget->height()/2, 300, 200);
}

void MainWindow::hideChartPopup(QChartView *chart)
{
    oldParent_->addWidget(chart);
    showChart();
    chart->findChild<QPushButton*>("close")->hide();
    chart->findChild<QPushButton*>("save")->show();

}


void MainWindow::clearCharts()
{
    charts_.clear();
    placeHolder_->setStyleSheet("font: 17pt; background-color: rgba(0,0,0,0)");
    placeHolder_->setText(QString("Please select data \n to display first..."));
    placeHolder_->show();

    int currPageCount = ui->stackedWidget->count();

    for (int a = 0; a < currPageCount; ++a) {
        QWidget* widget = ui->stackedWidget->widget(0);
        ui->stackedWidget->removeWidget(widget);
        widget->deleteLater();
    }
    pages_.clear();

    for (QWidget* widget: currentLines_) {
        ui->currentSelectionsLayout->removeWidget(widget);
        widget->deleteLater();
    }

    currentLines_.clear();
    ui->settingsButton->toggle();

    setButtonState(ui->clearButton, false);
    setButtonState(ui->saveButton, false);

}


void MainWindow::dateBeginEditingFinished()
{
    ui->dateEnd->setMinimumDate(ui->dateBegin->date().addDays(1));
    if (apitype_ == "smear") {
        ui->dateEnd->setMaximumDate(ui->dateBegin->date().addYears(2));
        ui->dateEnd->setDate(std::min(ui->dateBegin->date().addYears(2), today_));
    } else {
        ui->dateEnd->setMaximumDate(today_);
    }
}


void MainWindow::leftArrowClicked()
{
    if (ui->stackedWidget->currentIndex() != 0) {
        ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() - 1);
        ui->currPage->setText(QString::number(ui->stackedWidget->currentIndex() + 1));
        if (ui->stackedWidget->currentIndex() == 0) {
            setButtonState(ui->leftArrow, false);
        }
        setButtonState(ui->rightArrow, true);
    }
}


void MainWindow::rightArrowClicked()
{
    if (ui->stackedWidget->currentIndex() != ui->stackedWidget->count() - 1) {
        ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() + 1);
        ui->currPage->setText(QString::number(ui->stackedWidget->currentIndex() + 1));
        setButtonState(ui->leftArrow, true);
        if (ui->stackedWidget->currentIndex() == ui->stackedWidget->count() - 1) {
            setButtonState(ui->rightArrow, false);
        }
    }
}

void MainWindow::tabChanged(int index)
{
    for(int i = 0; i < ui->tabWidget->count(); i++){
        ui->tabWidget->tabBar()->setTabTextColor(i, QColor(255, 255, 255));
    }

    ui->tabWidget->tabBar()->setTabTextColor(index, QColor(237, 175, 200));

    if(index == 0){
        ui->currentTopLayout->addWidget(ui->settingsButton);

    } else if (index == 1){
        ui->historyTopLayout->addWidget(ui->settingsButton);

    } else if (index == 2){
        ui->bookmarkTopLayout->addWidget(ui->settingsButton);

    }
}

void MainWindow::hideChartButton(bool value)
{
    for (auto chart : charts_) {
        auto test = chart->findChild<QPushButton*>();
        if (value) {
            test->hide();
        }
        else {
            test->show();
        }
    }
}

void MainWindow::deleteChart(QWidget* widget)
{
    for(unsigned long long i = 0; i < currentLines_.size(); i++){
        if (widget == currentLines_.at(i)){
            charts_.erase(charts_.begin() + i);
            currentLines_.erase(currentLines_.begin() + i);
            ctrl_->clearCurrentSelections(i);
        }
    }

    widget->deleteLater();
    showChart();

    if (currentLines_.size() == 0){
        placeHolder_->setStyleSheet("font: 17pt; background-color: rgba(0,0,0,0)");
        placeHolder_->setText(QString("Please select data \n to display first..."));
        setButtonState(ui->clearButton, false);
        setButtonState(ui->saveButton, false);
    }
}

void MainWindow::clearButtonEnabled(bool value)
{
    setButtonState(ui->clearButton3, value);
}

void MainWindow::deleteAllBookmarks()
{
    while ( ui->scrollAreaWidgetContents_3->findChild<QWidget*>() != nullptr ) {
       delete ui->scrollAreaWidgetContents_3->findChild<QWidget*>();
    }
    setButtonState(ui->clearButton3, false);
}

