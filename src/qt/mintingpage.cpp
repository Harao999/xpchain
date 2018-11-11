// Copyright (c) 2011-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/overviewpage.h>
#include <qt/mintingpage.h>
#include <qt/forms/ui_mintingpage.h>

#include <qt/bitcoinunits.h>
#include <qt/clientmodel.h>
#include <qt/guiconstants.h>
#include <qt/guiutil.h>
#include <qt/optionsmodel.h>
#include <qt/platformstyle.h>
#include <qt/transactionfilterproxy.h>
#include <qt/transactiontablemodel.h>
#include <qt/walletmodel.h>

#include <QAbstractItemDelegate>
#include <QPainter>

#define DECORATION_SIZE 54
#define NUM_ITEMS 5



MintingPage::MintingPage(const PlatformStyle *platformStyle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MintingPage)
{
    ui->setupUi(this);

    // use a SingleColorIcon for the "out of sync warning" icon
    QIcon icon = platformStyle->SingleColorIcon(":/icons/warning");
    icon.addPixmap(icon.pixmap(QSize(64,64), QIcon::Normal), QIcon::Disabled); // also set the disabled icon because we are using a disabled QPushButton to work around missing HiDPI support of QLabel (https://bugreports.qt.io/browse/QTBUG-42503)
}

MintingPage::~MintingPage()
{
    delete ui;
}
