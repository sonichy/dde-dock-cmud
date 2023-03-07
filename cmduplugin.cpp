#include "cmduplugin.h"
#include <QLabel>
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextBrowser>
#include <QPushButton>

CMDUPlugin::CMDUPlugin(QObject *parent)
    : QObject(parent),
      m_tipsLabel(new QLabel),
      m_refershTimer(new QTimer(this)),
      m_settings("deepin", "HTYCMDU")
{
    i=db=ub=dbt=ubt=dbt1=ubt1=dbt0=ubt0=0;
    m_tipsLabel->setObjectName("cmdu");
    m_tipsLabel->setStyleSheet("color:white; padding:0px 3px;");
    m_refershTimer->setInterval(1000);
    m_refershTimer->start();
    m_centralWidget = new CMDUWidget;
    connect(m_centralWidget, &CMDUWidget::requestUpdateGeometry, [this] { m_proxyInter->itemUpdate(this, pluginName()); });
    connect(m_refershTimer, &QTimer::timeout, this, &CMDUPlugin::updateCMDU);

    // Boot time
    QProcess *process = new QProcess;
    process->start("systemd-analyze");
    process->waitForFinished();
    QString PO = process->readAllStandardOutput();
    QString SD = PO.mid(PO.indexOf("=") + 1, PO.indexOf("\n") - PO.indexOf("=") - 1);
    startup = "SUT: " + SD;
}

const QString CMDUPlugin::pluginName() const
{
    return "HTYCMDU";
}

const QString CMDUPlugin::pluginDisplayName() const
{
    return "HTYCMDU";
}

void CMDUPlugin::init(PluginProxyInterface *proxyInter)
{
    m_proxyInter = proxyInter;
    if (m_centralWidget->enabled())
        m_proxyInter->itemAdded(this, pluginName());
}

void CMDUPlugin::pluginStateSwitched()
{
    m_centralWidget->setEnabled(!m_centralWidget->enabled());
    if (m_centralWidget->enabled())
        m_proxyInter->itemAdded(this, pluginName());
    else
        m_proxyInter->itemRemoved(this, pluginName());
}

bool CMDUPlugin::pluginIsDisable()
{
    return !m_centralWidget->enabled();
}

int CMDUPlugin::itemSortKey(const QString &itemKey)
{
    Q_UNUSED(itemKey);

    const QString key = QString("pos_%1").arg(displayMode());
    return m_settings.value(key, 0).toInt();
}

void CMDUPlugin::setSortKey(const QString &itemKey, const int order)
{
    Q_UNUSED(itemKey);

    const QString key = QString("pos_%1").arg(displayMode());
    m_settings.setValue(key, order);
}

QWidget *CMDUPlugin::itemWidget(const QString &itemKey)
{
    Q_UNUSED(itemKey);
    return m_centralWidget;
}

QWidget *CMDUPlugin::itemTipsWidget(const QString &itemKey)
{
    Q_UNUSED(itemKey);
    return m_tipsLabel;
}

const QString CMDUPlugin::itemCommand(const QString &itemKey)
{
    Q_UNUSED(itemKey);
    return "";
}

const QString CMDUPlugin::itemContextMenu(const QString &itemKey)
{
    Q_UNUSED(itemKey);

    QList<QVariant> items;
    items.reserve(1);

    QMap<QString, QVariant> about;
    about["itemId"] = "about";
    about["itemText"] = tr("About");
    about["isActive"] = true;
    items.push_back(about);

    QMap<QString, QVariant> changelog;
    changelog["itemId"] = "changelog";
    changelog["itemText"] = tr("Changelog");
    changelog["isActive"] = true;
    items.push_back(changelog);

    QMap<QString, QVariant> boot_analyze;
    boot_analyze["itemId"] = "boot_analyze";
    boot_analyze["itemText"] = tr("Boot analyze");
    boot_analyze["isActive"] = true;
    items.push_back(boot_analyze);

    QMap<QString, QVariant> boot_record;
    boot_record["itemId"] = "boot_record";
    boot_record["itemText"] = tr("Boot record");
    boot_record["isActive"] = true;
    items.push_back(boot_record);

    QMap<QString, QVariant> system_monitor;
    system_monitor["itemId"] = "system_monitor";
    system_monitor["itemText"] = tr("System monitor");
    system_monitor["isActive"] = true;
    items.push_back(system_monitor);

    QMap<QString, QVariant> menu;
    menu["items"] = items;
    menu["checkableMenu"] = false;
    menu["singleCheck"] = false;
    return QJsonDocument::fromVariant(menu).toJson();
}

