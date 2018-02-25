#include "cmduplugin.h"
#include <QLabel>
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>

CMDUPlugin::CMDUPlugin(QObject *parent)
    : QObject(parent),
      m_tipsLabel(new QLabel),
      m_refershTimer(new QTimer(this))
{
    i=0, db=0, ub=0, tt0=0, idle0=0, nl=3;

    m_tipsLabel->setObjectName("cmdu");
    m_tipsLabel->setStyleSheet("color:white; padding:0px 3px;");

    m_refershTimer->setInterval(1000);
    m_refershTimer->start();

    m_centralWidget = new CMDUWidget;

    connect(m_centralWidget, &CMDUWidget::requestContextMenu, [this] { m_proxyInter->requestContextMenu(this, QString()); });
    connect(m_centralWidget, &CMDUWidget::requestUpdateGeometry, [this] { m_proxyInter->itemUpdate(this, QString()); });

    connect(m_refershTimer, &QTimer::timeout, this, &CMDUPlugin::updateCMDU);

    // 开机时长
    QProcess *process = new QProcess;
    process->start("systemd-analyze");
    process->waitForFinished();
    QString PO = process->readAllStandardOutput();
    QStringList SLSA = PO.split(" = ");
    QString SD = SLSA.at(1);
    if(SD.contains("min"))SD.replace("min","分");
    labelStartupDuration = new QLabel;
    labelStartupDuration->setText(SD.mid(0,SD.indexOf(".")) + "秒");
    labelStartupDuration->setFixedSize(QSize(200,150));
    labelStartupDuration->setWindowFlags(Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
    QFont font;
    font.setPointSize(30);
    labelStartupDuration->setFont(font);
    labelStartupDuration->setAlignment(Qt::AlignCenter);
    labelStartupDuration->setStyleSheet("QLabel { padding:2px; color:white; background-color:#00FF00;}");
    labelStartupDuration->adjustSize();
    labelStartupDuration->move(QApplication::desktop()->width()-labelStartupDuration->width()-10, QApplication::desktop()->height()-labelStartupDuration->height()-50);
    labelStartupDuration->show();
    QTimer::singleShot(5000, labelStartupDuration, SLOT(hide()));
}

const QString CMDUPlugin::pluginName() const
{
    return "cmdu";
}

const QString CMDUPlugin::pluginDisplayName() const
{
    return "网速";
}

void CMDUPlugin::init(PluginProxyInterface *proxyInter)
{
    m_proxyInter = proxyInter;
    if (m_centralWidget->enabled())
        m_proxyInter->itemAdded(this, QString());
}

void CMDUPlugin::pluginStateSwitched()
{
    m_centralWidget->setEnabled(!m_centralWidget->enabled());
    if (m_centralWidget->enabled())
        m_proxyInter->itemAdded(this, QString());
    else
        m_proxyInter->itemRemoved(this, QString());
}

bool CMDUPlugin::pluginIsDisable()
{
    return !m_centralWidget->enabled();
}

int CMDUPlugin::itemSortKey(const QString &itemKey)
{
    Q_UNUSED(itemKey);
    return -1;
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
    labelStartupDuration->show();
    QTimer::singleShot(5000, labelStartupDuration, SLOT(hide()));
    return "";
}

const QString CMDUPlugin::itemContextMenu(const QString &itemKey)
{
    Q_UNUSED(itemKey);

    QList<QVariant> items;
    items.reserve(1);

    QMap<QString, QVariant> about;
    about["itemId"] = "about";
    about["itemText"] = "关于";
    about["isActive"] = true;
    items.push_back(about);

    QMap<QString, QVariant> changelog;
    changelog["itemId"] = "changelog";
    changelog["itemText"] = "更新历史";
    changelog["isActive"] = true;
    items.push_back(changelog);

    QMap<QString, QVariant> menu;
    menu["items"] = items;
    menu["checkableMenu"] = false;
    menu["singleCheck"] = false;
    return QJsonDocument::fromVariant(menu).toJson();
}

void CMDUPlugin::invokedMenuItem(const QString &itemKey, const QString &menuId, const bool checked)
{
    Q_UNUSED(itemKey)
    Q_UNUSED(checked)

    QStringList menuitems;
    menuitems << "about" << "changelog";

    switch(menuitems.indexOf(menuId)){
    case 0:
        MBAbout();
        break;
    case 1:
        MBChangeLog();
        break;
    }
}

void CMDUPlugin::MBAbout()
{
    QMessageBox aboutMB(QMessageBox::NoIcon, "系统信息 3.0", "关于\n\n深度Linux系统上一款在任务栏显示网速，鼠标悬浮显示开机时间、CPU占用、内存占用、下载字节、上传字节的插件。\n作者：黄颖\nE-mail: sonichy@163.com\n源码：https://github.com/sonichy/CMDU_DDE_DOCK\n致谢：\nlinux028@deepin.org");
    aboutMB.setIconPixmap(QPixmap(":/icon.png"));
    aboutMB.exec();
}

void CMDUPlugin::MBChangeLog()
{
    QMessageBox changeLogMB(QMessageBox::NoIcon, "系统信息 3.0", "更新历史\n\n3.0 (2018-02-25)\n在新版本时间插件源码基础上修改，解决右键崩溃问题，并支持右键开关。\n\n2.4 (2017-11-11)\n增加开机时间。\n\n2.3 (2017-09-05)\n自动判断网速所在行。\n\n2.２ (2017-07-08)\n1.设置网速所在行。\n\n2.1 (2017-02-01)\n1.上传下载增加GB单位换算，且参数int改long，修复字节单位换算溢出BUG。\n\n2.0 (2016-12-07)\n1.增加右键菜单。\n\n1.0 (2016-11-01)\n1.把做好的Qt程序移植到DDE-DOCK。");
    changeLogMB.exec();
}

QString CMDUPlugin::KB(long k)
{
    QString s = "";
    if(k > 999999){
        s = QString::number(k/(1024*1024.0),'f',2) + "GB";
    }else{
        if(k > 999){
            s = QString::number(k/1024.0,'f',2) + "MB";
        }else{
            s = QString::number(k/1.0,'f',2) + "KB";
        }
    }
    return s;
}

QString CMDUPlugin::BS(long b)
{
    QString s = "";
    if(b > 999999999){
        s = QString::number(b/(1024*1024*1024.0),'f',2) + "GB";
    }else{
        if(b > 999999){
            s = QString::number(b/(1024*1024.0),'f',2) + "MB";
        }else{
            if(b>999){
                s = QString::number(b/1024.0,'f',2) + "KB";
            }else{
                s = QString::number(b/1.0,'f',2) + "B";
            }
        }
    }
    return s;
}

void CMDUPlugin::updateCMDU()
{
    QFile file("/proc/uptime");
    file.open(QIODevice::ReadOnly);
    QString l = file.readLine();
    file.close();
    QTime t(0,0,0);
    t = t.addSecs(l.left(l.indexOf(".")).toInt());
    QString uptime = "开机: " + t.toString("hh:mm:ss");

    file.setFileName("/proc/meminfo");
    file.open(QIODevice::ReadOnly);
    l = file.readLine();
    long mt=l.mid(l.indexOf(":")+1,l.length()-13).replace(" ","").toInt();
    l = file.readLine();
    file.close();
    long mf = l.mid(l.indexOf(":")+1,l.length()-11).replace(" ","").toInt();
    long mu = mt-mf;
    QString musage = QString::number(mu*100/mt) + "%";
    QString mem = "内存: " + QString("%1/%2=%3").arg(KB(mu)).arg(KB(mt)).arg(musage);

    file.setFileName("/proc/stat");
    file.open(QIODevice::ReadOnly);
    l = file.readLine();
    QByteArray ba;
    ba = l.toLatin1();
    const char *ch;
    ch = ba.constData();
    char cpu[5];
    long int user,nice,sys,idle,iowait,irq,softirq,tt;
    sscanf(ch,"%s%ld%ld%ld%ld%ld%ld%ld",cpu,&user,&nice,&sys,&idle,&iowait,&irq,&softirq);
    tt = user + nice + sys + idle + iowait + irq + softirq;
    file.close();
    QString cusage = "";
    if(i>0) cusage = "CPU: " + QString::number(((tt-tt0)-(idle-idle0))*100/(tt-tt0)) + "%";
    idle0 = idle;
    tt0 = tt;

    file.setFileName("/proc/net/dev");
    file.open(QIODevice::ReadOnly);
    l = file.readLine();
    l = file.readLine();
    l = file.readLine();
    if(l.contains("lo",Qt::CaseInsensitive)) l = file.readLine();
    file.close();
    QStringList list = l.split(QRegExp("\\s{1,}")); // 第一个\表示转义字符，\s表示空格，｛1，｝表示一个以上
    QString dss = "";
    QString uss = "";
    if(i>0){
        long ds = list.at(1).toLong() - db;
        long us = list.at(9).toLong() - ub;
        dss = BS(ds)+"/s";
        uss = BS(us)+"/s";
    }
    db = list.at(1).toLong();
    ub = list.at(9).toLong();
    QString net = "下载: " + BS(db) + "  " + dss + "\n上传: " + BS(ub) + "  " + uss;

    m_tipsLabel->setText(uptime + "\n" + cusage + "\n" + mem + "\n" + net);
    m_centralWidget->text = "↑" + uss + "\n↓" + dss;
    m_centralWidget->update();
    i++;    
}
