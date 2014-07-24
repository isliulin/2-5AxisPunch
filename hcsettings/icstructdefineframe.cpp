#include "icstructdefineframe.h"
#include "ui_icstructdefineframe.h"

#include "iccommandprocessor.h"
#include "iccommands.h"
#include "icvirtualhost.h"
#include "icparameterssave.h"

#include <QVector>
#include <QMessageBox>
#include <QDebug>
#include "mainframe.h"

typedef union {
     struct {
        u_int16_t canType : 8;
        u_int16_t  canAddr:8;
    }b;
    u_int16_t all;
}CanConfig;

typedef union{
     struct{
        u_int16_t r1 : 1;
        u_int16_t r2 : 1;
        u_int16_t r3 : 1;
        u_int16_t r4 : 1;
        u_int16_t r5 : 1;
        u_int16_t r6 : 1;
        u_int16_t r7 : 1;
        u_int16_t r8 : 1;
        u_int16_t r : 8;
    }b;
    u_int16_t all;
}ReserveProgConfig;

typedef union{
    struct {
        u_int16_t a1 : 3;
        u_int16_t a2 : 3;
        u_int16_t a3 : 3;
        u_int16_t a4 : 3;
        u_int16_t a5 : 3;
        u_int16_t resv : 1;
    }mode;
    u_int16_t allMode;
}AxisMode;