void CMDUPlugin::invokedMenuItem(const QString &itemKey, const QString &menuId, const bool checked)
{
    Q_UNUSED(itemKey);
    Q_UNUSED(checked);
    if (menuId == "about") {
        about();
    } else if (menuId == "changelog") {
        changelog();
    } else if (menuId == "boot_analyze") {
        bootAnalyze();
    } else if (menuId == "boot_record") {
        bootRecord();
    } else if (menuId == "system_monitor") {
        QProcess::startDetached("deepin-system-monitor");
    }
}

void CMDUPlugin::about()
{
    QMessageBox aboutMB(QMessageBox::NoIcon, "HTYCMDU 3.12", "[About]\n\nDeepin Linux DDE Dock netspeed plugin.\nAuthor：海天鹰\nE-mail: sonichy@163.com\nSource：https://github.com/sonichy/CMDU_DDE_DOCK");
    aboutMB.setIconPixmap(QPixmap(":/icon.png"));
    aboutMB.exec();
}

void CMDUPlugin::changelog()
{
    QString s = "Changelog\n\n3.12 (2023-03-07)\nRemove fix net speed unit KB.\nImprove to 3 digit number.\n\n3.11 (2019-09-25)\nRight menu add [System monitor] to launch deepin-system-monitor.\n\n3.10 (2019-01-10)\n1.Change DDE-Dock API version 1.1 to 1.1.1。\n\n3.9 (2018-12-23)\n1.Use monospace font to fix width.\n\n3.8 (2018-12-14)\n1.Change DDE-Dock API 1.0 version to 1.1.\n\n3.7 (2018-10-12)\n1.Change memory vertial percent line green color to white, change above 90% red background to red line.\n2.Insert bit + monospace font, solve align.\n3.Netspeed unit fix to KB/s.\n\n3.6 (2018-07-30)\n1.Add memory vertical percent line and CPU usage percent vertical line.\n\n3.5 (2018-06-25)\n1.Add boot analyze and boot record.\n\n3.4 (2018-06-03)\n1.Support dock reorder.\n\n3.3 (2018-05-17)\n1.Turn to red background when memory percent over 90%.\n2.Netspeed <999 bytes to 0.00 KB.\n3.Use safty QString.right() replace QStringList.at(), \"ms\" replace \"毫秒\".\n\n3.2 (2018-05-08)\nCalculate all net speed line, no need to choose line.\nRemove startup float window.\n\n3.1 (2018-03-17)\nModigfy free memory algorithm.\n\n3.0 (2018-02-25)\nModify from dock plugin source code, solve right click crash and switch.\n\n2.4 (2017-11-11)\nAdd boot duration.\n\n2.3 (2017-09-05)\nAuto get current netspeed line.\n\n2.２ (2017-07-08)\n1.Set current netspeed line.\n\n2.1 (2017-02-01)\n1.Upload and download support GB, int parameter change to long, solve bytes unit convert overflow.\n\n2.0 (2016-12-07)\n1.Add right click menu.\n\n1.0 (2016-11-01)\n1.Change Qt program to DDE-DOCK.";
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle("HTYCMDU");
    dialog->setFixedSize(400,300);
    QVBoxLayout *vbox = new QVBoxLayout;
    QTextBrowser *textBrowser = new QTextBrowser;
    textBrowser->setText(s);
    textBrowser->zoomIn();
    vbox->addWidget(textBrowser);
    QHBoxLayout *hbox = new QHBoxLayout;
    QPushButton *pushbutton_confirm = new QPushButton(tr("Confirm"));
    hbox->addStretch();
    hbox->addWidget(pushbutton_confirm);
    hbox->addStretch();
    vbox->addLayout(hbox);
    dialog->setLayout(vbox);
    dialog->show();
    connect(pushbutton_confirm, SIGNAL(clicked()), dialog, SLOT(accept()));
    if (dialog->exec() == QDialog::Accepted) {
        dialog->close();
    }
}

