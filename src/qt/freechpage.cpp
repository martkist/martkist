#include "freechpage.h"
#include "ui_freechpage.h"

#include "clientmodel.h"
#include "clientversion.h"
#include "init.h"
#include "guiutil.h"
#include "qrdialog.h"
#include "sync.h"
#include "wallet/wallet.h"
#include "walletmodel.h"

#include <QNetworkCookieJar>

class FreechWebPage : public QWebPage
{
public:
    FreechWebPage(QObject* parent = 0) : QWebPage(parent) {}
    virtual void javaScriptConsoleMessage(const QString& message, int lineNumber, const QString& sourceID)
    {
        LogPrintf("Freech::Javascript %s (%s:%i)\n", qPrintable(message), qPrintable(sourceID), lineNumber);
    }
};

FreechPage::FreechPage(const PlatformStyle *platformStyle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FreechPage),
    clientModel(0),
    walletModel(0)
{    
    QWebSettings* settings = QWebSettings::globalSettings();
    settings->setAttribute(QWebSettings::LocalStorageEnabled, true);
    auto path = QString::fromStdString((GetDataDir() / "freech").string());
    settings->setOfflineStoragePath(path);
    settings->enablePersistentStorage(path);

    ui->setupUi(this);

    QUrl url = QUrl(QString::fromUtf8("http://localhost:4032/"));
    url.setUserName("user");
    url.setPassword("pwd");

    FreechWebPage *page = new FreechWebPage(ui->webView);
    ui->webView->setPage(page);
    ui->webView->load(url);
    ui->webView->show();
}

FreechPage::~FreechPage()
{
    delete ui;
}

void FreechPage::setClientModel(ClientModel *model)
{
    this->clientModel = model;
    if(model) {
    }
}

void FreechPage::setWalletModel(WalletModel *model)
{
    this->walletModel = model;
}