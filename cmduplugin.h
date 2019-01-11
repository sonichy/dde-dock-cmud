#ifndef CMDUPlugin_H
#define CMDUPlugin_H

#include "dde-dock/pluginsiteminterface.h"
#include "cmduwidget.h"
#include <QTimer>
#include <QLabel>

class CMDUPlugin : public QObject, PluginsItemInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginsItemInterface)
    Q_PLUGIN_METADATA(IID "com.deepin.dock.PluginsItemInterface" FILE "cmdu.json")

public:
    explicit CMDUPlugin(QObject *parent = 0);

    const QString pluginName() const override;
    const QString pluginDisplayName() const override;
    void init(PluginProxyInterface *proxyInter) override;

    void pluginStateSwitched() override;
    bool pluginIsAllowDisable() override { return true; }
    bool pluginIsDisable() override;

    int itemSortKey(const QString &itemKey);
    void setSortKey(const QString &itemKey, const int order);

    QWidget *itemWidget(const QString &itemKey) override;
    QWidget *itemTipsWidget(const QString &itemKey) override;

    const QString itemCommand(const QString &itemKey) override;
    const QString itemContextMenu(const QString &itemKey) override;

    void invokedMenuItem(const QString &itemKey, const QString &menuId, const bool checked) override;

private slots:
    void updateCMDU();

private:
    long int i, db, ub, dbt, ubt, dbt1, ubt1, dbt0, ubt0, tt0, idle0;
    QPointer<CMDUWidget> m_centralWidget;
    QPointer<QLabel> m_tipsLabel;
    QTimer *m_refershTimer;
    QSettings m_settings;
    QString KB(long k);
    QString BS(long b);
    QString NB(long b);
    QString startup;
    //QLabel *labelStartupDuration;
    void about();
    void changelog();
    void bootRecord();
    void bootAnalyze();

};

#endif // CMDUPlugin_H
