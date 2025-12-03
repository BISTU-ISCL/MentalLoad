#pragma once

#include <QDateTime>
#include <QFrame>
#include <QLinearGradient>
#include <QPainterPath>
#include <QVector>

// 心理负荷时间轴控件：用于展示一段时间内的负荷趋势，支持高/中/低分段显示。
class LoadTimelineWidget : public QFrame {
    Q_OBJECT
    // 时间窗口（秒），控制横坐标范围
    Q_PROPERTY(int timeWindowSeconds READ timeWindowSeconds WRITE setTimeWindowSeconds NOTIFY timeWindowSecondsChanged)
    // 刻度间隔（秒），横坐标刻度显示间隔
    Q_PROPERTY(int tickIntervalSeconds READ tickIntervalSeconds WRITE setTickIntervalSeconds NOTIFY tickIntervalSecondsChanged)
    // 纵轴最小值
    Q_PROPERTY(double loadMin READ loadMin WRITE setLoadMin NOTIFY loadRangeChanged)
    // 纵轴最大值
    Q_PROPERTY(double loadMax READ loadMax WRITE setLoadMax NOTIFY loadRangeChanged)
    // 高负荷阈值（超过即显示高负荷颜色）
    Q_PROPERTY(double highThreshold READ highThreshold WRITE setHighThreshold NOTIFY thresholdChanged)
    // 中负荷阈值（超过即显示中负荷颜色）
    Q_PROPERTY(double mediumThreshold READ mediumThreshold WRITE setMediumThreshold NOTIFY thresholdChanged)
    // 背景渐变起始颜色
    Q_PROPERTY(QColor gradientStart READ gradientStart WRITE setGradientStart NOTIFY gradientChanged)
    // 背景渐变结束颜色
    Q_PROPERTY(QColor gradientEnd READ gradientEnd WRITE setGradientEnd NOTIFY gradientChanged)
    // 是否绘制网格
    Q_PROPERTY(bool gridVisible READ gridVisible WRITE setGridVisible NOTIFY gridVisibilityChanged)
    // 是否使用平滑曲线
    Q_PROPERTY(bool smoothingEnabled READ smoothingEnabled WRITE setSmoothingEnabled NOTIFY smoothingChanged)
    // 是否在末尾显示当前值标签
    Q_PROPERTY(bool currentValueLabelVisible READ currentValueLabelVisible WRITE setCurrentValueLabelVisible NOTIFY labelVisibilityChanged)

public:
    explicit LoadTimelineWidget(QWidget *parent = nullptr);

    // 数据结构：时间戳 + 负荷值
    struct Sample {
        QDateTime timestamp;
        double loadValue = 0.0;
    };

    // 数据管理接口
    void appendSample(const Sample &sample);
    void setSamples(const QVector<Sample> &samples);
    QVector<Sample> samples() const { return m_samples; }

    // 属性访问器
    int timeWindowSeconds() const { return m_timeWindowSeconds; }
    int tickIntervalSeconds() const { return m_tickIntervalSeconds; }
    double loadMin() const { return m_loadMin; }
    double loadMax() const { return m_loadMax; }
    double highThreshold() const { return m_highThreshold; }
    double mediumThreshold() const { return m_mediumThreshold; }
    QColor gradientStart() const { return m_gradientStart; }
    QColor gradientEnd() const { return m_gradientEnd; }
    bool gridVisible() const { return m_gridVisible; }
    bool smoothingEnabled() const { return m_smoothingEnabled; }
    bool currentValueLabelVisible() const { return m_currentValueLabelVisible; }

public slots:
    void setTimeWindowSeconds(int seconds);
    void setTickIntervalSeconds(int seconds);
    void setLoadMin(double value);
    void setLoadMax(double value);
    void setHighThreshold(double value);
    void setMediumThreshold(double value);
    void setGradientStart(const QColor &color);
    void setGradientEnd(const QColor &color);
    void setGridVisible(bool visible);
    void setSmoothingEnabled(bool enabled);
    void setCurrentValueLabelVisible(bool visible);

signals:
    void timeWindowSecondsChanged(int value);
    void tickIntervalSecondsChanged(int value);
    void loadRangeChanged(double minValue, double maxValue);
    void thresholdChanged(double medium, double high);
    void gradientChanged();
    void gridVisibilityChanged(bool visible);
    void smoothingChanged(bool enabled);
    void labelVisibilityChanged(bool visible);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QRectF chartRect() const;
    void pruneOutdatedSamples();
    QPainterPath buildPath(const QVector<QPointF> &points) const;
    QVector<QPointF> mapSamplesToPoints() const;
    QColor colorForLoad(double value) const;
    void drawAxis(QPainter &painter, const QRectF &area);
    void drawThresholdZones(QPainter &painter, const QRectF &area);
    void drawCurrentValueLabel(QPainter &painter, const QPointF &point, double value);

    int m_timeWindowSeconds = 60;
    int m_tickIntervalSeconds = 10;
    double m_loadMin = 0.0;
    double m_loadMax = 100.0;
    double m_highThreshold = 80.0;
    double m_mediumThreshold = 50.0;
    QColor m_gradientStart = QColor(240, 248, 255);
    QColor m_gradientEnd = QColor(210, 228, 255);
    bool m_gridVisible = true;
    bool m_smoothingEnabled = true;
    bool m_currentValueLabelVisible = true;

    QVector<Sample> m_samples;
};