QString CMDUPlugin::KB(long k)
{
    QString s = "";
    if (k > 999999) {
        s = QString::number(k/(1024*1024.0),'f',2) + "GB";
    } else {
        if (k > 999) {
            s = QString::number(k/1024.0,'f',2) + "MB";
        } else {
            s = QString::number(k/1.0,'f',2) + "KB";
        }
    }
    return s;
}

QString CMDUPlugin::BS(long b)
{
    QString s = "";
    if (b > 999999999) {
        float f = b / (1024*1024*1024.0);
        if (f >= 100)
            s = " " + QString::number(f, 'f', 0) + "GB";
        else if (f >= 10)
            s = QString::number(f, 'f', 1) + "GB";
        else
            s = QString::number(f, 'f', 2) + "GB";
    } else {
        if (b > 999999) {
            float f = b / (1024*1024.0);
            if (f >= 100)
                s = " " + QString::number(f, 'f', 0) + "MB";
            else if (f >= 10)
                s = QString::number(f, 'f', 1) + "MB";
            else
                s = QString::number(f, 'f', 2) + "MB";
        } else {
            if (b > 999) {
                float f = b / 1024.0;
                if (f >= 100)
                    s = " " + QString::number(f, 'f', 0) + "KB";
                else if (f >= 10)
                    s = QString::number(f, 'f', 1) + "KB";
                else
                    s = QString::number(f, 'f', 2) + "KB";
            } else {
                if (b > 100)
                    s = " " + QString::number(b) + " B";
                else if (b > 10)
                    s = "  " + QString::number(b) + " B";
                else
                    s = "   " + QString::number(b) + " B";
            }
        }
    }
    return s;
}

QString CMDUPlugin::NB(long b)
{
    QString s = "";
    if (b>999) {
        s = QString("%1").arg(b/1024, 5, 'f', 0, QLatin1Char(' ')) + "KB";
    } else { // <1K => 0
        s = QString("%1").arg(0, 5, 'f', 0, QLatin1Char(' ')) + "KB";
    }
    return s;
}

