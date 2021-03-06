#include "ContactSearchWidget.h"
#include "ui_ContactSearchWidget.h"

#include <QPushButton>
#include <QHBoxLayout>

class ContactSearchWidget::ContactSearchWidgetPrivate
{
public:
    ContactSearchWidgetPrivate()
    {

    }
public:

};

ContactSearchWidget::ContactSearchWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContactSearchWidget),
    _p(new ContactSearchWidgetPrivate())
{
    ui->setupUi(this);
    InitWidget();
}

ContactSearchWidget::~ContactSearchWidget()
{
    delete _p;
    delete ui;
}

void ContactSearchWidget::StartSearchSlots()
{
    emit searchSignal(ui->lineEdit->text());
}

void ContactSearchWidget::InitWidget()
{
    InitStyle();

    QPushButton *pSearchButton = new QPushButton(this);

    pSearchButton->setCursor(Qt::PointingHandCursor);
    pSearchButton->setFixedSize(14, 14);
    pSearchButton->setToolTip(tr("Search"));
    pSearchButton->setIconSize(QSize(14,14));
    pSearchButton->setIcon(QIcon(":/search.png"));

    //防止文本框输入内容位于按钮之下
    QMargins margins = ui->lineEdit->textMargins();
    ui->lineEdit->setTextMargins(pSearchButton->width()+5, margins.top(),margins.right() , margins.bottom());
    ui->lineEdit->setPlaceholderText(tr("name or addr"));

    QHBoxLayout *pSearchLayout = new QHBoxLayout();
    pSearchLayout->addStretch();
    pSearchLayout->addWidget(pSearchButton);
    pSearchLayout->setSpacing(0);
    pSearchLayout->setDirection(QBoxLayout::RightToLeft);
    pSearchLayout->setContentsMargins(5, 0, 0, 0);
    ui->lineEdit->setLayout(pSearchLayout);

    connect(pSearchButton, &QPushButton::clicked, this, &ContactSearchWidget::StartSearchSlots);

    connect(ui->lineEdit,&QLineEdit::textEdited,this, &ContactSearchWidget::StartSearchSlots);

    ui->label->setVisible(false);
    //ui->lineEdit->setStyleSheet();
}

void ContactSearchWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(94,116,235));
    setPalette(palette);


    setStyleSheet("QPushButton{ background:transparent;}"
                  "QLineEdit{background:rgb(130,157,255);font-size:11px;border:none;color:white;}"
                  );

}
