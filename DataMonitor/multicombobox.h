#ifndef MULTICOMBOBOX_H
#define MULTICOMBOBOX_H

#include <QComboBox>
#include <QListWidgetItem>
#include <QLabel>

class MultiComboBox: public QComboBox
{
    Q_OBJECT
public:
    /**
     * @brief MultiComboBox
     * @param aParent
     */
    MultiComboBox(QWidget* aParent = Q_NULLPTR);

    /**
     * @brief hidePopup Prevents the popup from closing when options are clicked
     */
    void hidePopup() override;

    /**
     * @brief addItem Adds selectable item to combo box
     * @param aText Item label
     * @param aUserData
     */
    void addItem(const QString& aText, const QVariant& aUserData = QVariant());

    /**
     * @brief addItems Adds all items to combo box
     * @param aTexts Item lables
     */
    void addItems(const QStringList& aTexts);

    /**
     * @brief count Returns the number of selectable combo box items
     * @return Combo box item count
     */
    int count() const;

    /**
     * @brief setPlaceHolderText Sets the label at the top of the combo box
     * @param aPlaceHolderText Text to display
     */
    void setPlaceHolderText(const QString& aPlaceHolderText);

    /**
     * @brief resetSelection Resets combo box selection
     */
    void resetSelection();

    /**
     * @brief currentText Returns currently selected items
     * @return Current selection
     */
    QStringList currentText();

    /**
     * @brief setState Sets the combo box to enabled or disabled
     * @param val State
     */
    void setState(bool val);

    /**
     * @brief getMainWindow Getter for MainWindow
     * @param window
     */
    void getMainWindow(class MainWindow* window);

    /**
     * @brief stateChanged Processes selections
     * @param aState
     */
    virtual void stateChanged(int aState);

    /**
     * @brief setDataComboBox Sets the combo box to DataComboBox variant
     * @param val Boolean
     */
    void setDataComboBox(bool val);

protected:
    /**
     * @brief wheelEvent Do not handle wheel event
     * @param aWheelEvent
     */
    void wheelEvent(QWheelEvent* aWheelEvent) override;

    /**
     * @brief eventFilter Show popup when clicking the label
     * @param aObject
     * @param aEvent
     * @return
     */
    bool eventFilter(QObject* aObject, QEvent* aEvent) override;

    /**
     * @brief keyPressEvent Do not handle key event
     * @param aEvent
     */
    void keyPressEvent(QKeyEvent* aEvent) override;

signals:
    /**
     * @brief selectionChanged Notify that the selection has changed
     */
    void selectionChanged();

private:
    // List of selectable items
    QListWidget* mListWidget;

    // Top label to display selected data
    QLabel* mLabel;

    /**
     * @brief itemClicked Sets checkbox as checked when clicked
     * @param aIndex Index of the item clicked
     */
    void itemClicked(int aIndex);

    // MainWindow reference to notify about selections
    class MainWindow* currWindow_;

    // Is DataComboBox variant boolean
    bool isDataComboBox_ = false;

    // Tells what type of data is selected
    QString smearOrStatfi_ = "none";

    // Top label text
    QString placeholder_ = "placeholder";
};

#endif // MULTICOMBOBOX_H