void CMDUPlugin::updateCMDU()
{
    // uptime
    QFile file("/proc/uptime");
    file.open(QIODevice::ReadOnly);
    QString l = file.readLine();
    file.close();
    QTime t(0,0,0);
    t = t.addSecs(l.left(l.indexOf(".")).toInt());
    QString uptime = "UTM: " + t.toString("hh:mm:ss");

    // memory
    file.setFileName("/proc/meminfo");
    file.open(QIODevice::ReadOnly);
    l = file.readLine();
    long mt = l.replace("MemTotal:","").replace("kB","").replace(" ","").toLong();
    l = file.readLine();
    l = file.readLine();
    long ma = l.replace("MemAvailable:","").replace("kB","").replace(" ","").toLong();
    l = file.readLine();
    l = file.readLine();
    file.close();
    long mu = mt - ma;
    int mp = mu*100/mt;
    QString mem = "MEM: " + QString("%1 / %2 = %3").arg(KB(mu)).arg(KB(mt)).arg(QString::number(mp) + "%");

    // CPU
    file.setFileName("/proc/stat");
    file.open(QIODevice::ReadOnly);
    l = file.readLine();
    QByteArray ba;
    ba = l.toLatin1();
    const char *ch;
    ch = ba.constData();
    char cpu[5];
    long user, nice, sys, idle, iowait, irq, softirq, tt;
    sscanf(ch,"%s%ld%ld%ld%ld%ld%ld%ld", cpu, &user, &nice, &sys, &idle, &iowait, &irq, &softirq);
    tt = user + nice + sys + idle + iowait + irq + softirq;
    file.close();
    QString cusage = "";
    int cp = ((tt-tt0)-(idle-idle0))*100/(tt-tt0);
    if (i > 0)
        cusage = "CPU: " + QString::number(cp) + "%";
    idle0 = idle;
    tt0 = tt;

    // net
    file.setFileName("/proc/net/dev");
    file.open(QIODevice::ReadOnly);
    l = file.readLine();
    l = file.readLine();
    dbt1 = ubt1 = 0;
    while (!file.atEnd()) {
        l = file.readLine();
        QStringList list = l.split(QRegExp("\\s{1,}"));
        db = list.at(1).toLong();
        ub = list.at(9).toLong();
        dbt1 += db;
        ubt1 += ub;
    }
    file.close();
    QString uss = "";
    QString dss = "";
    if (i > 0) {
        long us = ubt1 - ubt0;
        long ds = dbt1 - dbt0;
        uss = BS(us) + "/s";
        dss = BS(ds) + "/s";
        ubt0 = ubt1;
        dbt0 = dbt1;
    }
    QString netspeed = "↑" + uss + "\n↓" + dss;
    QString net = "UPB: " + BS(ubt1) + "  " + uss + "\nDNB: " + BS(dbt1) + "  " + dss;

    i++;
    if (i>2)
        i = 2;

    // draw
    m_tipsLabel->setText(startup + "\n" + uptime + "\n" + cusage + "\n" + mem + "\n" + net);
    m_centralWidget->text = netspeed;
    m_centralWidget->mp = mp;
    m_centralWidget->cp = cp;
    m_centralWidget->update();
}

void CMDUPlugin::bootRecord()
{
    QProcess *process = new QProcess;
    process->start("last reboot");
    process->waitForFinished();
    QString PO = process->readAllStandardOutput();
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle(tr("Boot record"));
    dialog->setFixedSize(600,400);
    QVBoxLayout *vbox = new QVBoxLayout;
    QTextBrowser *textBrowser = new QTextBrowser;
    textBrowser->setText(PO);
    textBrowser->zoomIn();
    vbox->addWidget(textBrowser);
    QHBoxLayout *hbox = new QHBoxLayout;
    QPushButton *pushButton_confirm = new QPushButton(tr("Confirm"));
    hbox->addStretch();
    hbox->addWidget(pushButton_confirm);
    hbox->addStretch();
    vbox->addLayout(hbox);
    dialog->setLayout(vbox);
    dialog->show();
    connect(pushButton_confirm, SIGNAL(clicked()), dialog, SLOT(accept()));
    if (dialog->exec() == QDialog::Accepted) {
        dialog->close();
    }
}

void CMDUPlugin::bootAnalyze()
{
    QProcess *process = new QProcess;
    process->start("systemd-analyze blame");
    process->waitForFinished();
    QString PO = process->readAllStandardOutput();
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle(tr("Boot analyze"));
    dialog->setFixedSize(500,400);
    QVBoxLayout *vbox = new QVBoxLayout;
    QTextBrowser *textBrowser = new QTextBrowser;
    textBrowser->setText(PO);
    textBrowser->zoomIn();
    vbox->addWidget(textBrowser);
    QHBoxLayout *hbox = new QHBoxLayout;
    QPushButton *pushButton_confirm = new QPushButton(tr("Confirm"));
    hbox->addStretch();
    hbox->addWidget(pushButton_confirm);
    hbox->addStretch();
    vbox->addLayout(hbox);
    dialog->setLayout(vbox);
    dialog->show();
    connect(pushButton_confirm, SIGNAL(clicked()), dialog, SLOT(accept()));
    if (dialog->exec() == QDialog::Accepted) {
        dialog->close();
    }
}