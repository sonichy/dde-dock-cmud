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
    i=db=ub=dbt=ubt=dbt1=ubt1=dbt0=ubt0=0;
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
    //QStringList SLSA = PO.split(" = ");
    //QString SD = SLSA.at(1);
    //QString SD = PO.right(PO.length() - PO.indexOf("=") - 1);
    QString SD = PO.mid(PO.indexOf("=") + 1, PO.indexOf("\n") - PO.indexOf("=") - 1);
    //SD.replace("\n","");
    SD.replace("min"," 分");
    SD.replace("ms"," 毫秒");
    SD.replace("s"," 秒");
    startup = "启动: " + SD;
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
    changelog["itemText"] = "更新日志";
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
    QMessageBox aboutMB(QMessageBox::NoIcon, "系统信息 3.3", "关于\n\n深度Linux系统上一款在任务栏显示网速，鼠标悬浮显示开机时间、CPU占用、内存占用、下载字节、上传字节的插件。\n作者：黄颖\nE-mail: sonichy@163.com\n源码：https://github.com/sonichy/CMDU_DDE_DOCK\n致谢：\nlinux028@deepin.org");
    aboutMB.setIconPixmap(QPixmap(":/icon.png"));
    aboutMB.exec();
}

void CMDUPlugin::MBChangeLog()
{
    QMessageBox changeLogMB(QMessageBox::NoIcon, "系统信息 3.3", "更新日志\n\n3.3 (2018-05-17)\n1.内存超过90%变红预警。\n2.网速小于 999 字节显示为 0.00 KB\n3.使用安全的 QString.right() 替代 QStringList.at()，增加：ms 替换为 毫秒。\n\n3.2 (2018-05-08)\n网速全部计算，不会再出现为0的情况。\n取消启动时间浮窗。\n\n3.1 (2018-03-17)\n修改空余内存计算范围。\n\n3.0 (2018-02-25)\n在新版本时间插件源码基础上修改，解决右键崩溃问题，并支持右键开关。\n\n2.4 (2017-11-11)\n增加开机时间。\n\n2.3 (2017-09-05)\n自动判断网速所在行。\n\n2.２ (2017-07-08)\n1.设置网速所在行。\n\n2.1 (2017-02-01)\n1.上传下载增加GB单位换算，且参数int改long，修复字节单位换算溢出BUG。\n\n2.0 (2016-12-07)\n1.增加右键菜单。\n\n1.0 (2016-11-01)\n1.把做好的Qt程序移植到DDE-DOCK。");
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
                s = QString::number(0,'f',2) + "KB";
            }
        }
    }
    return s;
}

void CMDUPlugin::updateCMDU()
{
    // 开机
    QFile file("/proc/uptime");
    file.open(QIODevice::ReadOnly);
    QString l = file.readLine();
    file.close();
    QTime t(0,0,0);
    t = t.addSecs(l.left(l.indexOf(".")).toInt());
    QString uptime = "开机: " + t.toString("hh:mm:ss");

    //内存
    file.setFileName("/proc/meminfo");
    file.open(QIODevice::ReadOnly);
    l = file.readLine();
    long mt = l.replace("MemTotal:","").replace("kB","").replace(" ","").toLong();
    l = file.readLine();
    //long mf = l.replace("MemFree:","").replace("kB","").replace(" ","").toLong();
    l = file.readLine();
    long ma = l.replace("MemAvailable:","").replace("kB","").replace(" ","").toLong();
    l = file.readLine();
    //long mb = l.replace("Buffers:","").replace("kB","").replace(" ","").toLong();
    l = file.readLine();
    //long mc = l.replace("Cached:","").replace("kB","").replace(" ","").toLong();
    file.close();
    //long mu = mt - mf - mb -mc;
    long mu = mt - ma;
    int mp = mu*100/mt;
    QString mem = "内存: " + QString("%1/%2=%3").arg(KB(mu)).arg(KB(mt)).arg(QString::number(mp) + "%");

    // CPU
    file.setFileName("/proc/stat");
    file.open(QIODevice::ReadOnly);
    l = file.readLine();
    QByteArray ba;
    ba = l.toLatin1();
    const char *ch;
    ch = ba.constData();
    char cpu[5];
    long user,nice,sys,idle,iowait,irq,softirq,tt;
    sscanf(ch,"%s%ld%ld%ld%ld%ld%ld%ld",cpu,&user,&nice,&sys,&idle,&iowait,&irq,&softirq);
    tt = user + nice + sys + idle + iowait + irq + softirq;
    file.close();
    QString cusage = "";
    if(i>0) cusage = "CPU: " + QString::number(((tt-tt0)-(idle-idle0))*100/(tt-tt0)) + "%";
    idle0 = idle;
    tt0 = tt;

    // 网速
    file.setFileName("/proc/net/dev");
    file.open(QIODevice::ReadOnly);
    l = file.readLine();
    l = file.readLine();
    dbt1=ubt1=0;
    while(!file.atEnd()){
        l = file.readLine();
        //if(l.contains("lo",Qt::CaseInsensitive))l = file.readLine();
        QStringList list = l.split(QRegExp("\\s{1,}"));
        db = list.at(1).toLong();
        ub = list.at(9).toLong();
        dbt1 += db;
        ubt1 += ub;
    }
    file.close();
    QString dss = "";
    QString uss = "";
    if(i > 0){
        long ds = dbt1 - dbt0;
        long us = ubt1 - ubt0;
        dbt = dbt0 + ds;
        ubt = ubt0 + us;
        dss = BS(ds) + "/s";
        uss = BS(us) + "/s";
        dbt0 = dbt1;
        ubt0 = ubt1;
    }
    QString netspeed = "↑ " + uss + "\n↓ " + dss;
    QString net = "上传: " + BS(ubt) + "  " + uss + "\n下载: " + BS(dbt) + "  " + dss;

    i++;
    if(i>2)i=2;

    // 绘制
    m_tipsLabel->setText(startup + "\n" + uptime + "\n" + cusage + "\n" + mem + "\n" + net);
    m_centralWidget->text = netspeed;
    m_centralWidget->mp = mp;
    m_centralWidget->update();

}
