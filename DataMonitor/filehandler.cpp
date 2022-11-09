#include "filehandler.h"
#include <QDebug>

FileHandler::FileHandler()
{

}

void FileHandler::save(QString location, QString data)
{
    std::fstream file;
    file.open(location.toStdString(), std::fstream::app);
    file << data.toStdString();
    file.close();
}

QStringList FileHandler::load(QString location)
{
    std::ifstream file;
    file.open(location.toStdString());
    std::string line;
    QStringList list;

    while(std::getline(file, line)) {
        list.append(QString::fromStdString(line));
    }

    file.close();

    return list;
}

void FileHandler::clearFile(QString location)
{
    std::ofstream file;
    file.open(location.toStdString(), std::ofstream::out | std::ofstream::trunc);
    file.close();
}

void FileHandler::saveImage(QWidget* widget, QString fileLocation)
{
    widget->resize(QSize(1920,1080));
    QPixmap p( widget->size() );
        widget->render( &p );
    auto splitted = fileLocation.split(".");
    if (splitted.at(splitted.size() - 1) == "jpg" || splitted.at(splitted.size() - 1) == "jpeg") {
        p.save(fileLocation, "JPEG");
    }
    else {
        p.save(fileLocation, "PNG");
    }
}

void FileHandler::removeLine(QString deleteline, QString location)
{
    std::fstream file;
    file.open(location.toStdString());
    std::string line;
    std::ofstream temp;
    temp.open("temp.txt");

    while (std::getline(file,line)) {
        std::string id(line.begin(), line.end());
        qDebug() << QString::fromStdString(id);
        if (id != deleteline.toStdString())
            temp << line << std::endl;
    }
    file.close();
    temp.close();

    // assigning value to string s

    int n = location.length(); // declaring character array
    char char_array[n + 1]; // copying the contents of the // string to char array
    strcpy(char_array, location.toStdString().c_str());

    std::remove(char_array);
    std::rename("temp.txt", char_array);
}
