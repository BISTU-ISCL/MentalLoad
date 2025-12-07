# Mental Load Timeline 控件

一个用于统计并可视化一段时间内用户负荷/压力高、中、低分布的 Qt 控件，包含 Designer 插件与演示程序。控件支持丰富的属性配置，便于在业务中快速调整显示效果。

## 功能摘要
- 时间轴显示：横轴按秒为单位，可配置时间窗口与刻度间隔。
- 负荷分段：高/中/低阈值分区，自定义颜色渐变背景。
- 绘制特性：可选平滑曲线、网格、当前值标签，支持实时追加数据。
- Designer 集成：提供 `LoadTimelineWidgetPlugin`，可加入 Qt Designer 控件面板。
- 示例应用：`mental_load_demo` 展示属性调节与随机数据输入。
- 自适应缩放：根据控件尺寸调整字体、线宽与边距，放大缩小时保持观感一致。

## 主要属性（`LoadTimelineWidget`）
| 属性 | 说明 | 默认值 |
| --- | --- | --- |
| `timeWindowSeconds` | 时间窗口长度（秒），控制横轴范围 | 60 |
| `tickIntervalSeconds` | 横轴刻度间隔（秒） | 10 |
| `loadMin` / `loadMax` | 纵轴负荷范围 | 0 / 100 |
| `mediumThreshold` | 中负荷阈值 | 50 |
| `highThreshold` | 高负荷阈值 | 80 |
| `gradientStart` / `gradientEnd` | 背景渐变起止色 | `#F0F8FF` / `#D2E4FF` |
| `gridVisible` | 是否显示网格 | `true` |
| `smoothingEnabled` | 是否使用平滑曲线 | `true` |
| `currentValueLabelVisible` | 末尾是否显示当前值标签 | `true` |

数据接口：
- `appendSample(const Sample &sample)` 追加单个采样点（UTC 时间戳 + 负荷值）。
- `setSamples(const QVector<Sample> &samples)` 批量设置数据。

## 构建与运行
本仓库提供 CMake 构建脚本，已针对 **Qt 5.15.2**（Qt Designer 同版本）设置：

```bash
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt/5.15.2/gcc_64
cmake --build .
```

生成内容：
- `libLoadTimelineWidget.a`：控件静态库。
- `designer/libLoadTimelineWidgetPlugin.so`：Designer 插件（将该文件放入 Qt 5.15.2 安装的 `plugins/designer` 目录即可在 Qt Designer 中使用）。
- `mental_load_demo`：演示程序。

运行示例：
```bash
./mental_load_demo
```

## Designer 插件说明
- 插件类：`LoadTimelineWidgetPlugin`（分组名：`Mental Load Widgets`）。
- XML 描述已在 `domXml()` 中定义，加载后可在 Designer 侧边栏直接拖拽使用。
- 默认 CMake 已使用 `Qt5::Designer` 链接，直接针对 Qt 5.15.2 构建，无需额外调整。

## 演示界面说明
演示程序提供以下控制项实时修改控件行为：
- 时间窗口、刻度间隔
- 负荷上下限
- 中/高负荷阈值
- 网格显示、曲线平滑开关
- 定时随机生成高/中/低负荷数据流

## 配置文件示例
可在项目中通过 `.cmake` 或 `.pri` 引入控件：
- CMake：
  ```cmake
  add_subdirectory(path/to/MentalLoad)
  target_link_libraries(your_app PRIVATE LoadTimelineWidget)
  ```
- qmake：
  ```qmake
  INCLUDEPATH += $$PWD/src
  HEADERS += $$PWD/src/widget/LoadTimelineWidget.h
  SOURCES += $$PWD/src/widget/LoadTimelineWidget.cpp
  ```

