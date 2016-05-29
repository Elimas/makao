#include "utils.h"
#include <QDebug>
Utils::Utils()
{

}

void Utils::showNotBlockingMessageBox(QWidget* parent, const QString &title, const QString &message, QMessageBox::Icon icon)
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
int Utils::showJackSelectionMessageBox(QWidget* parent, QString &title, QString &message, QMessageBox::Icon icon)
{
    QMessageBox msgBox;
    QPushButton* pButton5 = msgBox.addButton("5tek", QMessageBox::AcceptRole);
    QPushButton* pButton6 = msgBox.addButton("6tek", QMessageBox::AcceptRole);
    QPushButton* pButton7 = msgBox.addButton("7mek",QMessageBox::AcceptRole);
    QPushButton* pButton8 = msgBox.addButton("8mek", QMessageBox::AcceptRole);
    QPushButton* pButton9 = msgBox.addButton("9tek", QMessageBox::AcceptRole);
    QPushButton* pButton10 = msgBox.addButton("10tek", QMessageBox::AcceptRole);
    QPushButton* pButtonNothing = msgBox.addButton("Niczego", QMessageBox::AcceptRole);
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    msgBox.setIcon(icon);
    msgBox.setModal(true);
    msgBox.setObjectName("messageBox");
    msgBox.exec();
    QAbstractButton* compare = msgBox.clickedButton();
    if(compare==pButton5)
    {
        return 5;
    }
    else if(compare==pButton6)
    {
        return 6;
    }
    else if(compare==pButton7)
    {
        return 7;
    }
    else if(compare==pButton8)
    {
        return 8;
    }
    else if(compare==pButton9)
    {
        return 9;
    }
    else if(compare==pButton10)
    {
        return 10;
    }
    else if(compare==pButtonNothing)
    {
        return 0;
    }
    else return -1;
}
