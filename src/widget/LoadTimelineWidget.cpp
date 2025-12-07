#include "LoadTimelineWidget.h"

#include <QBrush>
#include <QDebug>
#include <QPainter>
#include <QtMath>

LoadTimelineWidget::LoadTimelineWidget(QWidget *parent)
    : QFrame(parent) {
    setMinimumHeight(180);
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setLineWidth(1);
}

void LoadTimelineWidget::appendSample(const Sample &sample) {
    m_samples.append(sample);
    pruneOutdatedSamples();
    update();
}

void LoadTimelineWidget::setSamples(const QVector<Sample> &samples) {
    m_samples = samples;
    pruneOutdatedSamples();
    update();
}

void LoadTimelineWidget::setTimeWindowSeconds(int seconds) {
    if (seconds <= 0 || seconds == m_timeWindowSeconds) return;
    m_timeWindowSeconds = seconds;
    pruneOutdatedSamples();
    emit timeWindowSecondsChanged(seconds);
    update();
}

void LoadTimelineWidget::setTickIntervalSeconds(int seconds) {
    if (seconds <= 0 || seconds == m_tickIntervalSeconds) return;
    m_tickIntervalSeconds = seconds;
    emit tickIntervalSecondsChanged(seconds);
    update();
}

void LoadTimelineWidget::setLoadMin(double value) {
    if (qFuzzyCompare(value, m_loadMin)) return;
    m_loadMin = value;
    emit loadRangeChanged(m_loadMin, m_loadMax);
    update();
}

void LoadTimelineWidget::setLoadMax(double value) {
    if (qFuzzyCompare(value, m_loadMax)) return;
    m_loadMax = value;
    emit loadRangeChanged(m_loadMin, m_loadMax);
    update();
}

void LoadTimelineWidget::setHighThreshold(double value) {
    if (qFuzzyCompare(value, m_highThreshold)) return;
    m_highThreshold = value;
    emit thresholdChanged(m_mediumThreshold, m_highThreshold);
    update();
}

void LoadTimelineWidget::setMediumThreshold(double value) {
    if (qFuzzyCompare(value, m_mediumThreshold)) return;
    m_mediumThreshold = value;
    emit thresholdChanged(m_mediumThreshold, m_highThreshold);
    update();
}

void LoadTimelineWidget::setGradientStart(const QColor &color) {
    if (color == m_gradientStart) return;
    m_gradientStart = color;
    emit gradientChanged();
    update();
}

void LoadTimelineWidget::setGradientEnd(const QColor &color) {
    if (color == m_gradientEnd) return;
    m_gradientEnd = color;
    emit gradientChanged();
    update();
}

void LoadTimelineWidget::setGridVisible(bool visible) {
    if (visible == m_gridVisible) return;
    m_gridVisible = visible;
    emit gridVisibilityChanged(visible);
    update();
}

void LoadTimelineWidget::setSmoothingEnabled(bool enabled) {
    if (enabled == m_smoothingEnabled) return;
    m_smoothingEnabled = enabled;
    emit smoothingChanged(enabled);
    update();
}

void LoadTimelineWidget::setCurrentValueLabelVisible(bool visible) {
    if (visible == m_currentValueLabelVisible) return;
    m_currentValueLabelVisible = visible;
    emit labelVisibilityChanged(visible);
    update();
}

void LoadTimelineWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const qreal scale = uiScale();

    QRectF area = chartRect();

    // 绘制背景渐变
    QLinearGradient gradient(area.topLeft(), area.bottomLeft());
    gradient.setColorAt(0, m_gradientStart);
    gradient.setColorAt(1, m_gradientEnd);
    painter.fillRect(area, gradient);

    drawThresholdZones(painter, area);
    drawAxis(painter, area, scale);

    QVector<QPointF> points = mapSamplesToPoints();
    if (points.size() < 2) return;

    QPainterPath path = buildPath(points);
    painter.setPen(QPen(QColor(0, 96, 180), 2.0 * scale));
    painter.drawPath(path);

    // 末尾标签
    if (m_currentValueLabelVisible) {
        const QPointF &last = points.constLast();
        double lastValue = m_samples.constLast().loadValue;
        drawCurrentValueLabel(painter, last, lastValue, scale);
    }
}

