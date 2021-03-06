#include "actionsettingframe.h"
#include "ui_actionsettingframe.h"

#include <qmath.h>
#include "routesettingdialog.h"

#include "iccommandprocessor.h"
#include "icvirtualkey.h"
#include "icvirtualhost.h"
#include "ickeyboard.h"

#include "icparameterssave.h"
#include "icsystemconfig.h"
#include "icaxiskeyboard.h"

ActionSettingFrame::ActionSettingFrame(QWidget *parent) :
    ICInstructionEditorBase(parent),
    ui(new Ui::ActionSettingFrame),
    axisDefine_(-1)
{
    ui->setupUi(this);
#ifdef HC_AXIS_COUNT_2
    ui->gzButton->hide();
    ui->zPosLineEdit->hide();
    ui->label_12->hide();
    ui->gtButton->hide();
    ui->tPosLineEdit->hide();
    ui->label_13->hide();
#endif
    QStringList points;
    ICUserDefineConfigSPTR config = ICUserDefineConfig::Instance();
    for(int i = 0; i != 6; ++i)
    {
        points.append(config->GetPointsLocaleName(i, "zh"));
    }
    ui->pointSel->addItems(points);

    for(int i = 0; i != 8; ++i)
    {
        posValidators_[i].setBottom(0);
    }
    for(int i = 0; i != 3; ++i)
    {
        posValidator[i].setBottom(-50);
    }
    InitInterface();
    axisWidgets_.append(QList<QWidget*>()<<ui->gxButton<<ui->x1PosLineEdit);
    axisWidgets_.append(QList<QWidget*>()<<ui->gyButton<<ui->y1PosLineEdit);
#ifdef HC_AXIS_COUNT_5
    axisWidgets_.append(QList<QWidget*>()<<ui->gzButton<<ui->zPosLineEdit);
    axisWidgets_.append(QList<QWidget*>()<<ui->gPButton<<ui->pPosLineEdit);
    axisWidgets_.append(QList<QWidget*>()<<ui->gtButton<<ui->tPosLineEdit);
#endif

#ifndef HC_SK_8_SC
#ifndef HC_SK_5
    ui->axisBoard->hide();
#endif
#endif

#ifdef Q_WS_X11
    UpdateAxisDefine_();
#endif
    //    ui->x1SpeedLineEdit->hide();
//    ui->curveBox->hide();
}

ActionSettingFrame::~ActionSettingFrame()
{
    delete ui;
}

void ActionSettingFrame::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ActionSettingFrame::InitInterface()
{     /*******BUG#119**********/
    QIntValidator * validator = new QIntValidator(0, 30000, this);
    ui->x1PosLineEdit->SetDecimalPlaces(POS_DECIMAL);
    ui->x1PosLineEdit->setValidator(posValidators_ + 0);
    //    ui->x1PosLineEdit->setValidator(posvalidator);

    ui->y1PosLineEdit->SetDecimalPlaces(POS_DECIMAL);
    //    ui->y1PosLineEdit->setValidator(posvalidator);
    ui->y1PosLineEdit->setValidator(posValidators_ + 1);
#ifdef HC_AXIS_COUNT_5
    ui->zPosLineEdit->SetDecimalPlaces(POS_DECIMAL);
    //    ui->zPosLineEdit->setValidator(posvalidator);
    ui->zPosLineEdit->setValidator(posValidators_ + 2);
    ui->tPosLineEdit->SetDecimalPlaces(POS_DECIMAL);
    //    ui->zPosLineEdit->setValidator(posvalidator);
    ui->tPosLineEdit->setValidator(posValidators_ + 4);
    ui->pPosLineEdit->SetDecimalPlaces(POS_DECIMAL);
    ui->pPosLineEdit->setValidator(posValidators_ + 3);
#endif

    QIntValidator *speed = new QIntValidator(0, 100, this);
    ui->speedEdit->SetDecimalPlaces(0);
    ui->speedEdit->setValidator(speed);
    ui->delayEdit->SetDecimalPlaces(2);
    ui->delayEdit->setValidator(validator);
#ifdef HC_8AXIS

#endif


}

