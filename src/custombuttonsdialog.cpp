#include "custombuttonsdialog.h"
#include "ui_custombuttonsdialog.h"
#include <QPushButton>

CustomButtonsDialog::CustomButtonsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomButtonsDialog)
{
    ui->setupUi(this);
}

CustomButtonsDialog::~CustomButtonsDialog()
{
    delete ui;
}

void CustomButtonsDialog::setMessage(const QString& message)
{
    ui->messageLabel->setText(message);
}

void CustomButtonsDialog::addButton(const QString& buttonText)
{
    QPushButton* button = new QPushButton(buttonText, this);
    connect(button, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    ui->buttonsLayout->addWidget(button);
    button->show();
}

QString CustomButtonsDialog::getSelectedButtonText() const
{
    return selectedButtonText;
}

void CustomButtonsDialog::onButtonClicked()
{
    QObject *senderObj = sender();
    QPushButton* button = dynamic_cast<QPushButton*>(senderObj);
    selectedButtonText = button->text();
	done(0);
}
