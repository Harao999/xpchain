#include <qt/transactiontablemodel.h>

#include <qt/addresstablemodel.h>
#include <qt/guiconstants.h>
#include <qt/guiutil.h>
#include <qt/optionsmodel.h>
#include <qt/platformstyle.h>
#include <qt/transactiondesc.h>
#include <qt/transactionrecord.h>
#include <qt/walletmodel.h>
#include <qt/mintingtablemodel.h>
#include <qt/mintingfilterproxy.h>

#include <core_io.h>
#include <interfaces/handler.h>
#include <interfaces/node.h>
#include <interfaces/wallet.h>
#include <validation.h>
#include <sync.h>
#include <uint256.h>
#include <util.h>
#include <key_io.h>
#include <consensus/consensus.h>
#include <timedata.h>
#include <stdint.h>
#include <chain.h>
#include <chainparams.h>
#include <math.h>

#include <rpc/blockchain.h>

#include <kernelrecord.h>
#include <kernel.h>

#include <QAbstractItemDelegate>
#include <QPainter>
#include <QLocale>
#include <QList>
#include <QColor>
#include <QTimer>
#include <QIcon>
#include <QDebug>
#include <QDateTime>
#include <QtAlgorithms>

// Amount column is right-aligned it contains numbers
static int column_alignments[] = {
    Qt::AlignLeft|Qt::AlignVCenter,
    Qt::AlignLeft|Qt::AlignVCenter,
    Qt::AlignRight|Qt::AlignVCenter,
    Qt::AlignRight|Qt::AlignVCenter,
    Qt::AlignRight|Qt::AlignVCenter,
    Qt::AlignRight|Qt::AlignVCenter,
    Qt::AlignRight|Qt::AlignVCenter
};

// Comparison operator for sort/binary search of model tx list
struct TxLessThan
{
    bool operator()(const KernelRecord &a, const KernelRecord &b) const
    {
        return a.hash < b.hash || (a.hash == b.hash && a.n < b.n);
    }
};

// Private implementation
class MintingTablePriv
{
public:
    MintingTablePriv(MintingTableModel *_parent):
            parent(_parent)
    {
    }

    MintingTableModel *parent;

    /* Local cache of wallet.
     * As it is in the same order as the CWallet, by definition
     * this is sorted by sha256.
     */
    QList<KernelRecord> cachedWallet;

    /* Query entire wallet anew from core.
     */
    void refreshWallet(interfaces::Wallet& wallet)
    {
        qDebug() << "MintingTablePriv::refreshWallet";

        // Make mask
        QList<bool> mask;
        for(int i = 0; i < cachedWallet.size(); i++)
        {
            mask.append(false);
        }

        // Update and add records
        for (const auto& coins : wallet.listCoins())
        {
            for (const auto& outpair : coins.second)
            {
                const COutPoint& output = std::get<0>(outpair);
                const interfaces::WalletTxOut& out = std::get<1>(outpair);
                std::vector<KernelRecord> txList = KernelRecord::decomposeOutput(output, out);
                if(KernelRecord::showTransaction())
                {

                    for(const KernelRecord& kr : txList)
                    {
                        QList<KernelRecord>::iterator lower = qLowerBound(
                            cachedWallet.begin(), cachedWallet.end(), kr, TxLessThan());
                        QList<KernelRecord>::iterator upper = qUpperBound(
                            cachedWallet.begin(), cachedWallet.end(), kr, TxLessThan());
                        int lowerIndex = (lower - cachedWallet.begin());
                        bool inModel = (lower != upper);

                        if(inModel)
                        {
                            cachedWallet.replace(lowerIndex, kr);
                            mask.replace(lowerIndex, true);
                        }
                        else
                        {
                            parent->beginInsertRows(QModelIndex(), lowerIndex, lowerIndex);
                            cachedWallet.insert(lowerIndex, kr);
                            mask.insert(lowerIndex, true);
                            parent->endInsertRows();
                        }
                    }
                }
            }
        }
        // Delete old records
        for(int i = 0; i < mask.size(); i++)
        {
            if(mask.at(i) == false)
            {
                parent->beginRemoveRows(QModelIndex(), i, i);
                cachedWallet.removeAt(i);
                parent->endRemoveRows();
            }
        }
    }