void ActionSettingFrame::on_inputButton_clicked()
{
    //    if(ICVirtualHost::GlobalVirtualHost()->IsOutputOn(6))
    //    {
    //        ui->gxComboBox->setCurrentIndex(0);
    //    }
    //    else
    //    {
    //        ui->gxComboBox->setCurrentIndex(1);
    //    }
    ui->x1PosLineEdit->setText(QString::number(oXP_ / 100.0, 'f', 2));
    ui->y1PosLineEdit->setText(QString::number(oYP_ / 100.0, 'f', 2));
#ifdef HC_AXIS_COUNT_5
    ui->zPosLineEdit->setText(QString::number(oZP_ / 100.0, 'f', 2));
    ui->tPosLineEdit->setText(QString::number(oTP_ / 100.0, 'f', 2));
    ui->pPosLineEdit->setText(QString::number(oPP_ / 100.0, 'f', 2));

#endif

    //do someting
}

void ActionSettingFrame::hideEvent(QHideEvent *e)
{
    ui->gxButton->setChecked(false);
    ui->gyButton->setChecked(false);
#ifdef HC_AXIS_COUNT_5
    ui->gzButton->setChecked(false);
    ui->gtButton->setChecked(false);
    ui->gPButton->setChecked(false);
#endif
    QFrame::hideEvent(e);
    disconnect(ICVirtualHost::GlobalVirtualHost(),
               SIGNAL(StatusRefreshed()),
               this,
               SLOT(StatusRefresh()));
}

void ActionSettingFrame::showEvent(QShowEvent *e)
{
    UpdateAxisDefine_();
    ICVirtualHost* host = ICVirtualHost::GlobalVirtualHost();
    if(host->AxisDefine(ICVirtualHost::ICAxis_AxisX1) == ICVirtualHost::ICAxisDefine_Servo)
    {
        ui->gxButton->show();ui->x1PosLineEdit->show();ui->label_11->show();
    }
    else
    {
        ui->gxButton->hide();ui->x1PosLineEdit->hide();ui->label_11->hide();
    }
    if(host->AxisDefine(ICVirtualHost::ICAxis_AxisY1) == ICVirtualHost::ICAxisDefine_Servo)
    {
        ui->gyButton->show();ui->y1PosLineEdit->show();ui->label_10->show();
    }
    else
    {
        ui->gyButton->hide();ui->y1PosLineEdit->hide();ui->label_10->hide();
    }
    if(host->AxisDefine(ICVirtualHost::ICAxis_AxisZ) == ICVirtualHost::ICAxisDefine_Servo)
    {
        ui->gzButton->show();ui->zPosLineEdit->show();ui->label_12->show();
    }
    else
    {
        ui->gzButton->hide();ui->zPosLineEdit->hide();ui->label_12->hide();
    }
    if(host->AxisDefine(ICVirtualHost::ICAxis_AxisX2) == ICVirtualHost::ICAxisDefine_Servo)
    {
        ui->gPButton->show();ui->pPosLineEdit->show();ui->label_14->show();
    }
    else
    {
        ui->gPButton->hide();ui->pPosLineEdit->hide();ui->label_14->hide();
    }
    if(host->AxisDefine(ICVirtualHost::ICAxis_AxisY2) == ICVirtualHost::ICAxisDefine_Servo)
    {
        ui->gtButton->show();ui->tPosLineEdit->show();ui->label_13->show();
    }
    else
    {
        ui->gtButton->hide();ui->tPosLineEdit->hide();ui->label_13->hide();
    }

    posMaxs_[0] = (qint16)host->SystemParameter(ICVirtualHost::SYS_X_Length).toInt();
    posMaxs_[1] = (qint16)host->SystemParameter(ICVirtualHost::SYS_Y_Length).toInt();
    posMaxs_[2] = (qint16)host->SystemParameter(ICVirtualHost::SYS_Z_Length).toInt();
    posMaxs_[3] = (qint16)host->SystemParameter(ICVirtualHost::SYS_P_Length).toInt();
    posMaxs_[4] = (qint16)host->SystemParameter(ICVirtualHost::SYS_Q_Length).toInt();
    posMaxs_[5] = (qint16)host->SystemParameter(ICVirtualHost::SYS_A_Length).toInt();
    posMaxs_[6] = (qint16)host->SystemParameter(ICVirtualHost::SYS_B_Length).toInt();
    posMaxs_[7] = (qint16)host->SystemParameter(ICVirtualHost::SYS_C_Length).toInt();

    posMins_[0] = (qint16)host->SystemParameter(ICVirtualHost::SYS_X_Maxium).toInt();
    posMins_[1] = (qint16)host->SystemParameter(ICVirtualHost::SYS_Y_Maxium).toInt();
    posMins_[2] = (qint16)host->SystemParameter(ICVirtualHost::SYS_Z_Maxium).toInt();
    posMins_[3] = (qint16)host->SystemParameter(ICVirtualHost::SYS_P_Maxium).toInt();
    posMins_[4] = (qint16)host->SystemParameter(ICVirtualHost::SYS_Q_Maxium).toInt();
    posMins_[5] = (qint16)host->SystemParameter(ICVirtualHost::SYS_A_Maxium).toInt();
    posMins_[6] = (qint16)host->SystemParameter(ICVirtualHost::SYS_B_Maxium).toInt();
    posMins_[7] = (qint16)host->SystemParameter(ICVirtualHost::SYS_C_Maxium).toInt();
    posLength_[0] = (qint16)host->SystemParameter(ICVirtualHost::SYS_A_Length).toInt();
    posLength_[1] = (qint16)host->SystemParameter(ICVirtualHost::SYS_B_Length).toInt();
    posLength_[2] = (qint16)host->SystemParameter(ICVirtualHost::SYS_C_Length).toInt();

    int mutil = qPow(10, SECTION_DECIMAL);
    for(int i = 0; i != 8; ++i)
    {
        posValidators_[i].setTop(posMaxs_[i] * mutil);
        posValidators_[i].setBottom(posMins_[i] * mutil);
    }

    QFrame::showEvent(e);
    connect(ICVirtualHost::GlobalVirtualHost(),
            SIGNAL(StatusRefreshed()),
            this,
            SLOT(StatusRefresh()));
}

