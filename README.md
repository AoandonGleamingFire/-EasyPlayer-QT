# EasyPlayer-Qt 简易视频播放器

这是一个基于 **Qt 6** 开发的轻量级视频播放器，实现了基础的视频加载、播放控制（播放/暂停、进度跳转、音量调节）以及界面交互功能。

开发环境
* **框架**: Qt 6.7.x (LTS 推荐)
* **编译器**: MinGW 64-bit (推荐 11.2.0 或以上)
* **模块依赖**: 
    * `widgets` (界面组件)
    * `multimedia` (多媒体核心)
    * `multimediawidgets` (视频渲染窗口)

##  项目结构
项目仅包含运行所需的 C++ 核心源码：
* `EasyPlayer.pro` - Qt 项目配置文件（定义模块与路径）
* `main.cpp` - 程序启动入口
* `mainwindow.h` / `mainwindow.cpp` - 播放器主窗体逻辑实现
* `mainwindow.ui` - XML 格式的界面布局文件

##  如何运行
1. **安装 Qt**: 确保已安装 Qt Creator 并在安装时勾选了 `Qt Multimedia` 组件。
2. **打开项目**: 使用 Qt Creator 菜单 `文件` -> `打开文件或项目`，选择 `EasyPlayer.pro`。
3. **配置 Kit**: 在弹出的 "Configure Project" 页面，勾选你本地的 MinGW 编译套件。
4. **编译运行**: 
   * 点击左下角的 **绿色三角形按钮 (Run)** 或按下 `Ctrl + R`。
   * 如果提示找不到头文件，请执行 `构建` -> `清理所有项目`，然后重新构建。

##  注意事项
* **解码器**: 如果播放视频只有声音没有画面，请确保你的 Windows 系统安装了基础解码器（如 K-Lite Codec Pack），或者在 Linux/macOS 下安装了对应的 gstreamer/ffmpeg 插件。
* **路径问题**: 建议将项目放在不含中文、不含空格的磁盘路径下，以避免 MinGW 编译报错。

##  许可证
本项目仅供学习与交流使用。
