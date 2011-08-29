#include "virtualkeyboarddialog.h"
#include "ui_virtualkeyboarddialog.h"

#include <QMouseEvent>
#include <QFont>

#include <QDebug>

QScopedPointer<VirtualKeyboardDialog> VirtualKeyboardDialog::instance_;

VirtualKeyboardDialog::VirtualKeyboardDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VirtualKeyboardDialog)
{
    ui->setupUi(this);

    this->setWindowTitle(tr("VirtualKeyboard"));

    QFont font = ui->display->font();
    font.setPointSize(font.pointSize() + 8);
    ui->display->setFont(font);

    ui->display->setFocus();
    ui->ok->setDefault(true);

    QObjectList objList = ui->buttonBoxWidget->children();
    foreach(QObject *obj,objList)
    {
        QPushButton *button = qobject_cast<QPushButton *>(obj);
        connect(button,
                SIGNAL(clicked()),
                this,
                SLOT(KeyboardClicked()));
    }
}

VirtualKeyboardDialog::~VirtualKeyboardDialog()
{
    delete ui;
}

void VirtualKeyboardDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

//public slots:
void VirtualKeyboardDialog::ResetDisplay()
{
    //qDebug()<<(ui->display == NULL);
    ui->display->clear();
    ui->display->setFocus();
}

//private slots
void VirtualKeyboardDialog::on_cancel_clicked()
{
    this->accept();
}

void VirtualKeyboardDialog::on_backspace_clicked()
{
    QString displayString = ui->display->text();
    int cursorPosition = ui->display->cursorPosition() - 1;
    displayString.remove(cursorPosition, 1);
    ui->display->setText(displayString);
    ui->display->setCursorPosition(cursorPosition);
    ui->display->setFocus();
}

void VirtualKeyboardDialog::on_del_clicked()
{
    QString displayString = ui->display->text();
    int cursorPosition = ui->display->cursorPosition();
    displayString.remove(cursorPosition, 1);
    ui->display->setText(displayString);
    ui->display->setCursorPosition(cursorPosition);
    ui->display->setFocus();
}
//    qDebug("after");
void VirtualKeyboardDialog::on_space_clicked()
{
    QString displayString = ui->display->text();
    int cursorPosition = ui->display->cursorPosition();
    displayString.insert(cursorPosition, 0x20);
    ui->display->setText(displayString);
    ui->display->setCursorPosition(++cursorPosition);
    ui->display->setFocus();
}

void VirtualKeyboardDialog::on_ok_clicked()
{
    emit EnterComplete(ui->display->text());
    this->accept();
}

void VirtualKeyboardDialog::KeyboardClicked()
{
    QPushButton * clickedButton = qobject_cast<QPushButton *>(sender());
    QChar addedValue = clickedButton->text()[0];
    if(addedValue.isLetter())
    {
        addedValue = addedValue.toAscii() + 0x20;
    }
    int cursorPosition = ui->display->cursorPosition();
    QString disPlayValue = ui->display->text();
    disPlayValue.insert(cursorPosition, addedValue);

    ui->display->setText(disPlayValue);
    ui->display->setCursorPosition(++cursorPosition);
    ui->display->setFocus();
}

void VirtualKeyboardDialog::on_prior_clicked()
{
    ui->display->cursorBackward(false);
    ui->display->setFocus();
}

void VirtualKeyboardDialog::on_next_clicked()
{
    ui->display->cursorForward(false);
    ui->display->setFocus();
}

void VirtualKeyboardDialog::on_home_clicked()
{
    ui->display->setCursorPosition(0);
    ui->display->setFocus();
}

void VirtualKeyboardDialog::on_end_clicked()
{
    ui->display->setCursorPosition(ui->display->text().length());
    ui->display->setFocus();
}