void ActionSettingFrame::SyncStatusImpl(const QList<ICMoldItem> &items)
{
    Q_UNUSED(items)
}
quint32 ActionSettingFrame::GetPointValue(quint16 pos)
{
    ICVirtualHost* host = ICVirtualHost::GlobalVirtualHost();
    quint32 s  = host->HostStatus(ICVirtualHost::DbgB0).toUInt() << 16;
    s = s + host->HostStatus(ICVirtualHost::DbgA1).toUInt();

    return ( (s >>( (pos -  ICVirtualHost::XPos)* 4 ) )& 0xF);
}

qint32 ActionSettingFrame::GetPosValue(ICVirtualHost::ICStatus status)
{
    ICVirtualHost* host = ICVirtualHost::GlobalVirtualHost();
    qint16  p =  host->HostStatus(status).toInt() ;
    if(p < 0){
        qint32 v = p * 10 -  GetPointValue(status) ;
        return v;
    }
    else{
        qint32 v = p * 10 + GetPointValue(status) ;
        return v;
    }
}


void ActionSettingFrame::StatusRefresh()
{
     int pos = GetPosValue(ICVirtualHost::YPos);
    if(pos != oYP_)
    {
        oYP_ = pos;
    }
    pos = GetPosValue(ICVirtualHost::ZPos);
    if(pos != oZP_)
    {
        oZP_ = pos;
    }

    pos = GetPosValue(ICVirtualHost::XPos);
    if(pos != oXP_)
    {
        oXP_ = pos;
    }

#ifdef HC_AXIS_COUNT_5
    pos = GetPosValue(ICVirtualHost::PPos);
    if(pos != oPP_)
    {
        oPP_ = pos;
    }
    pos = GetPosValue(ICVirtualHost::QPos);
    if(pos != oTP_)
    {
        oTP_ = pos;
    }
#endif
}

