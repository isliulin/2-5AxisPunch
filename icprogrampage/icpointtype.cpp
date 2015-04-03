#include "icpointtype.h"
#include "ui_icpointtype.h"

ICPointType::ICPointType(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ICPointType)
{
    ui->setupUi(this);
    _box = ui->getWaitBox;

    typeToStr.insert(Get_Wait,tr("Get_Wait"));
    typeToStr.insert(Get_Up,tr("Get_Up"));
    typeToStr.insert(Get,tr("Get"));
    typeToStr.insert(Get_Finish,tr("Get_Finish"));
    typeToStr.insert(Put_Wait,tr("Put_Wait"));
    typeToStr.insert(Put_Up,tr("Put_Up"));
    typeToStr.insert(Put,tr("Put"));
    typeToStr.insert(Put_Finish,tr("Put_Finish"));
    typeToStr.insert(Reserve,tr("Reserve"));

    boxToType.insert(ui->getWaitBox,Get_Wait);
    boxToType.insert(ui->getUpBox,Get_Up);
    boxToType.insert(ui->GetBox,Get);
    boxToType.insert(ui->getFinishBOx,Get_Finish);
    boxToType.insert(ui->putWaitBox,Put_Wait);
    boxToType.insert(ui->putUpBox,Put_Up);
    boxToType.insert(ui->putBox,Put);
    boxToType.insert(ui->putFinishbox,Put_Finish);
    boxToType.insert(ui->reserveBox,Reserve);

    foreach(QCheckBox *box,boxToType.keys()){
         box->setText(typeToStr.value(boxToType.value(box)));

         connect(box,SIGNAL(stateChanged(int)),
                 SLOT(stateChanged(int)));
    }

}

ICPointType::~ICPointType()
{
    delete ui;
}

QString ICPointType::toString(PointType type)
{
    if(type == NULL_Type)
        return typeToStr.value(boxToType.value(_box));
    else
        return typeToStr.value(type);
}


PointType ICPointType::currentType()
{
    return boxToType.value(_box,NULL_Type);
}

void ICPointType::stateChanged(int status)
{
    if(status){
        _box = qobject_cast<QCheckBox*> (sender());
    }
}