    int size()
    {
        return cachedWallet.size();
    }

    KernelRecord *index(int idx)
    {
        if(idx >= 0 && idx < cachedWallet.size())
        {
            KernelRecord *rec = &cachedWallet[idx];
            return rec;
        }
        else
        {
            return 0;
        }
    }

    QString describe(interfaces::Node& node, interfaces::Wallet& wallet, TransactionRecord *rec, int unit)
    {
        /*{
            LOCK(wallet->cs_wallet);
            std::map<uint256, CWalletTx>::iterator mi = wallet->mapWallet.find(rec->hash);
            if(mi != wallet->mapWallet.end())
            {*/
                return TransactionDesc::toHTML(node, wallet, rec, unit);
            /*}
        }
        return QString("");*/
    }

};

MintingTableModel::MintingTableModel(const PlatformStyle *_platformStyle, WalletModel *parent):
        QAbstractTableModel(parent),
        walletModel(parent),
        mintingInterval(10),
        priv(new MintingTablePriv(this)),
        //cachedNumBlocks(0),
        platformStyle(_platformStyle)
{
    columns << tr("Transaction") <<  tr("Address") << tr("Balance") << tr("Age") << tr("CoinDay") << tr("MintProbability") << tr("MintReward");

    priv->refreshWallet(walletModel->wallet());

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateAge()));
    timer->start(MODEL_MINTING_UPDATE_DELAY);

    connect(walletModel->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));
}

MintingTableModel::~MintingTableModel()
{
    delete priv;
}

void MintingTableModel::updateTransaction(const QString &hash, int status, bool showTransaction)
{
    uint256 updated;
    updated.SetHex(hash.toStdString());

    priv->refreshWallet(walletModel->wallet());
    mintingProxyModel->invalidate(); // Force deletion of empty rows
}

void MintingTableModel::updateAge()
{
    Q_EMIT dataChanged(index(0, Age), index(priv->size()-1, Age));
    Q_EMIT dataChanged(index(0, CoinDay), index(priv->size()-1, CoinDay));
    Q_EMIT dataChanged(index(0, MintProbability), index(priv->size()-1, MintProbability));
    Q_EMIT dataChanged(index(0, MintReward), index(priv->size()-1, MintReward));
    priv->refreshWallet(walletModel->wallet());
}

void MintingTableModel::setMintingProxyModel(MintingFilterProxy *mintingProxy)
{
    mintingProxyModel = mintingProxy;
}

int MintingTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return priv->size();
}

int MintingTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.length();
}

QVariant MintingTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    KernelRecord *rec = static_cast<KernelRecord*>(index.internalPointer());

    switch(role)
    {
      case Qt::DisplayRole:
        switch(index.column())
        {
        case Address:
            return formatTxAddress(rec, false);
        case TxHash:
            return formatTxHash(rec);
        case Age:
            return formatTxAge(rec);
        case Balance:
            return formatTxBalance(rec);
        case CoinDay:
            return formatTxCoinDay(rec);
        case MintProbability:
            return formatDayToMint(rec);
        case MintReward:
            return formatTxPoSReward(rec);
        }
        break;
      case Qt::TextAlignmentRole:
        return column_alignments[index.column()];
        break;
      case Qt::ToolTipRole:
        switch(index.column())
        {
        case MintProbability:
            int interval = this->mintingInterval;
            QString unit = tr("minutes");

            int hours = interval / 60;
            int days = hours  / 24;

            if(hours > 1) {
                interval = hours;
                unit = tr("hours");
            }
            if(days > 1) {
                interval = days;
                unit = tr("days");
            }

            QString str = QString(tr("You have %1 chance to find a POS block if you mint %2 %3 at current difficulty."));
            return str.arg(index.data().toString().toUtf8().constData()).arg(interval).arg(unit);
        }
        break;
      case Qt::EditRole:
        switch(index.column())
        {
        case Address:
            return formatTxAddress(rec, false);
        case TxHash:
            return formatTxHash(rec);
        case Age:
            return (qlonglong)rec->getAge();
        case CoinDay:
            return (qlonglong)rec->getCoinDay();
        case Balance:
            return (qlonglong)rec->nValue;
        case MintProbability:
            return getDayToMint(rec);
        case MintReward:
            return formatTxPoSReward(rec);
        }
        break;
      case Qt::BackgroundColorRole:
        int minAge = Params().GetConsensus().nStakeMinAge / 60 / 60 / 24;
        int maxAge = Params().GetConsensus().nStakeMaxAge / 60 / 60 / 24;
        if(rec->getAge() < minAge)
        {
            return COLOR_MINT_YOUNG;
        }
        else if (rec->getAge() >= minAge && rec->getAge() < maxAge)
        {
            return COLOR_MINT_MATURE;
        }
        else
        {
            return COLOR_MINT_OLD;
        }
        break;

    }
    return QVariant();
}

