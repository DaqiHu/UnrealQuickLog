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

**Syntax**: 

```cpp
qLog(Verbosity, Format, ...Args);
```

where

- `Verbosity`: one of `Fatal`, `Error`, `Warning`, `Display`, `Log`, `Verbose`, `VeryVerbose`
- `Format`: `const char*` type string (C++ string literal), e.g. `"Hello world"`
- `Args` (optional): provide format argument for `%s`, `%d`, `%f`, ... in `Format` message

**Example**: 

```cpp
qLog(Verbose, "Initialize component");
qLog(Fatal, "AssetManager [%s] can't be null! The editor will crash!", *this->GetName());
```

**Explanation**:

Quick-log macros are minimal wrapper of `UE_LOG`. 

For example, if we want a `Verbose` verbosity log with format, we can write like this:

```cpp
qLog(Verbose, "Format message %s", *this->GetName());
```

It's equivalent to:

```cpp
UE_LOG(LogTemp, Verbose, TEXT("%hs(): Format message %s [%hs:%d]"), __FUNCTION__, *this->GetName(), __FILE__, __LINE__);
```

As you can see, `qLog` automatically add `__FUNCTION__`, `__FILE__` and `__LINE__` for you and place them and format arguments in a user friendly way. So the only thing you need to care about is the log message itself and log verbosity. 

### Print-to-screen

**Syntax**: 

```cpp
qErr(Format, ...Args);
qWarn(Format, ...Args);
qPrint(Format, ...Args);
```

where

- `Format`: `const char*` type string (C++ string literal), e.g. `"Hello world"`
- `Args` (optional): provide format argument for `%s`, `%d`, `%f`, ... in `Format` message

**Example**: 

```cpp
qErr("An error message.");
qWarn("Input mapping context [%s] is forced loaded from disk.", *ObjectPath->ToString());
qPrint("Control Rot: Roll=%.2f, Pitch=%.2f, Yaw=%.2f", Rotator.Roll, Rotator.Pitch, Rotator.Yaw);
```

**Explanation**:

Since Quick-log category already handles all the `UE_LOG` cases, now we can add more functionality to the most frequently used log verbosities: `Error`, `Warning` and `Display` (aka. log, default, info).

Firstly, we need to review the definition of those verbosities.

- `Error`: This level of log will not crash the editor or game, but they shouldn't happen ever once. This may be caused by logic errors, syntax errors, casting errors, coding mistakens, etc.
- `Warning`: This level of log will not crash the editor or game, and do no harm to the logic itself. But the implementation is ill-formed, hard coded, force loaded, hacked from memory, etc.
- `Display`: No errors and warnings. We simply want to know what happened inside the function.



### Assertion

**Syntax**: 

```cpp
qCheck(Condition, Format, ...Args);
qEnsure(Condition, Format, ...Args);
qVerify(Condition, Format, ...Args);
```

where

- `Condition`: The condition to test. If false, assertion will cause the game crash
- `Format` (optional): `const char*` type string (C++ string literal), e.g. `"Hello world"`
- `Args` (optional): provide format argument for `%s`, `%d`, `%f`, ... in `Format` message

**Example**: 

```cpp
qCheck(GetWorld());
qEnsure(Actor != nullptr, "Actor is null!");
if (qEnsure(Cast<ACharacter>(GetOuter()), "outer [%s] is not a character", *GetOuter()->GetName())) { }
```

**Explanation**:



### Nullable

**Syntax**: 

```cpp
qNullable(VarName, InitValue, Format, ...Args);
qNullableRetNull(VarName, InitValue, Format, ...Args);
qNullableRetZero(VarName, InitValue, Format, ...Args);
qNullableRetNeg1(VarName, InitValue, Format, ...Args);
qNullableRetFalse(VarName, InitValue, Format, ...Args);
qNullableRet(VarName, InitValue, ReturnValue, Format, ...Args);
```

where

- `VarName`:
- `InitValue`:
- `ReturnValue` (optional):
- `Format` (optional): `const char*` type string (C++ string literal), e.g. `"Hello world"`
- `Args` (optional): provide format argument for `%s`, `%d`, `%f`, ... in `Format` message

**Example**: 

```cpp
// TODO. 
```

**Explanation**:



### Not-implemented

**Syntax**: 

```cpp
qNotImpl();
```

No parameter, no return type.

**Example**: 

```cpp
qNotImpl();
```

**Explanation**:

### Cast-helper

This section is under maintainance. Current we only have `qB2S`:

// TODO.

## Change default log category

Maybe you notice that the log category seems to always be `LogTemp`. This can be changed by re-defining `_DEFAULT_LOG_CATEGORY` macro.

```cpp
// MyActor.cpp
#include "UnrealQuickLog/qLogs.h"

#define _DEFAULT_LOG_CATEGORY LogMyGame

void AMyActor::Func()
{
    qLog(Verbose, "Format message %s", *this->GetName());
}

#define _DEFAULT_LOG_CATEGORY LogTemp
```

Now this time the `qLog` expression is equivalent to:

```cpp
UE_LOG(LogMyGame, Verbose, TEXT("%hs(): Format message %s [%hs:%d]"), __FUNCTION__, *this->GetName(), __FILE__, __LINE__);
```

Note that you should always `#define` it back to `LogTemp` at the end of file. Unreal Engine enables Unity Build by default. To put that simple, it combines multiple `.cpp` files together and compile them once. So if you don't define the log category back to default, it may affect logs in other file.

## API reference

// TODO.

## FAQ

### Do I really need to redefine the log category macro?

Not actually.

In my real-project practice, all logs in a file usually use the same log category, such as `MyGameMode` in gamemodes and player states,  `MyGameAnimation` in anim instances `MyGameAbilitySystem` in attributes. I prefer to define the log category at the top of code instead of write each of them manually in log macro body.

However, re-definition of macros will cause compiler warning, and they are treated as error in Unreal Engine 5.4 and higher version. So I added a pragma in `qLogs.h` header.

```cpp
// Unreal Engine doesn't have these warning suppress macros for MSVC. 
#ifndef PRAGMA_DISABLE_MACRO_REDEFINED_WARNINGS
#define PRAGMA_DISABLE_MACRO_REDEFINED_WARNINGS \
	__pragma(warning(push)) \
	__pragma(warning(disable: 4005)) // '_DEFAULT_LOG_CATEGORY': macro redefinition
#endif

#ifndef PRAGMA_ENABLE_MACRO_REDEFINED_WARNINGS
#define PRAGMA_ENABLE_MACRO_REDEFINED_WARNINGS \
	__pragma(warning(pop))
#endif

// Apply the compiler pragma.
// Please note that this will influence every file where this header is introduced.
PRAGMA_DISABLE_MACRO_REDEFINED_WARNINGS

///////////////////////////// Configurations //////////////////////////////////////

// Default category used in `UE_LOG`.
#define _DEFAULT_LOG_CATEGORY LogTemp
```

Obviously, you can avoid macro re-definition by undefine it first, then define it.

Change this 

```cpp
#define _DEFAULT_LOG_CATEGORY LogMyGame
// Some code ...
#define _DEFAULT_LOG_CATEGORY LogTemp
```

to this

```cpp
#define _DEFAULT_LOG_CATEGORY LogMyGame
// Some code ...
#undef _DEFAULT_LOG_CATEGORY
#define _DEFAULT_LOG_CATEGORY LogTemp
```

Now you can comment the pragma line to disable it:

```cpp
// PRAGMA_DISABLE_MACRO_REDEFINED_WARNINGS
```

## Contact

Please create issues if you need help! Pull requests are welcomed!
