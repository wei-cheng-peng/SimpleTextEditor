# 简单文本编辑器（Qt 实验二）

Qt Widgets 写的多窗口文本编辑器，界面用 designer 搭，功能包括文件读写、字体/颜色、
自动换行、显示行号、工具栏与状态栏开关、查找替换、撤销重做、修改标记与定时自动保存。

## 编译

用 qmake 直接构建（Qt 6.9.3 + MinGW 13.1）：

```
mkdir build && cd build
qmake ../SimpleTextEditor/SimpleTextEditor.pro
mingw32-make -j4
```

也可以直接用 Qt Creator 打开 `SimpleTextEditor.pro`，选择 MinGW 64-bit 套件后构建运行。

## 目录

- `mainwindow.ui/.cpp/.h`：主窗口、菜单与工具栏、文件与编辑逻辑
- `codeeditor.h/.cpp`：从 QPlainTextEdit 派生的编辑控件，带行号栏与当前行高亮
- `finddialog.*` / `replacedialog.*`：非模态的查找、替换对话框
- `aboutdialog.*`：关于对话框
