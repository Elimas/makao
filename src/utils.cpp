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
int Utils::showJackSelectionMessageBox(QWidget* parent, QString &title, QString &message, QMessageBox::Icon icon)
{
    QMessageBox* msgBox = new QMessageBox(parent);
    msgBox->setAttribute( Qt::WA_DeleteOnClose );
    QAbstractButton* pButton5 = msgBox->addButton("5tek", QMessageBox::AcceptRole);
    QAbstractButton* pButton6 = msgBox->addButton("6tek", QMessageBox::AcceptRole);
    QAbstractButton* pButton7 = msgBox->addButton("7mek",QMessageBox::AcceptRole);
    QAbstractButton* pButton8 = msgBox->addButton("8mek", QMessageBox::AcceptRole);
    QAbstractButton* pButton9 = msgBox->addButton("9tek", QMessageBox::AcceptRole);
    QAbstractButton* pButton10 = msgBox->addButton("10tek", QMessageBox::AcceptRole);
    QAbstractButton* pButtonNothing = msgBox->addButton("Niczego", QMessageBox::AcceptRole);
    msgBox->setWindowTitle(title);
    msgBox->setText(message);
    msgBox->setIcon(icon);
    msgBox->setModal(true);
    msgBox->setObjectName("messageBox");
    msgBox->open();
    if(msgBox->clickedButton()==pButton5)
    {
        return 5;
    }
    else if(msgBox->clickedButton()==pButton6)
    {
        return 6;
    }
    else if(msgBox->clickedButton()==pButton7)
    {
        return 7;
    }
    else if(msgBox->clickedButton()==pButton8)
    {
        return 8;
    }
    else if(msgBox->clickedButton()==pButton9)
    {
        return 9;
    }
    else if(msgBox->clickedButton()==pButton10)
    {
        return 9;
    }
    else
    {
        return 0;
    }
}
