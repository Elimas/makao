#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QMessageBox>
#include <QPushButton>

class Utils
{
public:
    Utils();
    static void showNotBlockingMessageBox(QWidget* parent, const QString &title, const QString &message, QMessageBox::Icon icon);
    static int showJackSelectionMessageBox(QWidget* parent, QString &title, QString &message, QMessageBox::Icon icon);
};

#endif // UTILS_H
