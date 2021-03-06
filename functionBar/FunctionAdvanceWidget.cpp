#include "FunctionAdvanceWidget.h"
#include "ui_FunctionAdvanceWidget.h"

#include "extra/style.h"

FunctionAdvanceWidget::FunctionAdvanceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FunctionAdvanceWidget)
{
    ui->setupUi(this);
    InitWidget();
}

FunctionAdvanceWidget::~FunctionAdvanceWidget()
{
    delete ui;
}
void FunctionAdvanceWidget::retranslator()
{
    ui->retranslateUi(this);
}
void FunctionAdvanceWidget::DefaultShow()
{
    PoundageShowSlots();
}

void FunctionAdvanceWidget::PoundageShowSlots()
{
    ui->toolButton_poundage->setChecked(true);
    ui->toolButton_multiSig->setChecked(false);
    emit showPoundageSignal();
}

void FunctionAdvanceWidget::MultiSigShowSlots()
{
    ui->toolButton_poundage->setChecked(false);
    ui->toolButton_multiSig->setChecked(true);
    emit showMultiSigSignal();
}

void FunctionAdvanceWidget::InitWidget()
{
    InitStyle();
    ui->toolButton_poundage->setCheckable(true);
    ui->toolButton_multiSig->setCheckable(true);
    connect(ui->toolButton_poundage,&QToolButton::clicked,this,&FunctionAdvanceWidget::PoundageShowSlots);
    connect(ui->toolButton_multiSig,&QToolButton::clicked,this,&FunctionAdvanceWidget::MultiSigShowSlots);
}

void FunctionAdvanceWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(84,116,235));
    setPalette(palette);

    QFont font("\"Microsoft YaHei UI Light\"",14,63);
    ui->label->setFont(font);
    QPalette pa;
    pa.setColor(QPalette::WindowText,QColor(248,249,254));
    ui->label->setPalette(pa);

    //ui->pushButton_multiSig->setFlat(true);
    //ui->pushButton_poundage->setFlat(true);

    setStyleSheet(FUNCTIONBAR_TOOLBUTTON_STYLE);
}
