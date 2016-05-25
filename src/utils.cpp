#include "utils.h"

Utils::Utils()
{

}

void Utils::showNotBlockingMessageBox(QWidget* parent, QString &title, QString &message, QMessageBox::Icon icon)
{
    QMessageBox* msgBox = new QMessageBox(parent);
    msgBox->setAttribute( Qt::WA_DeleteOnClose );
    msgBox->setStandardButtons( QMessageBox::Ok );
    msgBox->setWindowTitle(title);
    msgBox->setText(message);
    msgBox->setIcon(icon);
    msgBox->setModal(true);
    msgBox->setObjectName("messageBox");
    msgBox->open();
}
