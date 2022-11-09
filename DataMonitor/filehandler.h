#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <QObject>
#include <fstream>
#include "mainwindow.h"

class FileHandler: public QObject
{
    Q_OBJECT
public:
    /**
     * @brief FileHandler
     */
    FileHandler();

    /**
     * @brief save saves data to file
     * @param location Where additions are done
     * @param data What is added
     */
    void save(QString location, QString data);

    /**
     * @brief load loads data from file
     * @param location From where file is loaded
     * @return QStringList containing file contents
     */
    QStringList load(QString location);

    /**
     * @brief clearFile clears file
     * @param location Where file si cleared
     */
    void clearFile(QString location);

    /**
     * @brief removeLine deletes line in file
     * @param deleteline Line to be deleted
     * @param location Where line is deleted
     */
    void removeLine(QString deleteline, QString location);

    /**
     * @brief saveImage saves chart as image
     * @param widget Chart to be saved
     * @param fileLocation Where chart is save
     */
    void saveImage(QWidget *widget, QString fileLocation);
};

#endif // FILEHANDLER_H