ICStructDefineFrame::ICStructDefineFrame(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ICStructDefineFrame)
{
    ui->setupUi(this);

    ICVirtualHost* host = ICVirtualHost::GlobalVirtualHost();

    /*axis define*/
    boxToAxis_.insert(ui->x1Box, ICVirtualHost::ICAxis_AxisX1);
    boxToAxis_.insert(ui->y1Box, ICVirtualHost::ICAxis_AxisY1);
    boxToAxis_.insert(ui->x2Box, ICVirtualHost::ICAxis_AxisX2);
    boxToAxis_.insert(ui->y2Box, ICVirtualHost::ICAxis_AxisY2);
    boxToAxis_.insert(ui->zBox, ICVirtualHost::ICAxis_AxisZ);
    boxToAxis_.insert(ui->aBox, ICVirtualHost::ICAxis_AxisA);
    boxToAxis_.insert(ui->bBox, ICVirtualHost::ICAxis_AxisB);
    boxToAxis_.insert(ui->cBox, ICVirtualHost::ICAxis_AxisC);

    armDefineToIndex_.insert(ICVirtualHost::ICAxisDefine_None, 0);
    armDefineToIndex_.insert(ICVirtualHost::ICAxisDefine_Pneumatic, 1);
    armDefineToIndex_.insert(ICVirtualHost::ICAxisDefine_Servo, 2);
    indexToArmDefine_.insert(0, ICVirtualHost::ICAxisDefine_None);
    indexToArmDefine_.insert(1, ICVirtualHost::ICAxisDefine_Pneumatic);
    indexToArmDefine_.insert(2, ICVirtualHost::ICAxisDefine_Servo);

    QList<QComboBox*> boxs = ui->armDefineBox->findChildren<QComboBox*>();
    axisDefine_ = host->SystemParameter(ICVirtualHost::SYS_Config_Arm).toInt();
    for(int i = 0; i != boxs.size(); ++i)
    {
        boxs[i]->setCurrentIndex(armDefineToIndex_.value(host->AxisDefine(static_cast<ICVirtualHost::ICAxis>(boxToAxis_.value(boxs.at(i))))));
        connect(boxs[i],
                SIGNAL(currentIndexChanged(int)),
                SLOT(OnAxisDefineChanged(int)));
    }
    InitEscapeBox() ;


//    outputDefineToNumber_.insert(ui->outABox, 0);
//    outputDefineToNumber_.insert(ui->outBBox, 1);
//    outputDefineToNumber_.insert(ui->outCBox, 2);
//    outputDefineToNumber_.insert(ui->outDBox, 3);
//    outputDefineToNumber_.insert(ui->outEBox, 4);
//    outputDefineToNumber_.insert(ui->outFBox, 5);
//    outputDefineToNumber_.insert(ui->outGBox, 6);
//    outputDefineToNumber_.insert(ui->outHBox, 7);

//    numberToOutputDefine_.insert(0, ui->outABox);
//    numberToOutputDefine_.insert(1, ui->outBBox);
//    numberToOutputDefine_.insert(2, ui->outCBox);
//    numberToOutputDefine_.insert(3, ui->outDBox);
//    numberToOutputDefine_.insert(4, ui->outEBox);
//    numberToOutputDefine_.insert(5, ui->outFBox);
//    numberToOutputDefine_.insert(6, ui->outGBox);
//    numberToOutputDefine_.insert(7, ui->outHBox);

//    ui->fixtureSelectBox->setCurrentIndex(host->FixtureDefine());
  //  ui->escapeComboBox->setCurrentIndex(host->EscapeWay());

    punchButtons_<<ui->punch1<<ui->punch2<<ui->punch3<<ui->punch4<<ui->punch5<<ui->punch6
                   <<ui->punch7<<ui->punch8<<ui->punch9<<ui->punch10;

    int machineCount = ICVirtualHost::GlobalVirtualHost()->SystemParameter(ICVirtualHost::SYS_Config_Fixture).toInt();
    for(int i = 0; i != punchButtons_.size(); ++i)
    {
        if(machineCount & ((1 << (i))))
        {
            punchButtons_[i]->setChecked(true);
        }
    }

    CanConfig canConfig;
    canConfig.all = ICVirtualHost::GlobalVirtualHost()->SystemParameter(ICVirtualHost::SYS_Config_Resv1).toInt();
    ui->canType->setCurrentIndex(canConfig.b.canType);
    ui->canID->SetThisIntToThisText(canConfig.b.canAddr);

    ReserveProgConfig progConfig;
    progConfig.all =  ICVirtualHost::GlobalVirtualHost()->SystemParameter(ICVirtualHost::SYS_Config_Resv2).toInt();
    ui->rP1->setCurrentIndex(progConfig.b.r1);
    ui->rP2->setCurrentIndex(progConfig.b.r2);
    ui->rP3->setCurrentIndex(progConfig.b.r3);
    ui->rP4->setCurrentIndex(progConfig.b.r4);
    ui->rP5->setCurrentIndex(progConfig.b.r5);
    ui->rP6->setCurrentIndex(progConfig.b.r6);
    ui->rP7->setCurrentIndex(progConfig.b.r7);
    ui->rP8->setCurrentIndex(progConfig.b.r8);

    AxisMode axisMode;
    axisMode.allMode = ICVirtualHost::GlobalVirtualHost()->SystemParameter(ICVirtualHost::SYS_Config_Out).toInt();
    ui->a1->setCurrentIndex(axisMode.mode.a1);
    ui->a2->setCurrentIndex(axisMode.mode.a2);
    ui->a3->setCurrentIndex(axisMode.mode.a3);
    ui->a4->setCurrentIndex(axisMode.mode.a4);
    ui->a5->setCurrentIndex(axisMode.mode.a5);

//    ui->fixtureDefineBox_2->hide();
}

ICStructDefineFrame::~ICStructDefineFrame()
{
    delete ui;
}

void ICStructDefineFrame::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
     //  ui->retranslateUi(this);
       retranslateUi_();
        break;
    default:
        break;
    }
}