QRectF LoadTimelineWidget::chartRect() const {
    const qreal margin = scaledMargin();
    const qreal w = qMax<qreal>(0.0, width() - margin * 2);
    const qreal h = qMax<qreal>(0.0, height() - margin * 2);
    return QRectF(margin, margin, w, h);
}

qreal LoadTimelineWidget::uiScale() const {
    // 以设计稿 440x260 为基准进行自适应缩放，避免尺寸变化造成视觉畸变
    const qreal baseWidth = 440.0;
    const qreal baseHeight = 260.0;
    const qreal sx = width() / baseWidth;
    const qreal sy = height() / baseHeight;
    return qBound<qreal>(0.75, qMin(sx, sy), 1.6);
}

qreal LoadTimelineWidget::scaledMargin() const {
    const qreal scale = uiScale();
    return qBound<qreal>(12.0, 20.0 * scale, 32.0);
}

void LoadTimelineWidget::pruneOutdatedSamples() {
    if (m_samples.isEmpty()) return;
    const QDateTime bound = QDateTime::currentDateTimeUtc().addSecs(-m_timeWindowSeconds);

    while (!m_samples.isEmpty() && m_samples.first().timestamp < bound) {
        m_samples.pop_front();
    }
}

QPainterPath LoadTimelineWidget::buildPath(const QVector<QPointF> &points) const {
    QPainterPath path(points.first());
    if (!m_smoothingEnabled) {
        for (int i = 1; i < points.size(); ++i) {
            path.lineTo(points.at(i));
        }
        return path;
    }

    // 简单贝塞尔平滑：使用相邻点的中点作为控制点
    for (int i = 1; i < points.size(); ++i) {
        QPointF prev = points.at(i - 1);
        QPointF current = points.at(i);
        QPointF c1(prev.x() + (current.x() - prev.x()) / 2, prev.y());
        QPointF c2(prev.x() + (current.x() - prev.x()) / 2, current.y());
        path.cubicTo(c1, c2, current);
    }
    return path;
}

QVector<QPointF> LoadTimelineWidget::mapSamplesToPoints() const {
    QVector<QPointF> mapped;
    if (m_samples.isEmpty()) return mapped;

    QRectF area = chartRect();
    const double loadRange = m_loadMax - m_loadMin;
    if (loadRange <= 0) return mapped;

    const QDateTime now = QDateTime::currentDateTimeUtc();

    for (const Sample &sample : m_samples) {
        double secondsDiff = sample.timestamp.msecsTo(now) / 1000.0;
        double ratioX = qBound(0.0, 1.0 - secondsDiff / m_timeWindowSeconds, 1.0);
        double ratioY = (sample.loadValue - m_loadMin) / loadRange;
        ratioY = qBound(0.0, ratioY, 1.0);

        double x = area.left() + ratioX * area.width();
        double y = area.bottom() - ratioY * area.height();
        mapped.append(QPointF(x, y));
    }

    return mapped;
}

QColor LoadTimelineWidget::colorForLoad(double value) const {
    if (value >= m_highThreshold) {
        return QColor(198, 40, 40);
    }
    if (value >= m_mediumThreshold) {
        return QColor(255, 152, 0);
    }
    return QColor(56, 142, 60);
}

