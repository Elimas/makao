#ifndef CUSTOMBUTTONSDIALOG_H
#define CUSTOMBUTTONSDIALOG_H

#include <QDialog>

namespace Ui {
class CustomButtonsDialog;
}

class CustomButtonsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CustomButtonsDialog(QWidget *parent = 0);
    ~CustomButtonsDialog();
    void setMessage(const QString& message);
    void addButton(const QString& buttonText);
    QString getSelectedButtonText() const;

private:
    Ui::CustomButtonsDialog *ui;
    QString selectedButtonText;
private slots:
    void onButtonClicked();
};

#endif // CUSTOMBUTTONSDIALOG_H
