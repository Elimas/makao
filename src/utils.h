#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QMessageBox>

class Utils
{
public:
    Utils();
    static void showNotBlockingMessageBox(QWidget* parent, const QString &title, const QString &message, QMessageBox::Icon icon);
};

#endif // UTILS_H
