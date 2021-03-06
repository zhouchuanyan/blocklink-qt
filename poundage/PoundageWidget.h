#ifndef POUNDAGEWIDGET_H
#define POUNDAGEWIDGET_H

#include <QWidget>
//////////////////////////////////////////////////////////////////////////
///<summary>手续费承税单界面 </summary>
///
///<remarks> 2018.04.08 --朱正天  </remarks>/////////////////////////////
///////////////////////////////////////////////////////////////////////////
namespace Ui {
class PoundageWidget;
}

class PoundageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PoundageWidget(QWidget *parent = 0);
    ~PoundageWidget();
public slots:
    void autoRefreshSlots();
private slots:
    void PublishPoundageSlots();
    void ShowAllPoundageSlots();
    void ShowMyPoundageSlots();

    void SortByStuffSlots();
    //void
    void jsonDataUpdated(QString id);

    //删除自己的承税单
    void DeletePoundageSlots(const QString &orderID,const QString &accountName);

    //设置默认承税单
    void SetDefaultPoundageSlots(const QString &orderID);
private:
    void InitWidget();
    void InitStyle();
    void InitCoinType();

    void updateButtonStyle();//切换全部、我的承兑单时
private:
    Ui::PoundageWidget *ui;
private:
    class PoundageWidgetPrivate;
    PoundageWidgetPrivate *_p;
};

#endif // POUNDAGEWIDGET_H
