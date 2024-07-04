# UnrealQuickLog

[简体中文](./README_zh-CN.md)

UnrealQuickLog is a light log macros library for [Unreal Engine](https://www.unrealengine.com/) projects. You can use these macros to replace the use of `UE_LOG`, `check`, `ensure`, etc, for efficiency and versatility.

For example, if you want to log an error message `This is an error message!` and print to screen as well with red color, with function and class info, you need to write like this in Unreal Engine:

```cpp
// Before
UE_LOG(LogTemp, Error, TEXT("%hs(): This is an error message!, [%hs:%d]"), __FUNCTION__, __FILE__, __LINE__);
UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%hs(): This is an error message!, [%hs:%d]"), __FUNCTION__, __FILE__, __LINE__), true, false, FLinearColor::Red);
```

The output will be:

```
AActor::BeginPlay(): This is an error message! [E:/GitHub/UnrealEngine/Source/Runtime/Engine/Private/Actor.cpp:4216]
```

Now with UnrealQuickLog, you only need this:

```cpp
// After
qErr("This is an error message!");
```

## Quick Start

You can install UnrealQuickLog through [Git submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules) (**recommanded**) or source code.

### Install with Git submodule

Assume you are in the root directory of your game project `MyGame.uproject`, and the game project is already initialized with Git.

```shell
git submodule add https://github.com/DaqiHu/UnrealQuickLog.git Source/MyGame/UnrealQuickLog
git submodule update --init --recursive
```

If you haven't used any submodule before, a `.gitmodules` file is generated under your root directory.

Now you can perform a commit to your project's main branch.

```shell
git commit -m "add UnrealQuickLog"
```

All done! You can regenerate project files by right click `MyGame.uproject` and choose `Generate Visual Studio project files` to see the changes in your IDE.

All source files of UnrealQuickLog are tracked automatically by Git, and you will receive updates automatically.

### Install to source build Unreal Engine with Git submodule

If you are using [Unreal Engine from source code](github.com/EpicGames/UnrealEngine), You can add UnrealQuickLog to your engine repo. This will ensure every new project from this engine can use this library automatically, even the engine itself (if you need to debug the source code when breakpoints won't work, e.g. multithreading).

As usual, open your terminal in engine's Git root directory.

```shell
git submodule add https://github.com/DaqiHu/UnrealQuickLog.git Engine/Source/Runtime/Core/Public/UnrealQuickLog -f
git submodule update --init --recursive
```

>  Use the `-f` flag to suppress Git warnings due to `.gitignore`. This operation will not flush your repo like `git push -f`. Please rest assured to use.

Because of this repo only contains macros, there is no modification to exported symbols of engine `Core` module. So it will not cause the engine to full-rebuild.

### Install with source code (will not receive updates)

```shell
git clone https://github.com/DaqiHu/UnrealQuickLog.git
```

Clone this project, and place `qLogs.h` to any directory you like in either Unreal Engine source repo or your game's project repo.

## Usage

There are several categories of UnrealQuickLog macros.

- **Quick-log**: minimal log wrapper for standard logging (`qLog`)
- **Print-to-screen**: log to console while print to editor screen (`qErr`, `qWarn`, `qPrint`)
- **Assertion**: combine non-format and format assertions together (`qCheck`, `qEnsure`)
- **Nullable**: helpers for early out, inspired by C# nullable reference (`qNullable`, `qNullableRet`)
- **Not-implemented**: make the project can compile while leaving a message (`qNotImpl`)
- **Cast-helper**: quick cast from value to string literal (`qB2S`)

### Quick-log



### Print-to-screen



### Assertion



### Nullable



### Not-implemented



### Cast-helper



## Change default log category



## API reference



## FAQ



## Contact

Please create issues if you need help! Pull requests are welcomed!
