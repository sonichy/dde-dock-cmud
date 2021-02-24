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
    QMessageBox aboutMB(QMessageBox::NoIcon, "HTYCMDU 3.11", "[About]\n\nDeepin Linux DDE Dock netspeed plugin.\nAuthor：海天鹰\nE-mail: sonichy@163.com\nSource：https://github.com/sonichy/CMDU_DDE_DOCK");
    aboutMB.setIconPixmap(QPixmap(":/icon.png"));
    aboutMB.exec();
}

void CMDUPlugin::changelog()
{
    QString s = "Changelog\n\n3.11 (2019-09-25)\nRight menu add [System monitor] to launch deepin-system-monitor.\n\n3.10 (2019-01-10)\n1.Change DDE-Dock API version 1.1 to 1.1.1。\n\n3.9 (2018-12-23)\n1.Use monospace font to fix width.\n\n3.8 (2018-12-14)\n1.Change DDE-Dock API 1.0 version to 1.1.\n\n3.7 (2018-10-12)\n1.Change memory vertial percent line green color to white, change above 90% red background to red line.\n2.Insert bit + monospace font, solve align.\n3.Netspeed unit fix to KB/s.\n\n3.6 (2018-07-30)\n1.Add memory vertical percent line and CPU usage percent vertical line.\n\n3.5 (2018-06-25)\n1.Add boot analyze and boot record.\n\n3.4 (2018-06-03)\n1.Support dock reorder.\n\n3.3 (2018-05-17)\n1.Turn to red background when memory percent over 90%.\n2.Netspeed <999 bytes to 0.00 KB.\n3.Use safty QString.right() replace QStringList.at(), \"ms\" replace \"毫秒\".\n\n3.2 (2018-05-08)\nCalculate all net speed line, no need to choose line.\nRemove startup float window.\n\n3.1 (2018-03-17)\nModigfy free memory algorithm.\n\n3.0 (2018-02-25)\nModify from dock plugin source code, solve right click crash and switch.\n\n2.4 (2017-11-11)\nAdd boot duration.\n\n2.3 (2017-09-05)\nAuto get current netspeed line.\n\n2.２ (2017-07-08)\n1.Set current netspeed line.\n\n2.1 (2017-02-01)\n1.Upload and download support GB, int parameter change to long, solve bytes unit convert overflow.\n\n2.0 (2016-12-07)\n1.Add right click menu.\n\n1.0 (2016-11-01)\n1.Change Qt program to DDE-DOCK.";
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
    if(dialog->exec() == QDialog::Accepted){
        dialog->close();
    }
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
        //s = QString("%1").arg(b/(1024*1024*1024.0), 6, 'f', 2, QLatin1Char(' ')) + "GB";
        s = QString::number(b/(1024*1024*1024.0), 'f', 2) + "GB";
    }else{
        if(b > 999999){
            //s = QString("%1").arg(b/(1024*1024.0), 6, 'f', 2, QLatin1Char(' ')) + "MB";
            s = QString::number(b/(1024*1024.0), 'f', 2) + "MB";
        }else{
            if(b>999){
                //s = QString("%1").arg(b/1024.0, 6, 'f', 2, QLatin1Char(' ')) + "KB";
                s = QString::number(b/(1024.0), 'f',2) + "KB";
            }else{
                s = b + "B";
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
    long user,nice,sys,idle,iowait,irq,softirq,tt;
    sscanf(ch,"%s%ld%ld%ld%ld%ld%ld%ld",cpu,&user,&nice,&sys,&idle,&iowait,&irq,&softirq);
    tt = user + nice + sys + idle + iowait + irq + softirq;
    file.close();
    QString cusage = "";
    int cp = ((tt-tt0)-(idle-idle0))*100/(tt-tt0);
    if(i>0) cusage = "CPU: " + QString::number(cp) + "%";
    idle0 = idle;
    tt0 = tt;
    // gpu

    QString cmd_text(" #!/bin/bash \n "
                     " ################################################################################## \n "
                     " #    This file is part of System Monitor Gnome extension. \n "
                     " #    System Monitor Gnome extension is free software: you can redistribute it and/or modify \n "
                     " #    it under the terms of the GNU General Public License as published by \n "
                     " #    the Free Software Foundation, either version 3 of the License, or \n "
                     " #    (at your option) any later version. \n "
                     " #    System Monitor Gnome extension is distributed in the hope that it will be useful, \n "
                     " #    but WITHOUT ANY WARRANTY; without even the implied warranty of \n "
                     " #    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \n "
                     " #    GNU General Public License for more details. \n "
                     " #    You should have received a copy of the GNU General Public License \n "
                     " #    along with System Monitor.  If not, see <http://www.gnu.org/licenses/>. \n "
                     " #    Copyright 2017 Fran Glais, David King, indigohedgehog@github. \n "
                     " ################################################################################## \n "
                     "  \n "
                     " ################################## \n "
                     " #                                # \n "
                     " #   Check for GPU memory usage   # \n "
                     " #                                # \n "
                     " ################################## \n "
                     "  \n "
                     " checkcommand()  \n "
                     " {  \n "
                     " 	type $1 > /dev/null 2>&1 \n "
                     " 	return \"$?\" \n "
                     " } \n "
                     "  \n "
                     " # This will print three lines. The first one is the the total vRAM available, \n "
                     " # the second one is the used vRAM and the third on is the GPU usage in %. \n "
                     " if checkcommand nvidia-smi; then \n "
                     " 	nvidia-smi -i 0 --query-gpu=memory.total,memory.used,utilization.gpu --format=csv,noheader,nounits | sed 's%, %\\n%g' \n "
                     " elif checkcommand glxinfo; then \n "
                     " 	TOTALVRAM=\"`glxinfo | grep -A2 -i GL_NVX_gpu_memory_info | egrep -i \"dedicated\" | cut -f2- -d ':' | gawk '{print $1}'`\" \n "
                     " 	AVAILVRAM=\"`glxinfo | grep -A4 -i GL_NVX_gpu_memory_info | egrep -i \"available dedicated\" | cut -f2- -d ':' | gawk '{print $1}'`\" \n "
                     " 	let FREEVRAM=TOTALVRAM-AVAILVRAM \n "
                     " 	echo \"$TOTALVRAM\" \n "
                     " 	echo \"$FREEVRAM\" \n "
                     " fi \n "
                     "  \n ");
    QFile gfile("/tmp/gpu_usage.sh");
    if (!gfile.exists()){
        gfile.open(QIODevice::WriteOnly | QIODevice::Text);
        gfile.write(cmd_text.toUtf8());
        gfile.close();
    }
    QProcess p;
    int gpu_usage = 0;
    int gpu_mem = 0;
    p.start("/bin/bash /tmp/gpu_usage.sh");
    p.waitForFinished();
    QString strResult = p.readAllStandardOutput();
    QStringList list = strResult.split("\n");
    QString gpu_tips("");
    QString gpu_mem_tips("");
    if(list.size() == 4){
        int gpu_mem_total = list[0].toInt();
        int gpu_mem_use = list[1].toInt();
        gpu_mem = gpu_mem_use*100 / gpu_mem_total;
        gpu_usage = list[2].toInt();
        gpu_tips = "GPU: " + QString::number(gpu_usage) + "%";
        gpu_mem_tips = "GPU MEM: " + QString("%1 / %2 = %3").arg(KB(gpu_mem_use*1024)).arg(KB(gpu_mem_total*1024)).arg(QString::number(mp) + "%");
    }else{
        gpu_tips = "GPU detect error.";
    }

    // net
    file.setFileName("/proc/net/dev");
    file.open(QIODevice::ReadOnly);
    l = file.readLine();
    l = file.readLine();
    dbt1 = ubt1 = 0;
    while(!file.atEnd()){
        l = file.readLine();
        QStringList list = l.split(QRegExp("\\s{1,}"));
        db = list.at(1).toLong();
        ub = list.at(9).toLong();
        dbt1 += db;
        ubt1 += ub;
    }
    file.close();
    QString dss = "";
    QString uss = "";
    if (i > 0) {
        long ds = dbt1 - dbt0;
        long us = ubt1 - ubt0;
        dss = NB(ds) + "/s";
        uss = NB(us) + "/s";
        dbt0 = dbt1;
        ubt0 = ubt1;
    }
    QString netspeed = "↑" + uss + "\n↓" + dss;
    QString net = "UPB: " + BS(ubt1) + "  " + uss + "\nDNB: " + BS(dbt1) + "  " + dss;

    i++;
    if (i>2) i = 2;

    // draw
    m_tipsLabel->setText(startup + "\n" + uptime + "\n" + cusage + "\n" + mem + "\n" +gpu_tips + "\n" +gpu_mem_tips + "\n" + net);
    m_centralWidget->text = netspeed;
    m_centralWidget->mp = mp;
    m_centralWidget->cp = cp;
    m_centralWidget->gp = gpu_usage;
    m_centralWidget->gmp = gpu_mem;
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
    if(dialog->exec() == QDialog::Accepted){
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
    if(dialog->exec() == QDialog::Accepted){
        dialog->close();
    }
}
