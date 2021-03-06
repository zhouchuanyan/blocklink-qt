﻿#include "FeeChooseWidget.h"
#include "ui_FeeChooseWidget.h"

#include <algorithm>
#include <mutex>
#include "poundage/PoundageDataUtil.h"
#include "wallet.h"

class FeeChooseWidget::DataPrivate
{
public:
    DataPrivate(double number,const QString &type,const QString &account)
        :feeNumber(number),feeType(type),accountName(account)
    {

    }
public:
    double feeNumber;

    double coinNumber;
    QString feeType;

    QString poundageID;

    QString poundageTip;

    std::mutex mutex;

    QString accountName;
    std::map<QString,double> accountAssets;
};

FeeChooseWidget::FeeChooseWidget(double feeNumber,const QString &coinType,const QString &accountName,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FeeChooseWidget),
    _p(new DataPrivate(feeNumber,coinType,accountName))
{
    ui->setupUi(this);
    qDebug()<<accountName;
    InitWidget();
}

FeeChooseWidget::~FeeChooseWidget()
{
    delete _p;
    delete ui;
}

const QString &FeeChooseWidget::GetFeeID() const
{
    return _p->poundageID;
}

const QString &FeeChooseWidget::GetFeeType() const
{

    return _p->feeType;
}

QString FeeChooseWidget::GetFeeNumber() const
{
    int pre = 5;
    foreach(AssetInfo asset,UBChain::getInstance()->assetInfoMap){
        if(asset.symbol == _p->feeType)
        {
            pre = asset.precision;
            break;
        }
    }
    return QString::number(_p->coinNumber,'f',pre);
}

void FeeChooseWidget::updateFeeNumberSlots(double feeNumber)
{
    _p->feeNumber = std::max<double>(0,feeNumber);
    feeTypeChanged();
}

void FeeChooseWidget::jsonDataUpdated(QString id)
{
    if("feechoose_list_guarantee_order" == id)
    {
        std::lock_guard<std::mutex> lock(_p->mutex);
        //转化为结构体
        QString result = UBChain::getInstance()->jsonDataValue( id);
        result.prepend("{");
        result.append("}");

        std::shared_ptr<PoundageSheet> allPoundageSheet = std::make_shared<PoundageSheet>();
        PoundageDataUtil::convertJsonToPoundage(result,allPoundageSheet);
        std::shared_ptr<PoundageUnit> unit = nullptr;
        if(!allPoundageSheet->poundages.empty())
        {
            allPoundageSheet->sortByRate(false);

            for(auto it = allPoundageSheet->poundages.begin();it != allPoundageSheet->poundages.end();++it)
            {
                if(_p->feeNumber <= (*it)->calSourceLeftNumber())
                {
                    unit = *it;
                    break;
                }
            }
        }
        //解析交易
        ParsePoundage(unit);
        refreshUI();
    }

}

void FeeChooseWidget::coinTypeChanged()
{
    QueryPoundage(ui->comboBox_coinType->currentText());
}

void FeeChooseWidget::feeTypeChanged()
{
    if(ui->checkBox->checkState() == Qt::Checked)
    {

        QueryPoundage(ui->comboBox_coinType->currentText());
    }
    else if(ui->checkBox->checkState() == Qt::Unchecked)
    {
        _p->poundageID = "";
        _p->coinNumber = _p->feeNumber;
        _p->feeType = "LNK";
        refreshUI();
    }
}

void FeeChooseWidget::QueryPoundage(const QString &type)
{
    UBChain::getInstance()->postRPC("feechoose_list_guarantee_order",
                                    toJsonFormat("list_guarantee_order",
                                                 QJsonArray()<<type<<false
                                                 )
                                    );
}

void FeeChooseWidget::updatePoundageID()
{
    qDebug()<<"poundageID---"<<_p->poundageID;
    _p->poundageID.isEmpty()?UBChain::getInstance()->postRPC( "feechoose_remove_guarantee_id",
                                                              toJsonFormat( "remove_guarantee_id",
                                                                            QJsonArray())):
                             UBChain::getInstance()->postRPC( "feechoose_set_guarantee_id",
                                                              toJsonFormat( "set_guarantee_id",
                                                       QJsonArray() << _p->poundageID ));

}

void FeeChooseWidget::updateAccountNameSlots(const QString &accountName,bool refreshTip)
{
    _p->accountName = accountName;
    ResetAccountBalance();
    if(refreshTip)
    {
        checkAccountBalance();
    }
}

void FeeChooseWidget::ParsePoundage(const std::shared_ptr<PoundageUnit> &poundage)
{
    if(poundage == nullptr)
    {
        _p->feeType = "LNK";
        _p->coinNumber = _p->feeNumber;
        _p->poundageID = "";
        _p->poundageTip = tr("cannot find proper acceptance!");
    }
    else
    {
        _p->feeType = poundage->chainType;
        _p->coinNumber = poundage->targetCoinNumber/poundage->sourceCoinNumber*_p->feeNumber;
        _p->poundageID = poundage->poundageID;
        double rate = poundage->sourceCoinNumber/poundage->targetCoinNumber;
        int pre = 5;
        foreach(AssetInfo asset,UBChain::getInstance()->assetInfoMap){
            if(asset.symbol == _p->feeType)
            {
                pre = asset.precision;
                break;
            }
        }
        _p->poundageTip = tr("pay:") + QString::number(_p->coinNumber,'f',pre) + " " + _p->feeType + tr("  rate:") + QString::number(rate);

    }
}