void ICStructDefineFrame::retranslateUi_()
{
    this->setWindowTitle(tr("Form"));
    ui->armDefineBox->setTitle(tr("Arm Define"));
    ui->label_5->setText(tr("X1"));
    ui->x1Box->setItemText(0,tr("None"));
    ui->x1Box->setItemText(1,tr("Pneumatic"));
    ui->x1Box->setItemText(2,tr("Servo"));
    ui->label_14->setText(tr("C"));
    ui->y1Box->setItemText(0,tr("None"));
    ui->y1Box->setItemText(1,tr("Pneumatic"));
    ui->y1Box->setItemText(2,tr("Servo"));
    ui->label_13->setText(tr("Z"));
    ui->cBox->setItemText(0,tr("None"));
    ui->cBox->setItemText(1,tr("Pneumatic"));
    ui->cBox->setItemText(2,tr("Servo"));
    ui->label_15->setText(tr("A"));
    ui->aBox->setItemText(0,tr("None"));
    ui->aBox->setItemText(1,tr("Pneumatic"));
    ui->aBox->setItemText(2,tr("Servo"));
    ui->label_16->setText(tr("B"));
    ui->zBox->setItemText(0,tr("None"));
    ui->zBox->setItemText(1,tr("Pneumatic"));
    ui->zBox->setItemText(2,tr("Servo"));
    ui->label_7->setText(tr("Y1"));
    ui->bBox->setItemText(0,tr("None"));
    ui->bBox->setItemText(1,tr("Pneumatic"));
    ui->bBox->setItemText(2,tr("Servo"));
    ui->label_6->setText(tr("X2"));
    ui->x2Box->setItemText(0,tr("None"));
    ui->x2Box->setItemText(1,tr("Pneumatic"));
    ui->x2Box->setItemText(2,tr("Servo"));
    ui->label_12->setText(tr("Y2"));
    ui->y2Box->setItemText(0,tr("None"));
    ui->y2Box->setItemText(1,tr("Pneumatic"));
    ui->y2Box->setItemText(2,tr("Servo"));
//    ui->outABox->setItemText(0,tr("Normal"));
//    ui->outABox->setItemText(1,tr("Extent"));
//    ui->label_8->setText(tr("5"));
//    ui->outEBox->setItemText(0,tr("Normal"));
//    ui->outEBox->setItemText(1,tr("Extent"));
//    ui->label_2->setText(tr("2"));
//    ui->outBBox->setItemText(0,tr("Normal"));
//    ui->outBBox->setItemText(1,tr("Extent"));
//    ui->label_9->setText(tr("6"));
//    ui->outFBox->setItemText(0,tr("Normal"));
//    ui->outFBox->setItemText(1,tr("Extent"));
//    ui->label_3->setText(tr("3"));
//    ui->outCBox->setItemText(0,tr("Normal"));
//    ui->outCBox->setItemText(1,tr("Extent"));
//    ui->label_11->setText(tr("7"));
//    ui->outGBox->setItemText(0,tr("Normal"));
//    ui->outGBox->setItemText(1,tr("Extent"));
//    ui->label_4->setText(tr("4"));
//    ui->outDBox->setItemText(0,tr("Normal"));
//    ui->outDBox->setItemText(1,tr("Extent"));
//    ui->label_10->setText(tr("8"));
//    ui->outHBox->setItemText(0,tr("Normal"));
//    ui->outHBox->setItemText(1,tr("Extent"));

    ui->fixtureDefineBox_2->setTitle(tr("Other Define"));
    ui->saveButton->setText(tr("Save"));
}

