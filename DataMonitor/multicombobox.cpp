#include "multicombobox.h"
#include <QCheckBox>
#include <QEvent>
#include <QLineEdit>
#include "mainwindow.h"

QString dataTypeNotify = "Data types from different sources need to be selected separately";

MultiComboBox::MultiComboBox(QWidget *aParent) :
    QComboBox(aParent),
    mListWidget(new QListWidget(this)),
    mLabel(new QLabel(this))
{
    setModel(mListWidget->model());
    setView(mListWidget);
    //setLineEdit(mLabel);

    //mLabel->setReadOnly(true);
    //mLabel->installEventFilter(this);
    mLabel->setFixedWidth(1.75 * this->width());
    mLabel->setFixedHeight(this->height());
    mLabel->setContentsMargins(5, 0, 0, 0);

    connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &MultiComboBox::itemClicked);
}

void MultiComboBox::hidePopup()
{
    int width = this->width();
    int height = mListWidget->height();
    int x = QCursor::pos().x() - mapToGlobal(geometry().topLeft()).x() + geometry().x();
    int y = QCursor::pos().y() - mapToGlobal(geometry().topLeft()).y() + geometry().y();
    if (x >= 0 && x <= width && y >= this->height() && y <= height + this->height())
    {
        // Item was clicked, do not hide popup
    }
    else
    {
        QComboBox::hidePopup();
    }
}

void MultiComboBox::addItem(const QString &aText, const QVariant &aUserData)
{
    Q_UNUSED(aUserData);
    QListWidgetItem* listWidgetItem = new QListWidgetItem(mListWidget);
    if (aText != "Smear" && aText != "Stat.fi") {
        QCheckBox* checkBox = new QCheckBox(this);
        checkBox->setText(aText);
        mListWidget->addItem(listWidgetItem);
        mListWidget->setItemWidget(listWidgetItem, checkBox);
        connect(checkBox, &QCheckBox::stateChanged, this, &MultiComboBox::stateChanged);
    }
    else {
        QLabel* label = new QLabel;
        label->setText(aText);
        label->setAlignment(Qt::AlignHCenter);
        label->setStyleSheet("font-weight: bold");
        //label->setEnabled(false);
        mListWidget->addItem(listWidgetItem);
        mListWidget->setItemWidget(listWidgetItem, label);
    }
}

void MultiComboBox::addItems(const QStringList &aTexts)
{
    for(const auto& string : aTexts)
    {
        addItem(string);
    }
}

int MultiComboBox::count() const
{
    int count = mListWidget->count();
    if(count < 0)
    {
        count = 0;
    }
    return count;
}

void MultiComboBox::setPlaceHolderText(const QString &aPlaceHolderText)
{
    placeholder_ = aPlaceHolderText;
    mLabel->setText(placeholder_);
}

void MultiComboBox::resetSelection()
{
    int count = mListWidget->count();

    for (int i = 0; i < count; ++i)
    {
        if (isDataComboBox_ && (i == 0 || i == 4)) {
            continue;
        }
        QWidget *widget = mListWidget->itemWidget(mListWidget->item(i));
        QCheckBox *checkBox = static_cast<QCheckBox *>(widget);
        checkBox->setChecked(false);
    }
}

QStringList MultiComboBox::currentText()
{
    QStringList emptyStringList;
    if(!mLabel->text().isEmpty())
    {
        emptyStringList = mLabel->text().split(';');
    }
    return emptyStringList;
}

void MultiComboBox::setState(bool val)
{
    if (val) {
        this->setEnabled(true);
    }
    else {
        setEnabled(false);
        resetSelection();
    }
}

void MultiComboBox::getMainWindow(MainWindow *window)
{
    currWindow_ = window;
}

void MultiComboBox::wheelEvent(QWheelEvent *aWheelEvent)
{
    // Do not handle the wheel event
    Q_UNUSED(aWheelEvent);
}

bool MultiComboBox::eventFilter(QObject *aObject, QEvent *aEvent)
{
    if(aObject == mLabel && aEvent->type() == QEvent::MouseButtonRelease) {
        showPopup();
        return false;
    }
    return false;
}

void MultiComboBox::keyPressEvent(QKeyEvent *aEvent)
{
    // Do not handle key event
    Q_UNUSED(aEvent);
}

void MultiComboBox::itemClicked(int aIndex)
{
    if (isDataComboBox_ && (aIndex == 0 || aIndex == 4)) {
        stateChanged(0);
        return;
    }
    QWidget* widget = mListWidget->itemWidget(mListWidget->item(aIndex));
    QCheckBox *checkBox = static_cast<QCheckBox *>(widget);
    checkBox->setChecked(!checkBox->isChecked());
}

void MultiComboBox::stateChanged(int aState)
{
    Q_UNUSED(aState);
    QString selectedData("");
    int count = mListWidget->count();

    for (int i = 0; i < count; ++i)
    {
        if (isDataComboBox_ && (i == 0 || i == 4)) {
            continue;
        }
        QWidget *widget = mListWidget->itemWidget(mListWidget->item(i));
        QCheckBox *checkBox = static_cast<QCheckBox *>(widget);

        if (checkBox->isChecked()) {
            selectedData.append(checkBox->text()).append(";");
            if (i < 4) {
                smearOrStatfi_ = "smear";
            }
            else {
                smearOrStatfi_ = "statfi";
            }
        }
    }

    if (selectedData.endsWith(";"))
    {
        selectedData.remove(selectedData.count() - 1, 1);
    }
    if (!selectedData.isEmpty())
    {
        mLabel->setText(selectedData);
    }
    else
    {
        setPlaceHolderText(placeholder_);
        smearOrStatfi_ = "none";
    }

    if (smearOrStatfi_ == "smear") {
        currWindow_->comboBoxSelections(this->objectName(), "smear");
        for (int j = 5; j < count; ++j) {
            QWidget* widget = mListWidget->itemWidget(mListWidget->item(j));
            widget->setToolTip(dataTypeNotify);
            widget->setEnabled(false);
        }
    }
    else if (smearOrStatfi_ == "statfi") {
        currWindow_->comboBoxSelections(this->objectName(), "statfi");
        for (int j = 1; j < 4; ++j) {
            QWidget* widget = mListWidget->itemWidget(mListWidget->item(j));
            widget->setToolTip(dataTypeNotify);
            widget->setEnabled(false);
        }
    }
    else {
        for (int j = 0; j < count; ++j) {
            if (j != 0 && j != 4) {
                currWindow_->comboBoxSelections(this->objectName(), "none");
                QWidget* widget = mListWidget->itemWidget(mListWidget->item(j));
                widget->setToolTip("");
                widget->setEnabled(true);
            }
        }
    }

    emit selectionChanged();
}

void MultiComboBox::setDataComboBox(bool val)
{
    if (val) {
        isDataComboBox_ = true;
    }
    else {
        isDataComboBox_ = false;
    }
}
