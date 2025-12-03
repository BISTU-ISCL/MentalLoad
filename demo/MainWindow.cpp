#include "MainWindow.h"

#include <QCheckBox>
#include <QDateTime>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QRandomGenerator>
#include <QSpinBox>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);

    // 控件实例
    m_widget = new LoadTimelineWidget(this);
    layout->addWidget(m_widget);

    // 配置区
    QFormLayout *form = new QFormLayout();

    m_timeWindowSpin = new QSpinBox(this);
    m_timeWindowSpin->setRange(10, 600);
    m_timeWindowSpin->setValue(m_widget->timeWindowSeconds());
    connect(m_timeWindowSpin, QOverload<int>::of(&QSpinBox::valueChanged), m_widget, &LoadTimelineWidget::setTimeWindowSeconds);
    form->addRow("时间窗口(秒)", m_timeWindowSpin);

    m_tickIntervalSpin = new QSpinBox(this);
    m_tickIntervalSpin->setRange(1, 120);
    m_tickIntervalSpin->setValue(m_widget->tickIntervalSeconds());
    connect(m_tickIntervalSpin, QOverload<int>::of(&QSpinBox::valueChanged), m_widget, &LoadTimelineWidget::setTickIntervalSeconds);
    form->addRow("刻度间隔(秒)", m_tickIntervalSpin);

    m_loadMinSpin = new QDoubleSpinBox(this);
    m_loadMinSpin->setRange(-1000, 1000);
    m_loadMinSpin->setValue(m_widget->loadMin());
    connect(m_loadMinSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), m_widget, &LoadTimelineWidget::setLoadMin);
    form->addRow("负荷下限", m_loadMinSpin);

    m_loadMaxSpin = new QDoubleSpinBox(this);
    m_loadMaxSpin->setRange(-1000, 1000);
    m_loadMaxSpin->setValue(m_widget->loadMax());
    connect(m_loadMaxSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), m_widget, &LoadTimelineWidget::setLoadMax);
    form->addRow("负荷上限", m_loadMaxSpin);

    m_mediumSpin = new QDoubleSpinBox(this);
    m_mediumSpin->setRange(-1000, 1000);
    m_mediumSpin->setValue(m_widget->mediumThreshold());
    connect(m_mediumSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), m_widget, &LoadTimelineWidget::setMediumThreshold);
    form->addRow("中负荷阈值", m_mediumSpin);

    m_highSpin = new QDoubleSpinBox(this);
    m_highSpin->setRange(-1000, 1000);
    m_highSpin->setValue(m_widget->highThreshold());
    connect(m_highSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), m_widget, &LoadTimelineWidget::setHighThreshold);
    form->addRow("高负荷阈值", m_highSpin);

    m_gridCheck = new QCheckBox("显示网格", this);
    m_gridCheck->setChecked(m_widget->gridVisible());
    connect(m_gridCheck, &QCheckBox::toggled, m_widget, &LoadTimelineWidget::setGridVisible);
    form->addRow(m_gridCheck);

    m_smoothCheck = new QCheckBox("平滑曲线", this);
    m_smoothCheck->setChecked(m_widget->smoothingEnabled());
    connect(m_smoothCheck, &QCheckBox::toggled, m_widget, &LoadTimelineWidget::setSmoothingEnabled);
    form->addRow(m_smoothCheck);

    layout->addLayout(form);
    setCentralWidget(central);
    resize(720, 420);

    // 定时生成样例数据
    connect(&m_timer, &QTimer::timeout, this, &MainWindow::handleAddSample);
    m_timer.start(1000);
}

void MainWindow::handleAddSample() {
    // 生成假数据：基于随机数模拟高/中/低波动
    double base = QRandomGenerator::global()->bounded(m_widget->loadMin(), m_widget->loadMax());
    double jitter = QRandomGenerator::global()->bounded(-5.0, 5.0);

    LoadTimelineWidget::Sample sample;
    sample.timestamp = QDateTime::currentDateTimeUtc();
    sample.loadValue = base + jitter;

    m_widget->appendSample(sample);
}