void FeeChooseWidget::refreshUI()
{
    //检查账户余额
    if(!checkAccountBalance())
    {
        //提示余额不足
    }

    if(ui->checkBox->checkState() == Qt::Checked)
    {
        ui->comboBox_coinType->setVisible(true);
        ui->label_poundage->setVisible(true);
        ui->line_5->setVisible(true);
        ui->line_6->setVisible(true);

        ui->label_poundage->setText(_p->poundageTip);

        if(QVBoxLayout* vLay = dynamic_cast<QVBoxLayout*>(layout()))
        {
            vLay->setStretch(5,0);
        }
    }
    else
    {
        ui->comboBox_coinType->setVisible(false);
        ui->label_poundage->setVisible(false);
        ui->line_5->setVisible(false);
        ui->line_6->setVisible(false);
        if(QVBoxLayout* vLay = dynamic_cast<QVBoxLayout*>(layout()))
        {
            vLay->setStretch(5,1);
        }
    }

    //ui->label_fee->setText(QString::number(_p->coinNumber) + " " + _p->feeType);
    ui->label_fee->setText(QString::number(_p->feeNumber,'f',5)+" LNK");

    updatePoundageID();
}

bool FeeChooseWidget::checkAccountBalance() const
{
    ui->tipLabel->setVisible(false);
    if(_p->accountName.isEmpty())
    {
        return true;
    }
    //当前承兑单币种+金额  与 账户中对应资产比较
    qDebug() <<_p->coinNumber <<_p->feeType<< _p->accountAssets[_p->feeType] <<_p->accountAssets.size();
    if(_p->coinNumber > _p->accountAssets[_p->feeType])
    {
        int pre = 5;
        foreach(AssetInfo asset,UBChain::getInstance()->assetInfoMap){
            if(asset.symbol == _p->feeType)
            {
                pre = asset.precision;
                break;
            }
        }

        ui->tipLabel->setText(_p->feeType+tr(" less than ")+QString::number(_p->coinNumber,'f',pre));
        ui->tipLabel->setVisible(true);
        return false;
    }
    else
    {
        ui->tipLabel->setVisible(false);
        return true;
    }

}

void FeeChooseWidget::ResetAccountBalance()
{
    _p->accountAssets.clear();

    AccountInfo info = UBChain::getInstance()->accountInfoMap.value(_p->accountName);

    AssetAmountMap map = info.assetAmountMap;
    QStringList keys = UBChain::getInstance()->assetInfoMap.keys();

    for(int i = 0;i < keys.size();++i)
    {
        QString assetId = keys.at(i);
        AssetInfo assetInfo = UBChain::getInstance()->assetInfoMap.value(assetId);

        _p->accountAssets[assetInfo.symbol] = map.value(assetId).amount/pow(10.,assetInfo.precision);
        qDebug()<<assetInfo.symbol<<_p->accountAssets[assetInfo.symbol];
    }

}

void FeeChooseWidget::InitCoinType()
{
    //初始化币种(币symbol，币id)
    ui->comboBox_coinType->clear();
    foreach(AssetInfo asset,UBChain::getInstance()->assetInfoMap){
        if(asset.id == "1.3.0") continue;
        ui->comboBox_coinType->addItem(asset.symbol,asset.id);
    }
    if(ui->comboBox_coinType->count() > 0)
    {
        ui->comboBox_coinType->setCurrentIndex(0);
    }
}

void FeeChooseWidget::InitWidget()
{
    InitStyle();
    InitCoinType();
    ResetAccountBalance();

    if(_p->feeType == "LNK")
    {
        ui->checkBox->setChecked(false);
    }
    else
    {
        for(int i = 0;i < ui->comboBox_coinType->count();++i)
        {
            if(_p->feeType == ui->comboBox_coinType->itemText(i))
            {
                ui->comboBox_coinType->setCurrentIndex(i);
                break;
            }
        }
        ui->checkBox->setChecked(true);
    }

    connect( UBChain::getInstance(), &UBChain::jsonDataUpdated, this, &FeeChooseWidget::jsonDataUpdated);
    connect(ui->comboBox_coinType,static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),this,&FeeChooseWidget::coinTypeChanged);
    connect(ui->checkBox,&QCheckBox::stateChanged,this,&FeeChooseWidget::feeTypeChanged);

    feeTypeChanged();
}

void FeeChooseWidget::InitStyle()
{
    QFont font("\"Microsoft YaHei UI Light\"",9,50);
    QPalette pa;
    pa.setColor(QPalette::WindowText,QColor(0x54,0x74,0xEB));
    ui->label_poundage->setPalette(pa);
    ui->label_poundage->setFont(font);

    setStyleSheet("QCheckBox::checked{color:black;}");
}
