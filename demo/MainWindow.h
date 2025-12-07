#pragma once

#include <QMainWindow>
#include <QTimer>
#include "../src/widget/LoadTimelineWidget.h"

class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;

// 示例窗口：提供交互控制以演示控件属性和数据更新。
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void handleAddSample();

private:
    LoadTimelineWidget *m_widget = nullptr;
    QTimer m_timer;
    QSpinBox *m_timeWindowSpin = nullptr;
    QSpinBox *m_tickIntervalSpin = nullptr;
    QDoubleSpinBox *m_highSpin = nullptr;
    QDoubleSpinBox *m_mediumSpin = nullptr;
    QDoubleSpinBox *m_loadMaxSpin = nullptr;
    QDoubleSpinBox *m_loadMinSpin = nullptr;
    QCheckBox *m_gridCheck = nullptr;
    QCheckBox *m_smoothCheck = nullptr;
};

