# UnrealQuickLog

[English](./README.md)

UnrealQuickLog 是一个轻量级日志宏库，适用于 [Unreal Engine](https://www.unrealengine.com/) 项目。你可以使用这些宏来替代 `UE_LOG`、`check`、`ensure` 等，以提高效率和多功能性。

例如，如果你想记录一条错误信息 `This is an error message!` 并在屏幕上用红色打印，带有函数和类信息，你需要在 Unreal Engine 中这样写：

```cpp
// Unreal Engine
UE_LOG(LogTemp, Error, TEXT("%hs(): This is an error message!, [%hs:%d]"), __FUNCTION__, __FILE__, __LINE__);
UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%hs(): This is an error message!, [%hs:%d]"), __FUNCTION__, __FILE__, __LINE__), true, false, FLinearColor::Red);
```

输出将会是：

```
AActor::BeginPlay(): This is an error message! [E:/GitHub/UnrealEngine/Source/Runtime/Engine/Private/Actor.cpp:4216]
```

现在使用 UnrealQuickLog，你只需要这样写：

```cpp
// UnrealQuickLog
qErr("This is an error message!");
```

## 快速开始

你可以通过 [Git 子模块 (submodule)](https://git-scm.com/book/zh/v2/Git-工具-子模块)（**推荐**）或源代码来安装 UnrealQuickLog。

### 通过 Git 子模块 (submodule) 安装

假设你在游戏项目 `MyGame.uproject` 的根目录下，并且游戏项目已经初始化了 Git。

```shell
git submodule add https://github.com/DaqiHu/UnrealQuickLog.git Source/MyGame/UnrealQuickLog
git submodule update --init --recursive
```

如果你以前没有使用过子模块，一个 `.gitmodules` 文件会在你的根目录下生成。

现在你可以提交一次提交到项目的主分支。

```shell
git commit -m "add UnrealQuickLog"
```

全部完成！你可以通过右键单击 `MyGame.uproject` 并选择 `Generate Visual Studio project files` 来重新生成项目文件，以在你的 IDE 中看到更改。

UnrealQuickLog 的所有源文件都会被 Git 自动跟踪，你将自动收到更新。

### 使用 Git 子模块安装到源代码构建的 Unreal Engine 中

如果你使用 [Unreal Engine 源代码](github.com/EpicGames/UnrealEngine)，你可以将 UnrealQuickLog 添加到你的引擎仓库中。这将确保每个从该引擎创建的新项目都可以自动使用此库，即使是引擎本身（如果你需要在断点无法工作的情况下调试源代码，例如多线程）。

像往常一样，在引擎的 Git 根目录中打开终端。

```shell
git submodule add https://github.com/DaqiHu/UnrealQuickLog.git Engine/Source/Runtime/Core/Public/UnrealQuickLog -f
git submodule update --init --recursive
```

> 使用 `-f` 标志来抑制由于 `.gitignore` 引起的 Git 警告。此操作不会像 `git push -f` 那样刷新你的仓库。请放心使用。

因为这个仓库仅包含宏，所以对引擎 `Core` 模块的导出符号没有任何修改。因此不会导致引擎进行全面重建。

### 通过源代码安装（不会收到更新）

```shell
git clone https://github.com/DaqiHu/UnrealQuickLog.git
```

克隆此项目，并将 `qLogs.h` 放置到你喜欢的任何目录中，无论是在 Unreal Engine 源代码仓库中还是在你的游戏项目仓库中。

## 用法

UnrealQuickLog 宏有几类。

- **快速日志**：标准日志记录的最小日志包装器（`qLog`）
- **屏幕打印**：记录到控制台并打印到编辑器屏幕（`qErr`、`qWarn`、`qPrint`）
- **断言**：将非格式和格式断言结合在一起（`qCheck`、`qEnsure`）
- **空值**：用于提前退出的助手，灵感来自 C# 空引用（`qNullable`、`qNullableRet`）
- **未实现**：使项目可以编译，同时留下消息（`qNotImpl`）
- **转换助手**：从值快速转换为字符串文本（`qB2S`）