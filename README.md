# Mental Load Timeline 控件

一个用于统计并可视化一段时间内用户负荷/压力高、中、低分布的 Qt 控件，包含 Designer 插件与演示程序。控件支持丰富的属性配置，便于在业务中快速调整显示效果。当前配置面向 **Windows / Qt 6.10.0 / MSVC 2022 64bit / Qt Creator 18.0.0** 开发环境。

## 功能摘要
- 时间轴显示：横轴按秒为单位，可配置时间窗口与刻度间隔。
- 负荷分段：高/中/低阈值分区，自定义颜色渐变背景。
- 绘制特性：可选平滑曲线、网格、当前值标签，支持实时追加数据。
- Designer 集成：提供 `LoadTimelineWidgetPlugin`，可加入 Qt Designer 控件面板。
- 示例应用：`mental_load_demo` 展示属性调节与随机数据输入。
- 自适应缩放：根据控件尺寸与设备 DPI 调整字体、线宽与边距，放大缩小时保持观感一致。

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

## 构建与运行（Windows / Qt 6.10.0 / MSVC 2022 64bit）
本仓库提供 CMake 脚本，默认面向 Qt Creator 18.0.0 的 **MSVC 2022 64bit** Kit：

```bash
cmake -S . -B build -G "Ninja" \
  -DCMAKE_PREFIX_PATH="C:/Qt/6.10.0/msvc2022_64" \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

生成内容（Windows 路径示例）：
- `build/LoadTimelineWidget.lib`：控件静态库。
- `build/designer/LoadTimelineWidgetPlugin.dll`：Designer 插件（复制到 `C:/Qt/6.10.0/msvc2022_64/plugins/designer` 后在 Qt Designer 内可见）。
- `build/mental_load_demo.exe`：演示程序。

运行示例：
```powershell
./build/mental_load_demo.exe
```

## Qt Creator 18.0.0 使用提示
- 选择 **MSVC 2022 64bit** Kit（Qt 6.10.0）。
- 打开本项目后，直接构建 `mental_load_demo` 或 `LoadTimelineWidgetPlugin` 目标即可。
- 如果需要在 Designer 中实时测试，构建后将 `designer/LoadTimelineWidgetPlugin.dll` 复制到 Qt 安装目录的 `plugins/designer` 下并重启 Qt Creator。

## Designer 插件说明
- 插件类：`LoadTimelineWidgetPlugin`（分组名：`Mental Load Widgets`）。
- XML 描述已在 `domXml()` 中定义，加载后可在 Designer 侧边栏直接拖拽使用。
- 默认 CMake 已使用 `Qt6::Designer` 链接并对 Windows 输出目录进行了设置，无需额外调整。

## 演示界面说明
演示程序提供以下控制项实时修改控件行为：
- 时间窗口、刻度间隔
- 负荷上下限
- 中/高负荷阈值
- 网格显示、曲线平滑开关
- 定时随机生成高/中/低负荷数据流

## 配置文件示例
可在项目中通过 CMake 引入控件：
- CMake：
  ```cmake
  add_subdirectory(path/to/MentalLoad)
  target_link_libraries(your_app PRIVATE LoadTimelineWidget)
  ```