QList<ICMoldItem> ActionSettingFrame::CreateCommandImpl() const
{
    QList<ICMoldItem> ret;
    ICMoldItem item;
    item.SetBadProduct(ui->relBox->isChecked());
    item.SetSubNum(ui->pointSel->currentIndex());
    if(!ui->curveBox->isChecked())
    {

        if(ui->gxButton->isChecked() && (!ui->gxButton->isHidden()))
        {
            //        if(ui->gxComboBox->currentIndex() == 0)
            //        {
            //            item.SetAction(ICMold::ACTMAINFORWARD);
            //        }
            //        else
            //        {
            //            item.SetAction(ICMold::ACTMAINBACKWARD);
            //        }

            item.SetAction(ICMold::GX);
            item.SetActualPos(ui->x1PosLineEdit->TransThisTextToThisInt());
            item.SetSVal(ui->speedEdit->TransThisTextToThisInt());
            item.SetDVal(ui->delayEdit->TransThisTextToThisInt());
            //        item.SetIFVal(0);
//            item.SetActualIfPos(0);
            ret.append(item);
        }
        if(ui->gyButton->isChecked() && (!ui->gyButton->isHidden()))
        {
            item.SetAction(ICMold::GY);
            item.SetActualPos(ui->y1PosLineEdit->TransThisTextToThisInt());
            item.SetSVal(ui->speedEdit->TransThisTextToThisInt());
            item.SetDVal(ui->delayEdit->TransThisTextToThisInt());
            //        item.SetIFVal(0);
//            item.SetActualIfPos(0);
            ret.append(item);
        }
#ifdef HC_AXIS_COUNT_5
        if(ui->gzButton->isChecked() && (!ui->gzButton->isHidden()))
        {
            item.SetAction(ICMold::GZ);
            item.SetActualPos(ui->zPosLineEdit->TransThisTextToThisInt());
            item.SetSVal(ui->speedEdit->TransThisTextToThisInt());
            item.SetDVal(ui->delayEdit->TransThisTextToThisInt());
            //        item.SetIFVal(0);
//            item.SetActualIfPos(0);
            ret.append(item);
        }
        if(ui->gPButton->isChecked() && (!ui->gPButton->isHidden()))
        {
            item.SetAction(ICMold::GP);
            item.SetActualPos(ui->pPosLineEdit->TransThisTextToThisInt());
            item.SetSVal(ui->speedEdit->TransThisTextToThisInt());
            item.SetDVal(ui->delayEdit->TransThisTextToThisInt());
            //        item.SetIFVal(0);
//            item.SetActualIfPos(0);
            ret.append(item);
        }
        if(ui->gtButton->isChecked() && (!ui->gtButton->isHidden()))
        {
            item.SetAction(ICMold::GQ);
            item.SetActualPos(ui->tPosLineEdit->TransThisTextToThisInt());
            item.SetSVal(ui->speedEdit->TransThisTextToThisInt());
            item.SetDVal(ui->delayEdit->TransThisTextToThisInt());
            //        item.SetIFVal(0);
//            item.SetActualIfPos(0);
            ret.append(item);
        }

#endif
    }
    else
    {
        if(ui->gxButton->isChecked() && (!ui->gxButton->isHidden()))
        {
            //        if(ui->gxComboBox->currentIndex() == 0)
            //        {
            //            item.SetAction(ICMold::ACTMAINFORWARD);
            //        }
            //        else
            //        {
            //            item.SetAction(ICMold::ACTMAINBACKWARD);
            //        }

            item.SetAction(ICMold::GARC);
            item.SetIFPos(ICMold::GX);
//            item.SetIFVal(ui->arcBox->isChecked()?  2 : 1);
            item.SetActualPos(ui->x1PosLineEdit->TransThisTextToThisInt());
            item.SetSVal(ui->speedEdit->TransThisTextToThisInt());
            item.SetDVal(ui->delayEdit->TransThisTextToThisInt());
            //        item.SetIFVal(0);
//            item.SetActualIfPos(0);
            ret.append(item);
        }
        if(ui->gyButton->isChecked() && (!ui->gyButton->isHidden()))
        {
            item.SetAction(ICMold::GARC);
            item.SetIFPos(ICMold::GY);
//            item.SetIFVal(ui->arcBox->isChecked()?  2 : 1);
            item.SetActualPos(ui->y1PosLineEdit->TransThisTextToThisInt());
            item.SetSVal(ui->speedEdit->TransThisTextToThisInt());
            item.SetDVal(ui->delayEdit->TransThisTextToThisInt());
            //        item.SetIFVal(0);
//            item.SetActualIfPos(0);
            ret.append(item);
        }
#ifdef HC_AXIS_COUNT_5
        if(ui->gzButton->isChecked() && (!ui->gzButton->isHidden()))
        {
            item.SetAction(ICMold::GARC);
            item.SetIFPos(ICMold::GZ);
//            item.SetIFVal(ui->arcBox->isChecked()?  2 : 1);
            item.SetActualPos(ui->zPosLineEdit->TransThisTextToThisInt());
            item.SetSVal(ui->speedEdit->TransThisTextToThisInt());
            item.SetDVal(ui->delayEdit->TransThisTextToThisInt());
            //        item.SetIFVal(0);
//            item.SetActualIfPos(0);
            ret.append(item);
        }
        if(ui->gPButton->isChecked() && (!ui->gPButton->isHidden()))
        {
            item.SetAction(ICMold::GARC);
            item.SetIFPos(ICMold::GP);
//            item.SetIFVal(ui->arcBox->isChecked()? 2 : 1);
            item.SetActualPos(ui->pPosLineEdit->TransThisTextToThisInt());
            item.SetSVal(ui->speedEdit->TransThisTextToThisInt());
            item.SetDVal(ui->delayEdit->TransThisTextToThisInt());
            //        item.SetIFVal(0);
//            item.SetActualIfPos(0);
            ret.append(item);
        }
        if(ui->gtButton->isChecked() && (!ui->gtButton->isHidden()))
        {
            item.SetAction(ICMold::GARC);
            item.SetIFPos(ICMold::GQ);
//            item.SetIFVal(ui->arcBox->isChecked()?  2 : 1);
            item.SetActualPos(ui->tPosLineEdit->TransThisTextToThisInt());
            item.SetSVal(ui->speedEdit->TransThisTextToThisInt());
            item.SetDVal(ui->delayEdit->TransThisTextToThisInt());
            //        item.SetIFVal(0);
//            item.SetActualIfPos(0);
            ret.append(item);
        }
#endif
    }
    return ret;
}

