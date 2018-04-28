#include "ContactChooseWidget.h"
#include "ui_ContactChooseWidget.h"

#include <QPushButton>
#include <QHBoxLayout>
#include "contact/ContactDataUtil.h"
#include "wallet.h"
#include "commondialog.h"

Q_DECLARE_METATYPE(std::shared_ptr<ContactGroup>);
Q_DECLARE_METATYPE(std::shared_ptr<ContactPerson>);

class ContactChooseWidget::ContactChooseWidgetPrivate
{
public:
    ContactChooseWidgetPrivate()
        :contactSheet(nullptr)
        ,contactFilePath("")
    {
        if( UBChain::getInstance()->configFile->contains("/settings/chainPath"))
        {
            contactFilePath = UBChain::getInstance()->configFile->value("/settings/chainPath").toString() + "/contacts.dat";
        }
        else
        {
            contactFilePath = UBChain::getInstance()->appDataPath + "/contacts.dat";
        }
    }
public:
    std::shared_ptr<ContactSheet> contactSheet;//数据源
    QString contactFilePath;

    QString contactName;
    QString contactAddress;
};

ContactChooseWidget::ContactChooseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContactChooseWidget),
    _p(new ContactChooseWidgetPrivate())
{
    ui->setupUi(this);
    InitWidget();
}

ContactChooseWidget::~ContactChooseWidget()
{
    delete _p;
    delete ui;
}

void ContactChooseWidget::itemClickedSlots(QTreeWidgetItem *item)
{
    if(!item) return;
    std::shared_ptr<ContactPerson> person = item->data(0,Qt::UserRole).value<std::shared_ptr<ContactPerson>>();
    if(!person) return;

    _p->contactName = person->name;
    _p->contactAddress = person->address;
    updateUI();
}

void ContactChooseWidget::confirmSlots()
{
    emit selectContactSignal(_p->contactName,_p->contactAddress);
    emit closeSignal();
    close();
}

void ContactChooseWidget::updateUI()
{
    ui->name->setText(_p->contactName);
    ui->address->setText(_p->contactAddress);
}

void ContactChooseWidget::InitWidget()
{
    InitStyle();
    InitData();

    QPushButton *pSearchButton = new QPushButton(this);

    pSearchButton->setCursor(Qt::PointingHandCursor);
    pSearchButton->setFixedSize(20, 20);
    pSearchButton->setToolTip(tr("Search"));
    pSearchButton->setIconSize(QSize(18,18));
    pSearchButton->setIcon(QIcon(":/search.png"));

    //防止文本框输入内容位于按钮之下
    QMargins margins = ui->lineEdit->textMargins();
    ui->lineEdit->setTextMargins(10+pSearchButton->width(), margins.top(), margins.right(), margins.bottom());

    QHBoxLayout *pSearchLayout = new QHBoxLayout();
    pSearchLayout->addStretch();
    pSearchLayout->addWidget(pSearchButton);
    pSearchLayout->setSpacing(0);
    pSearchLayout->setDirection(QBoxLayout::RightToLeft);
    pSearchLayout->setContentsMargins(10, 0, 0, 0);
    ui->lineEdit->setLayout(pSearchLayout);


    connect(ui->treeWidget,&QTreeWidget::itemClicked,this,&ContactChooseWidget::itemClickedSlots);
    connect(ui->toolButton_cancel,&QToolButton::clicked,this,&ContactChooseWidget::closeSignal);
    connect(ui->toolButton_close,&QToolButton::clicked,this,&ContactChooseWidget::closeSignal);
    connect(ui->toolButton_cancel,&QToolButton::clicked,this,&ContactChooseWidget::close);
    connect(ui->toolButton_close,&QToolButton::clicked,this,&ContactChooseWidget::close);
    connect(ui->toolButton_confirm,&QToolButton::clicked,this,&ContactChooseWidget::confirmSlots);
}

void ContactChooseWidget::InitStyle()
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(248,249,253));
    setPalette(palette);

    ui->treeWidget->setHeaderHidden(true);
    ui->treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->treeWidget->setStyleSheet(
                  "QTreeView{background-color:white;border:none;\
                      color:black;font-size:16px;outline:0px;margin:0 0 0 20px;}"
                  "QTreeView::branch{background:white;}"
                  "QTreeView::item{height:20px;}"
                  "QTreeView::item::selected{background:#829DFF;border:none;color:white;}"
                  "QTreeView::branch::selected::adjoins-item,QTreeView::branch::selected::!adjoins-item{background:#829DFF;}"
                  "QTreeView::branch::hover::adjoins-item,QTreeView::branch::hover::!adjoins-item,QTreeView::item::hover{background:#829DFF;}"
                 );

    ui->toolButton_close->setIconSize(QSize(12,12));
    ui->toolButton_close->setIcon(QIcon(":/ui/wallet_ui/close.png"));
    ui->toolButton_close->setStyleSheet("QToolButton{background-color:transparent;border:none;}"
                            "QToolButton:hover{background-color:rgb(208,228,255);}");

    ui->toolButton_cancel->setStyleSheet("QToolButton{background-color:#E5E5E5;border:none;border-radius:15px;color: rgb(255, 255, 255);}"
                       "QToolButton:hover{background-color:#00D2FF;}");
    ui->toolButton_confirm->setStyleSheet("QToolButton{background-color:#5474EB;border:none;border-radius:15px;color: rgb(255, 255, 255);}"
                   "QToolButton:hover{background-color:#00D2FF;}");

    setStyleSheet("QWidget#ContactChooseWidget{border-radius:15px;}"
                  "QPushButton{ background:transparent;}"
                  "QLineEdit{background-color:transparent;border: 1px solid #999999 ;border-radius:15px;color:#5474EB;}");
}

void ContactChooseWidget::InitData()
{
    if(!ContactDataUtil::readContactSheetFromPath(_p->contactFilePath,_p->contactSheet))
    {//联系人文件解析失败，提示用户没有联系人
        close();
        CommonDialog dia(CommonDialog::OkOnly);
        dia.setText(tr("Contact file doesn't exist or damaged!"));
        dia.pop();
        return;
    }
    if(!_p->contactSheet) return;
//创建联系人树
    ui->treeWidget->clear();
    foreach (std::shared_ptr<ContactGroup> group, _p->contactSheet->groups) {
        QTreeWidgetItem *groupItem = new QTreeWidgetItem(QStringList()<<group->groupName);
        groupItem->setToolTip(0,group->groupName);
        groupItem->setData(0,Qt::UserRole,QVariant::fromValue<std::shared_ptr<ContactGroup>>(group));
        ui->treeWidget->addTopLevelItem(groupItem);
        foreach(std::shared_ptr<ContactPerson> person,group->groupPeople){
            QTreeWidgetItem *personItem = new QTreeWidgetItem(QStringList()<<person->name);
            personItem->setToolTip(0,person->name);
            personItem->setData(0,Qt::UserRole,QVariant::fromValue<std::shared_ptr<ContactPerson>>(person));
            groupItem->addChild(personItem);
        }
    }
    ui->treeWidget->expandAll();
}

void ContactChooseWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    painter.setBrush(QBrush(QColor(248,249,253)));
    painter.setPen(Qt::transparent);

    painter.drawRoundedRect(rect(), 25, 25);
    //QWidget::paintEvent(event);
}