void ICStructDefineFrame::on_saveButton_clicked()
{
    ICSetAxisConfigsCommand command;
    ICCommandProcessor* process = ICCommandProcessor::Instance();
    int sum = 0;
    int machineCount = 0;
    for(int i = 0; i != punchButtons_.size(); ++i)
    {
        if(punchButtons_.at(i)->isChecked())
        {
            machineCount |= (1 << i);
        }
    }
    ICVirtualHost::GlobalVirtualHost()->SetSystemParameter(ICVirtualHost::SYS_Config_Fixture, machineCount);
    AxisMode axisMode;
    axisMode.mode.a1 = ui->a1->currentIndex();
    axisMode.mode.a2 = ui->a2->currentIndex();
    axisMode.mode.a3 = ui->a3->currentIndex();
    axisMode.mode.a4 = ui->a4->currentIndex();
    axisMode.mode.a5 = ui->a5->currentIndex();
    QVector<uint> dataBuffer(7, 0);
    dataBuffer[0] = 0;
    dataBuffer[1] = axisDefine_;
    dataBuffer[2] = axisMode.allMode;
//    dataBuffer[3] = ICVirtualHost::GlobalVirtualHost()->FixtureDefineSwitch(ui->fixtureSelectBox->currentIndex());
    dataBuffer[3] = machineCount;
    CanConfig canConfig;
    canConfig.b.canType = ui->canType->currentIndex();
    canConfig.b.canAddr = ui->canID->TransThisTextToThisInt();
    ReserveProgConfig progConfig;
    progConfig.b.r1 = ui->rP1->currentIndex();
    progConfig.b.r2 = ui->rP2->currentIndex();
    progConfig.b.r3 = ui->rP3->currentIndex();
    progConfig.b.r4 = ui->rP4->currentIndex();
    progConfig.b.r5 = ui->rP5->currentIndex();
    progConfig.b.r6 = ui->rP6->currentIndex();
    progConfig.b.r7 = ui->rP7->currentIndex();
    progConfig.b.r8 = ui->rP8->currentIndex();
    dataBuffer[4] = canConfig.all;
    dataBuffer[5] = progConfig.all;
    for(int i = 0; i != 6; ++i)
    {
        sum += dataBuffer.at(i);
    }
    sum = (-sum & 0XFFFF);
    dataBuffer[6] = sum;
    command.SetSlave(process->SlaveID());
    command.SetDataBuffer(dataBuffer);
    command.SetAxis(8);
#ifndef Q_WS_X11
    if(process->ExecuteCommand(command).toBool())
#endif
    {
        ICVirtualHost* host = ICVirtualHost::GlobalVirtualHost();
        host->SetAxisDefine(axisDefine_);
        host->SetPeripheryOutput(0);
        host->SetSystemParameter(ICVirtualHost::SYS_Config_Fixture, dataBuffer.at(3));
        host->SetSystemParameter(ICVirtualHost::SYS_Config_Resv1, dataBuffer.at(4));
        host->SetSystemParameter(ICVirtualHost::SYS_Config_Resv2, dataBuffer.at(5));
        host->SetSystemParameter(ICVirtualHost::SYS_Config_Xorsum, dataBuffer.at(6));
        host->SetSystemParameter(ICVirtualHost::SYS_ARM_CONFIG, 0);
        host->SetSystemParameter(ICVirtualHost::SYS_Config_Signal, 0);
        host->SetSystemParameter(ICVirtualHost::SYS_Config_Out, dataBuffer[2]);
//        host->SystemParameter(ICVirtualHost::SYS_Function);
        host->SaveSystemConfig();
        QMessageBox::information(this, tr("Tips"), tr("Save Sucessfully!"));
        emit StructChanged();
        icMainFrame->UpdateAxisDefine_();
    }
    qDebug()<<"Arm Define"<<axisDefine_;
}



void ICStructDefineFrame::OnAxisDefineChanged(int index)
{
    QComboBox* box = qobject_cast<QComboBox*>(sender());
    ICVirtualHost::GlobalVirtualHost()->CalAxisDefine(axisDefine_,
                                                      static_cast<ICVirtualHost::ICAxis>(boxToAxis_.value(box)),
                                                      static_cast<ICVirtualHost::ICAxisDefine>(indexToArmDefine_.value(index)));
}


void ICStructDefineFrame::escapeBoxChange()
{
    ICVirtualHost::GlobalVirtualHost()->SetEscapeWay(buttongroup_->checkedId());
}

void ICStructDefineFrame::InitEscapeBox()
{
    buttongroup_ = new QButtonGroup();
    QList<QAbstractButton*> buttons = buttongroup_->buttons();
    for(int i = 0; i != buttons.size(); ++i)
    {
        buttons[i]->setCheckable(true);
        connect(buttons.at(i),
                SIGNAL(clicked()),
                this,
                SLOT(escapeBoxChange()));
    }
    buttongroup_->setExclusive(true);
}

void ICStructDefineFrame::on_adjUse_toggled(bool checked)
{
    ICParametersSave::Instance()->SetAdjustFunction(checked);
}