void MintingTableModel::setMintingInterval(int interval)
{
    mintingInterval = interval;
}

QString MintingTableModel::lookupAddress(const std::string &address, bool tooltip) const
{
    QString label = walletModel->getAddressTableModel()->labelForAddress(QString::fromStdString(address));
    QString description;
    if(!label.isEmpty())
    {
        description += label;
    }
    if(label.isEmpty() || tooltip)
    {
        description += QString(" (") + QString::fromStdString(address) + QString(")");
    }
    return description;
}

QString MintingTableModel::formatTxPoSReward(KernelRecord *wtx) const
{
    QString posReward;
    posReward += QString(QObject::tr("from  %1 to %2")).arg(BitcoinUnits::formatWithUnit(walletModel->getOptionsModel()->getDisplayUnit(), wtx->getPoSReward(0)),
        BitcoinUnits::formatWithUnit(walletModel->getOptionsModel()->getDisplayUnit(), wtx->getPoSReward(mintingInterval)));
    return posReward;
}
double MintingTableModel::getDayToMint(KernelRecord *wtx) const
{
    double difficulty = GetDifficulty(chainActive.Tip());

    double prob = wtx->getProbToMintWithinNMinutes(difficulty, mintingInterval);
    prob = prob * 100;
    return prob;
}

QString MintingTableModel::formatDayToMint(KernelRecord *wtx) const
{
    double prob = getDayToMint(wtx);
    return QString::number(prob, 'f', 6) + "%";
}

QString MintingTableModel::formatTxAddress(const KernelRecord *wtx, bool tooltip) const
{
    return QString::fromStdString(wtx->address);
}

QString MintingTableModel::formatTxHash(const KernelRecord *wtx) const
{
    return QString::fromStdString(wtx->hash.ToString());
}

QString MintingTableModel::formatTxCoinDay(const KernelRecord *wtx) const
{
    return QString::number(wtx->getCoinDay());
}

QString MintingTableModel::formatTxAge(const KernelRecord *wtx) const
{
    int64_t nAge = wtx->getAge();
    return QString::number(nAge);
}

QString MintingTableModel::formatTxBalance(const KernelRecord *wtx) const
{
    return BitcoinUnits::format(walletModel->getOptionsModel()->getDisplayUnit(), wtx->nValue);
}

QVariant MintingTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole)
        {
            return columns[section];
        }
        else if (role == Qt::TextAlignmentRole)
        {
            return column_alignments[section];
        } else if (role == Qt::ToolTipRole)
        {
            switch(section)
            {
            case Address:
                return tr("Destination address of the output.");
            case TxHash:
                return tr("Original transaction id.");
            case Age:
                return tr("Age of the transaction in days.");
            case Balance:
                return tr("Balance of the output.");
            case CoinDay:
                return tr("Coin age in the output.");
            case MintProbability:
                return tr("Chance to mint a block within given time interval.");
            case MintReward:
                return tr("The size of the potential rewards if the block is found at the beginning and the end given time interval.");
            }
        }
    }
    return QVariant();
}

QModelIndex MintingTableModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    KernelRecord *data = priv->index(row);
    if(data)
    {
        return createIndex(row, column, priv->index(row));
    }
    else
    {
        return QModelIndex();
    }
}

void MintingTableModel::updateDisplayUnit()
{
    // emit dataChanged to update Balance column with the current unit
    Q_EMIT dataChanged(index(0, Balance), index(priv->size()-1, Balance));
}
