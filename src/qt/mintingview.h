#ifndef BITCOIN_QT_MINTINGVIEW_H
#define BITCOIN_QT_MINTINGVIEW_H

#include <qt/guiutil.h>

#include <QWidget>
#include <QComboBox>
#include <qt/mintingfilterproxy.h>

class PlatformStyle;
class MintingFilterProxy;
class WalletModel;


QT_BEGIN_NAMESPACE
class QComboBox;
class QDateTimeEdit;
class QFrame;
class QLineEdit;
class QMenu;
class QModelIndex;
class QSignalMapper;
class QTableView;
QT_END_NAMESPACE

class MintingView : public QWidget
{
    Q_OBJECT
public:
    explicit MintingView(const PlatformStyle *platformStyle, QWidget *parent = 0);
    void setModel(WalletModel *model);

    enum MintingEnum
    {
        Minting10min,
        Minting1day,
        Minting7days,
        Minting30days,
        Minting60days,
    };

private:
    WalletModel *model;
    QTableView *mintingView;
    QComboBox *mintingCombo;
    MintingFilterProxy *mintingProxyModel;
    QMenu *contextMenu;

private Q_SLOTS:
    void contextualMenu(const QPoint &);
    void copyAddress();
    void copyTransactionId();
    void showHideAddress();
    void showHideTxID();

Q_SIGNALS:

public Q_SLOTS:
    void exportClicked();
    void chooseMintingInterval(int idx);
};

#endif // BITCOIN_QT_MINTINGVIEW_H
