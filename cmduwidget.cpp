#include "cmduwidget.h"
#include "constants.h"
#include <QApplication>
#include <QPainter>
#include <QDebug>
#include <QSvgRenderer>
#include <QMouseEvent>

#define PLUGIN_STATE_KEY    "enable"

CMDUWidget::CMDUWidget(QWidget *parent)
    : QWidget(parent),
    m_settings("deepin", "dde-dock-cmdu")
{
    text = "↑0.00B/s\n↓0.00B/s";
    mp = 0;
}

bool CMDUWidget::enabled()
{
    return m_settings.value(PLUGIN_STATE_KEY, true).toBool();
}

void CMDUWidget::setEnabled(const bool b)
{
    m_settings.setValue(PLUGIN_STATE_KEY, b);
}

QSize CMDUWidget::sizeHint() const
{
    QFontMetrics FM(qApp->font());
    return FM.boundingRect("↑000.00KB/s  ").size() + QSize(0,FM.boundingRect("↓000.00KB/s  ").height());
}

void CMDUWidget::resizeEvent(QResizeEvent *e)
{    
    QWidget::resizeEvent(e);
}

void CMDUWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    if(mp>=90){
        painter.setBrush(Qt::red);
        painter.drawRect(rect());
    }
    painter.setPen(Qt::white);
    painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, text);
}

void CMDUWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() != Qt::RightButton)
        return QWidget::mousePressEvent(e);

    const QPoint p(e->pos() - rect().center());
    if (p.manhattanLength() < std::min(width(), height()) * 0.8 * 0.5)
    {
        emit requestContextMenu();
        return;
    }

    QWidget::mousePressEvent(e);
}