void ActionSettingFrame::on_gxButton_toggled(bool checked)
{
    if(checked && ui->absBox->isChecked())
    {
        //        ui->gxComboBox->setEnabled(true);
        ui->x1PosLineEdit->setEnabled(true);
        //        ui->delayEdit->setEnabled(true);
        //        ui->speedEdit->setEnabled(true);
    }
    else
    {
        //        ui->gxComboBox->setEnabled(false);
        ui->x1PosLineEdit->setEnabled(false);
        //        ui->delayEdit->setEnabled(false);
        //        ui->x1SpeedLineEdit->setEnabled(false);
        //        ui->x1ForwardBox->setEnabled(false);
        //        ui->x1BackwardBox->setEnabled(false);
    }
}

void ActionSettingFrame::on_gyButton_toggled(bool checked)
{
    if(checked && ui->absBox->isChecked())
    {
        ui->y1PosLineEdit->setEnabled(true);
        //        ui->y1DelayLineEdit->setEnabled(true);
        //        ui->y1SpeedLineEdit->setEnabled(true);
        //        ui->y1DownBox->setEnabled(true);
        //        ui->y1UpBox->setEnabled(true);
    }
    else
    {
        ui->y1PosLineEdit->setEnabled(false);
        //        ui->y1DelayLineEdit->setEnabled(false);
        //        ui->y1SpeedLineEdit->setEnabled(false);
        //        ui->y1DownBox->setEnabled(false);
        //        ui->y1UpBox->setEnabled(false);
    }
}

#ifdef HC_AXIS_COUNT_5
void ActionSettingFrame::on_gzButton_toggled(bool checked)
{
    if(checked && ui->absBox->isChecked())
    {
        ui->zPosLineEdit->setEnabled(true);
        //        ui->zDelayLineEdit->setEnabled(true);
        //        ui->zSpeedLineEdit->setEnabled(true);
        //        ui->zComeInBox->setEnabled(true);
        //        ui->zComeOutBox->setEnabled(true);
    }
    else
    {
        ui->zPosLineEdit->setEnabled(false);
        //        ui->zDelayLineEdit->setEnabled(false);
        //        ui->zSpeedLineEdit->setEnabled(false);
        //        ui->zComeInBox->setEnabled(false);
        //        ui->zComeOutBox->setEnabled(false);
    }
}

void ActionSettingFrame::on_gtButton_toggled(bool checked)
{
    if(checked && ui->absBox->isChecked())
    {
        ui->tPosLineEdit->setEnabled(true);
    }
    else
    {
        ui->tPosLineEdit->setEnabled(false);
    }
}

