#ifndef FREECHPAGE_H
#define FREECHPAGE_H

#include "primitives/transaction.h"
#include "platformstyle.h"
#include "sync.h"
#include "util.h"

#include <QMenu>
#include <QTimer>
#include <QWidget>

namespace Ui {
    class FreechPage;
}

class ClientModel;
class WalletModel;

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

class FreechPage : public QWidget
{
    Q_OBJECT

public:
    explicit FreechPage(const PlatformStyle *platformStyle, QWidget *parent = 0);
    ~FreechPage();

    void setClientModel(ClientModel *clientModel);
    void setWalletModel(WalletModel *walletModel);
private:
    QMenu *contextMenu;

public Q_SLOTS:

Q_SIGNALS:

private:
    Ui::FreechPage *ui;
    ClientModel *clientModel;
    WalletModel *walletModel;

private Q_SLOTS:
};
#endif // FREECHPAGE_H