void LoadTimelineWidget::drawAxis(QPainter &painter, const QRectF &area, qreal scale) {
    painter.save();
    QPen borderPen(QColor(120, 120, 120), 1.0 * scale);
    painter.setPen(borderPen);
    painter.drawRect(area);

    // 网格与刻度
    if (m_gridVisible) {
        QPen gridPen(QColor(200, 200, 200), 1.0 * scale, Qt::DashLine);
        painter.setPen(gridPen);
        // 横向刻度
        const int tickCount = qMax(2, static_cast<int>(area.width() / (50.0 * scale))); // 基于大约50px一格
        for (int i = 1; i < tickCount; ++i) {
            double x = area.left() + i * (area.width() / tickCount);
            painter.drawLine(QPointF(x, area.top()), QPointF(x, area.bottom()));
        }

        // 纵向刻度
        const int vTicks = 5;
        for (int i = 1; i < vTicks; ++i) {
            double y = area.top() + i * (area.height() / vTicks);
            painter.drawLine(QPointF(area.left(), y), QPointF(area.right(), y));
        }
    }

    // X轴时间刻度文本
    painter.setPen(QPen(QColor(80, 80, 80), 1.0 * scale));
    QFont tickFont = painter.font();
    tickFont.setPixelSize(qMax(8.0, 10.0 * scale));
    painter.setFont(tickFont);

    const int tickCount = qMax(1, m_timeWindowSeconds / m_tickIntervalSeconds);
    for (int i = 0; i <= tickCount; ++i) {
        double ratio = static_cast<double>(i) / tickCount;
        double x = area.left() + (1.0 - ratio) * area.width();
        painter.drawLine(QPointF(x, area.bottom()), QPointF(x, area.bottom() + 5.0 * scale));
        QString label = QString::number(i * m_tickIntervalSeconds) + "s";
        painter.drawText(QPointF(x - 10.0 * scale, area.bottom() + 18.0 * scale), label);
    }

    // Y轴刻度文本
    const int vTicks = 4;
    for (int i = 0; i <= vTicks; ++i) {
        double ratio = static_cast<double>(i) / vTicks;
        double y = area.bottom() - ratio * area.height();
        double value = m_loadMin + ratio * (m_loadMax - m_loadMin);
        painter.drawLine(QPointF(area.left() - 5.0 * scale, y), QPointF(area.left(), y));
        painter.drawText(QPointF(area.left() - 45.0 * scale, y + 4.0 * scale), QString::number(value, 'f', 0));
    }

    painter.restore();
}

void LoadTimelineWidget::drawThresholdZones(QPainter &painter, const QRectF &area) {
    painter.save();

    const double range = m_loadMax - m_loadMin;
    if (range <= 0) {
        painter.restore();
        return;
    }

    auto toY = [&](double value) {
        double ratio = (value - m_loadMin) / range;
        ratio = qBound(0.0, ratio, 1.0);
        return area.bottom() - ratio * area.height();
    };

    const double highY = toY(m_highThreshold);
    const double mediumY = toY(m_mediumThreshold);

    painter.fillRect(QRectF(area.left(), area.top(), area.width(), highY - area.top()), QColor(255, 235, 238, 120));
    painter.fillRect(QRectF(area.left(), highY, area.width(), mediumY - highY), QColor(255, 248, 225, 120));
    painter.fillRect(QRectF(area.left(), mediumY, area.width(), area.bottom() - mediumY), QColor(232, 245, 233, 120));

    painter.restore();
}

void LoadTimelineWidget::drawCurrentValueLabel(QPainter &painter, const QPointF &point, double value, qreal scale) {
    painter.save();
    QString text = QString::number(value, 'f', 1);
    QFont font = painter.font();
    font.setBold(true);
    font.setPixelSize(qMax(10.0, 12.0 * scale));
    painter.setFont(font);

    QColor textColor = colorForLoad(value);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255, 220));
    QRectF rect(point.x() + 8.0 * scale, point.y() - 16.0 * scale, 60.0 * scale, 22.0 * scale);
    painter.drawRoundedRect(rect, 4.0 * scale, 4.0 * scale);

    painter.setPen(QPen(textColor, 1.0 * scale));
    painter.drawText(rect.adjusted(4, 0, -4, 0), Qt::AlignVCenter | Qt::AlignLeft, text);

    painter.setPen(QPen(textColor, 2.0 * scale));
    painter.drawEllipse(point, 4.0 * scale, 4.0 * scale);

    painter.restore();
}

