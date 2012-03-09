#include "ichcdetectionframe.h"
#include "ui_ichcdetectionframe.h"

#include "ictwoselectioncomboboxwrapper.h"
#include "icmold.h"
#include "icvirtualhost.h"
//#include "icactioncommand.h"

ICHCDetectionFrame::ICHCDetectionFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ICHCDetectionFrame)
{
    ui->setupUi(this);

    ICTwoSelectionComboBoxWrapper* wrapper;
    wrapper = new ICTwoSelectionComboBoxWrapper(ui->detectFixture1ComboBox, ICMold::CheckClip1);
    wrappers_.append(wrapper);
    wrapper = new ICTwoSelectionComboBoxWrapper(ui->detectFixture2ComboBox, ICMold::CheckClip2);
    wrappers_.append(wrapper);
    wrapper = new ICTwoSelectionComboBoxWrapper(ui->detectFixture3ComboBox, ICMold::CheckClip3);
    wrappers_.append(wrapper);
    wrapper = new ICTwoSelectionComboBoxWrapper(ui->detectFixture4ComboBox, ICMold::CheckClip4);
    wrappers_.append(wrapper);
    wrapper = new ICTwoSelectionComboBoxWrapper(ui->detectSucker1ComboBox, ICMold::CheckClip5);
    wrappers_.append(wrapper);
    wrapper = new ICTwoSelectionComboBoxWrapper(ui->detectSucker2ComboBox, ICMold::CheckClip6);
    wrappers_.append(wrapper);
    //    wrapper = new ICTwoSelectionComboBoxWrapper(ui->converyotComboBox, ICMold::CheckClip7);
    //    wrappers_.append(wrapper);
    //    wrapper = new ICTwoSelectionComboBoxWrapper(ui->injectionComboBox, ICMold::CheckClip8);
    //    wrappers_.append(wrapper);
    //    ui->converyotComboBox->hide();
    //    ui->injectionComboBox->hide();
    //    ui->label_13->hide();
    //    ui->label_8->hide();
    ICVirtualHost* host = ICVirtualHost::GlobalVirtualHost();
    ui->detectPressureComboBox->setCurrentIndex(qAbs(host->IsPressureCheck() - 1));
    ui->detectSecurityComboBox->setCurrentIndex(qAbs(host->IsSecurityCheck() - 1));
    ui->detectMidMoldComboBox->setCurrentIndex(qAbs(host->IsMidMoldCheck() - 1));
    ui->ejectionLinkLockBox->setCurrentIndex(qAbs(host->IsEjectionLink() - 1));
    ui->detectOriginBox->setCurrentIndex(qAbs(host->IsAlarmWhenOrigin() - 1));
    //    ui->detectPositionBox->setCurrentIndex(qAbs(host->IsPositionDetect() - 1));
    ui->detectPositionBox->setCurrentIndex(host->TranserferPosition());
    ui->originPositionBox->setCurrentIndex(host->OriginPosition());
    ui->escapeComboBox->setCurrentIndex(host->EscapeWay());

    connect(ICMold::CurrentMold(),
            SIGNAL(MoldNumberParamChanged()),
            this,
            SLOT(OnMoldNumberParamChanged()));
}

ICHCDetectionFrame::~ICHCDetectionFrame()
{
    delete ui;
    qDeleteAll(wrappers_);
}

void ICHCDetectionFrame::hideEvent(QHideEvent *e)
{
    if(ICVirtualHost::GlobalVirtualHost()->IsParamChanged())
    {
        qDebug("Detection hide");
        ICMold::CurrentMold()->SaveMoldParamsFile();
        ICVirtualHost::GlobalVirtualHost()->SaveSystemConfig();
        ICVirtualHost::GlobalVirtualHost()->ReConfigure();
    }
    //    ICCommandProcessor::Instance()->ExecuteHCCommand(IC::CMD_TurnStop, 0);
    QFrame::hideEvent(e);
}

void ICHCDetectionFrame::changeEvent(QEvent *e)
{
    QFrame::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
    {
        ui->retranslateUi(this);
    }
    break;
    default:
        break;
    }
}
void ICHCDetectionFrame::on_detectPressureComboBox_activated(int index)
{
    bool isCheck = qAbs(index - 1);
    ICVirtualHost::GlobalVirtualHost()->SetPressureCheck(isCheck);
}

void ICHCDetectionFrame::on_detectSecurityComboBox_activated(int index)
{
    bool isCheck = qAbs(index - 1);
    ICVirtualHost::GlobalVirtualHost()->SetSecurityCheck(isCheck);
}

void ICHCDetectionFrame::on_detectMidMoldComboBox_activated(int index)
{
    bool isCheck = qAbs(index - 1);
    ICVirtualHost::GlobalVirtualHost()->SetMidMoldCheck(isCheck);
}

void ICHCDetectionFrame::OnMoldNumberParamChanged()
{
    for(int i = 0; i != wrappers_.size(); ++i)
    {
        wrappers_[i]->UpdateParameter();
    }
}

//void ICHCDetectionFrame::on_closeMoldStatusComboBox_activated(int index)
//{
//    bool isCheck = qAbs(index - 1);
//    ICVirtualHost::GlobalVirtualHost()->SetCloseMoldPermit(isCheck);
//}

void ICHCDetectionFrame::on_ejectionLinkLockBox_activated(int index)
{
    bool isCheck = qAbs(index - 1);
    ICVirtualHost::GlobalVirtualHost()->SetEjectionLink(isCheck);
}

void ICHCDetectionFrame::on_detectPositionBox_activated(int index)
{
    //    bool isCheck = qAbs(index - 1);
    //    ICVirtualHost::GlobalVirtualHost()->SetPositionDetect(isCheck);
    ICVirtualHost::GlobalVirtualHost()->SetTranserferPosition(index);
}

void ICHCDetectionFrame::on_detectOriginBox_activated(int index)
{
    bool isCheck = qAbs(index - 1);
    ICVirtualHost::GlobalVirtualHost()->SetAlarmWhenOrigin(isCheck);
}

void ICHCDetectionFrame::on_originPositionBox_activated(int index)
{
    ICVirtualHost::GlobalVirtualHost()->SetOriginPosition(index);
}

void ICHCDetectionFrame::on_escapeComboBox_activated(int index)
{
    ICVirtualHost::GlobalVirtualHost()->SetEscapeWay(index);
}
