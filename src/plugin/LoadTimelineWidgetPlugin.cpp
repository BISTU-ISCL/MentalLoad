#include "LoadTimelineWidgetPlugin.h"
#include "../widget/LoadTimelineWidget.h"

#include <QtPlugin>

LoadTimelineWidgetPlugin::LoadTimelineWidgetPlugin(QObject *parent)
    : QObject(parent) {}

QIcon LoadTimelineWidgetPlugin::icon() const {
    return QIcon();
}

QString LoadTimelineWidgetPlugin::domXml() const {
    // Qt Designer 内部使用的 XML 描述，定义标签及默认属性
    return R"(<ui language="c++">
     <widget class="LoadTimelineWidget" name="loadTimelineWidget">
      <property name="geometry">
       <rect>
        <x>0</x>
        <y>0</y>
        <width>400</width>
        <height>220</height>
       </rect>
      </property>
      <property name="toolTip" >
       <string>时间轴负荷监视控件</string>
      </property>
      <property name="whatsThis" >
       <string>用于展示高/中/低负荷水平的时间曲线。</string>
      </property>
     </widget>
     <customwidgets>
      <customwidget>
       <class>LoadTimelineWidget</class>
       <extends>QFrame</extends>
       <header>widget/LoadTimelineWidget.h</header>
      </customwidget>
     </customwidgets>
    </ui>)";
}

QString LoadTimelineWidgetPlugin::group() const {
    return QStringLiteral("Mental Load Widgets");
}

QString LoadTimelineWidgetPlugin::includeFile() const {
    return QStringLiteral("LoadTimelineWidget.h");
}

QString LoadTimelineWidgetPlugin::name() const {
    return QStringLiteral("LoadTimelineWidget");
}

QString LoadTimelineWidgetPlugin::toolTip() const {
    return QStringLiteral("展示指定时间窗口内的负荷趋势");
}

QString LoadTimelineWidgetPlugin::whatsThis() const {
    return QStringLiteral("包含高/中/低分段、刻度配置和网格的负荷时间轴控件");
}

QWidget *LoadTimelineWidgetPlugin::createWidget(QWidget *parent) {
    return new LoadTimelineWidget(parent);
}

void LoadTimelineWidgetPlugin::initialize(QDesignerFormEditorInterface *core) {
    if (m_initialized)
        return;

    Q_UNUSED(core);
    m_initialized = true;
}