void ActionSettingFrame::on_gPButton_toggled(bool checked)
{
    if(checked && ui->absBox->isChecked())
    {
        ui->pPosLineEdit->setEnabled(true);
    }
    else
    {
        ui->pPosLineEdit->setEnabled(false);
    }
}


#endif

void ActionSettingFrame::ShowWidgets_(QList<QWidget *> &widgets)
{
    for(int i = 0; i != widgets.size(); ++i)
    {
        widgets[i]->show();;
    }
}

void ActionSettingFrame::HideWidgets_(QList<QWidget *> &widgets)
{
    for(int i = 0; i != widgets.size(); ++i)
    {
        widgets[i]->hide();;
    }
}

void ActionSettingFrame::UpdateAxisDefine_()
{
#ifdef HC_AXIS_COUNT_5
    ICVirtualHost* host = ICVirtualHost::GlobalVirtualHost();
    bool isAxisX2Visable = (host->AxisDefine(ICVirtualHost::ICAxis_AxisX2) != ICVirtualHost::ICAxisDefine_None);
    ui->gPButton->setVisible(isAxisX2Visable);
    ui->pPosLineEdit->setVisible(isAxisX2Visable);
    ui->label_13->setVisible(isAxisX2Visable);
#endif
}

void ActionSettingFrame::KeyToActionCheck(int key)
{
    switch(key)
    {
    case ICKeyboard::VFB_X1Add:
    case ICKeyboard::VFB_X1Sub:
        ui->gxButton->setChecked(true);
        break;
    case ICKeyboard::VFB_Y1Add:
    case ICKeyboard::VFB_Y1Sub:
        ui->gyButton->setChecked(true);
        break;
    case ICKeyboard::VFB_ZAdd:
    case ICKeyboard::VFB_ZSub:
        ui->gzButton->setChecked(true);
        break;
    case ICKeyboard::VFB_X2Add:
    case ICKeyboard::VFB_X2Sub:
        break;
    case ICKeyboard::VFB_Y2Add:
    case ICKeyboard::VFB_Y2Sub:
        break;
    case ICKeyboard::VFB_AAdd:
    case ICKeyboard::VFB_ASub:
        break;
    case ICKeyboard::VFB_BAdd:
    case ICKeyboard::VFB_BSub:
        break;
    case ICKeyboard::VFB_CAdd:
    case ICKeyboard::VFB_CSub:
        break;
    case ICKeyboard::VFB_Pose_Horizontal:
    case ICKeyboard::VFB_Pose_Vertical:
        break;

    }
}

void ActionSettingFrame::on_absBox_toggled(bool checked)
{
    ui->x1PosLineEdit->setEnabled(checked && ui->gxButton->isChecked());
    ui->y1PosLineEdit->setEnabled(checked && ui->gyButton->isChecked());
#ifdef HC_AXIS_COUNT_5
    ui->zPosLineEdit->setEnabled(checked && ui->gzButton->isChecked());
    ui->tPosLineEdit->setEnabled(checked && ui->gtButton->isChecked());
    ui->pPosLineEdit->setEnabled(checked && ui->gtButton->isChecked());
#endif
    ui->pointSel->setEnabled(!checked);
}

#ifdef HC_SK_8_SC
void ActionSettingFrame::on_axisBoard_clicked()
{
    ICAxisKeyboard::Instance()->show();
}
#endif


//void ActionSettingFrame::on_arcBox_clicked(bool checked)
//{
//    if(checked)
//    {
//        if(ui->slashBox->isChecked())
//        {
//            ui->slashBox->setChecked(false);
//        }
//    }

//}

//void ActionSettingFrame::on_slashBox_clicked(bool checked)
//{
//    if(checked)
//    {
//        if(ui->arcBox->isChecked())
//        {
//            ui->arcBox->setChecked(false);
//        }
//    }
//}

void ActionSettingFrame::on_serveControl_toggled(bool checked)
{
    if(checked)
    {
        ui->serveControl->setText(tr("Servo OFF"));
        ICCommandProcessor::Instance()->ExecuteVirtualKeyCommand(IC::VKEY_SERVO_OFF);
    }
    else
    {
        ui->serveControl->setText(tr("Servo On"));
        ICCommandProcessor::Instance()->ExecuteVirtualKeyCommand(IC::VKEY_SERVO_ON);
    }
}

void ActionSettingFrame::ResetServo()
{
    ui->serveControl->setChecked(false);
}
