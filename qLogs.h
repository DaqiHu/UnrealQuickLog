// Copyright (c) 2024 Daqi Hu. All rights reserved.
#pragma once
#include "Logging/LogMacros.h"

// clang-format off

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

// e.g. AActor::BeginPlay() => <RawMessage> ["\Path\To\Actor.cpp:299"]
#define _DEFAULT_LOG_MESSAGE_RAW_CHAR(RawMessage) "\t%60hs():\t" RawMessage "\t[\"%hs:%d\"]"

// __FUNCTION__, Type: %hs, e.g.: AActor::BeginPlay
// __FILE__,     Type: %hs, e.g.: \Path\To\Actor.cpp
// __LINE__,     Type: %d,  e.g.: 299
// __VA_OPT__(,) (since cpp20): if `...` is empty, leave it empty as well, otherwise add a `,` to separate nearby tokens.
#define _DEFAULT_LOG_ARGUMENTS(...) __FUNCTION__ __VA_OPT__(,) __VA_ARGS__, __FILE__, __LINE__

// Default log message used in `qCheck`, `qVerify` and `qEnsure`.
#define _DEFAULT_ASSERTION_MESSAGE(Condition) "`" #Condition "` is false or null."

// Used in `qNotImpl()`
#define _NOT_IMPL_HINT "Not Implemented!"

///////////////////////////// Log Helpers Internals, DON'T CALL DIRECTLY! //////////////////////////////////////

#define _PrintStringPrefix() \
	UWorld* _GlobalWorld = GWorld->GetWorld(); \
	FString _Prefix; \
	if (_GlobalWorld && _GlobalWorld->WorldType == EWorldType::PIE) \
	{ \
		switch(_GlobalWorld->GetNetMode()) \
		{ \
		case NM_Client: \
			_Prefix = FString::Printf(TEXT("Client %d: "), GPlayInEditorID); \
			break; \
		case NM_DedicatedServer: \
		case NM_ListenServer: \
			_Prefix = FString::Printf(TEXT("Server: ")); \
			break; \
		case NM_Standalone: \
			break; \
		} \
	} \

// Unreal PrintString.
// Hint: Use `GEngine->AddOnScreenDebugMessage` instead of `UKismetSystemLibrary::PrintString`
// to enable print in non-UObjects (they don't have WorldContextObject).
#define _PrintStringImpl(bLog, Color, Format, ...) \
	if (GAreScreenMessagesEnabled && GEngine) \
	{ \
		_PrintStringPrefix(); \
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Color, _Prefix + __VA_OPT__(FString::Printf) (TEXT(Format) __VA_OPT__(,) __VA_ARGS__)); \
	} \

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST) || USE_LOGGING_IN_SHIPPING // Do not Print in Shipping or Test unless explicitly enabled.
#define _PrintStringInner(bLog, Color, Format, ...) _PrintStringImpl(bLog, Color, Format __VA_OPT__(,) __VA_ARGS__)
#else
#define _PrintStringInner(bLog, Color, Format, ...)
#endif

#define _DEFAULT_LOG_MESSAGE(RawMessage) TEXT(_DEFAULT_LOG_MESSAGE_RAW_CHAR(RawMessage))

#define _DEFAULT_ASSERTION_MESSAGE_EMPTY(Condition)

// TODO: DO_ENSURE and CODE_ANALYSIS build configuration check, make them truly the same with UE `ensureMsgf`.
#define _CUSTOM_ENSURE_WRAPPER(Capture, Always, InExpression, InFormat, ...) \
	(LIKELY(!!(InExpression)) || ([Capture] () UE_DEBUG_SECTION \
	{ \
		_PrintStringInner(false, Red, InFormat __VA_OPT__(,) __VA_ARGS__); \
		UE_VALIDATE_FORMAT_STRING(TEXT(InFormat), ##__VA_ARGS__); \
		static std::atomic<bool> bExecuted = false; \
		static constexpr ::UE::Assert::Private::FStaticEnsureRecord ENSURE_Static(TEXT(InFormat), #InExpression, __builtin_FILE(), __builtin_LINE(), Always); \
		if ((Always || !bExecuted.load(std::memory_order_relaxed)) && FPlatformMisc::IsEnsureAllowed() && ::UE::Assert::Private::EnsureFailed(bExecuted, &ENSURE_Static, ##__VA_ARGS__)) \
		{ \
			PLATFORM_BREAK(); \
		} \
		return false; \
	} ()))

// Unreal Assertions
#define _LogCheck(Condition, Format, ...)		checkf(Condition,		_DEFAULT_LOG_MESSAGE(Format), _DEFAULT_LOG_ARGUMENTS(__VA_ARGS__))
#define _LogCheckSlow(Condition, Format, ...)	checkfSlow(Condition,	_DEFAULT_LOG_MESSAGE(Format), _DEFAULT_LOG_ARGUMENTS(__VA_ARGS__))
#define _LogVerify(Condition, Format, ...)		verifyf(Condition,		_DEFAULT_LOG_MESSAGE(Format), _DEFAULT_LOG_ARGUMENTS(__VA_ARGS__))

// TODO: `__FUNCTION__` still prints the inner lambda name. How to fix it?
// Note: __FUNCTION__ in _CUSTOM_ENSURE_WRAPPER must be passed by argument rather than literal string concatenation, or it will not compile in clang compiler. 
#define _LogEnsure(Condition, Format, ...) \
	_CUSTOM_ENSURE_WRAPPER( \
		&, \
		false, \
		Condition, \
		"%hs(): " Format " [\"" __FILE__ ":" PREPROCESSOR_TO_STRING(__LINE__) "\"]", __FUNCTION__ __VA_OPT__(,) \
		__VA_ARGS__ \
	)

#define _CheckInner(Condition, Format, ...)		_LogCheck(Condition, Format __VA_OPT__(,) __VA_ARGS__)
#define _VerifyInner(Condition, Format, ...)	_LogVerify(Condition, Format __VA_OPT__(,) __VA_ARGS__)
#define _EnsureInner(Condition, Format, ...)	_LogEnsure(Condition, Format __VA_OPT__(,) __VA_ARGS__)

// Unreal Logs
#define _LogVeryVerbose(Format, ...)	UE_LOG(_DEFAULT_LOG_CATEGORY, VeryVerbose,	_DEFAULT_LOG_MESSAGE(Format), _DEFAULT_LOG_ARGUMENTS(__VA_ARGS__))
#define _LogVerbose(Format, ...)		UE_LOG(_DEFAULT_LOG_CATEGORY, Verbose,		_DEFAULT_LOG_MESSAGE(Format), _DEFAULT_LOG_ARGUMENTS(__VA_ARGS__))
#define _LogLog(Format, ...)			UE_LOG(_DEFAULT_LOG_CATEGORY, Log,			_DEFAULT_LOG_MESSAGE(Format), _DEFAULT_LOG_ARGUMENTS(__VA_ARGS__))
#define _LogDisplay(Format, ...)		UE_LOG(_DEFAULT_LOG_CATEGORY, Display,		_DEFAULT_LOG_MESSAGE(Format), _DEFAULT_LOG_ARGUMENTS(__VA_ARGS__))
#define _LogWarning(Format, ...)		UE_LOG(_DEFAULT_LOG_CATEGORY, Warning,		_DEFAULT_LOG_MESSAGE(Format), _DEFAULT_LOG_ARGUMENTS(__VA_ARGS__))
#define _LogError(Format, ...)			UE_LOG(_DEFAULT_LOG_CATEGORY, Error,		_DEFAULT_LOG_MESSAGE(Format), _DEFAULT_LOG_ARGUMENTS(__VA_ARGS__))
#define _LogFatal(Format, ...)			UE_LOG(_DEFAULT_LOG_CATEGORY, Fatal,		_DEFAULT_LOG_MESSAGE(Format), _DEFAULT_LOG_ARGUMENTS(__VA_ARGS__))

///////////////////////////// Unreal Assertions Interface //////////////////////////////////////

/**
 * Performs runtime assertion.
 * This will crash the editor and game when condition is failed.
 * Condition statement will be ignored in shipping builds.
 */
#define qCheck(Expr, ...)	_CheckInner(Expr,  _DEFAULT_ASSERTION_MESSAGE##__VA_OPT__(_EMPTY)(Expr) __VA_ARGS__)

/**
 * Performs runtime assertion.
 * This will crash the editor and game when condition is failed.
 * Condition statement will be kept in shipping builds.
 */
#define qVerify(Expr, ...)	_VerifyInner(Expr, _DEFAULT_ASSERTION_MESSAGE##__VA_OPT__(_EMPTY)(Expr) __VA_ARGS__)

/**
 * Performs runtime assertion.
 * This will keep the editor and game running when condition is failed and only executed once.
 */
#define qEnsure(Expr, ...)	_EnsureInner(Expr, _DEFAULT_ASSERTION_MESSAGE##__VA_OPT__(_EMPTY)(Expr) __VA_ARGS__)

///////////////////////////// Unreal Log Interface //////////////////////////////////////

/** Default log helper, which you can choose the verbosity level. */
#define qLog(Verbosity, Fmt, ...) _Log##Verbosity(Fmt __VA_OPT__(,) __VA_ARGS__)

///////////////////////////// Unreal PrintString Interface //////////////////////////////////////

/** Behave the same as `UKismetSystemLibrary::PrintString`, it only prints logs to the screen without logging to file. */
#define qPrintString(Fmt, ...) _PrintStringInner(true, Green, Fmt __VA_OPT__(,) __VA_ARGS__)

///////////////////////////// Quick logs //////////////////////////////////////

// TODO: Impl a counter (static int Counter_FILE_LINE = 0;) and increment every call, finally output every 2 seconds with [+128] suffix to avoid flushing the terminal.

/** Log error and print red color. Consider using it for unexpected errors. */
#define qErr(Fmt, ...) \
	_PrintStringInner(false, Red, Fmt __VA_OPT__(,) __VA_ARGS__); \
	_LogError(Fmt __VA_OPT__(,) __VA_ARGS__)

/** Log warning and print yellow color. Consider using it for exceptions that not severe, or programmatic hacks that need to be refactored. */
#define qWarn(Fmt, ...) \
	_PrintStringInner(false, Yellow, Fmt __VA_OPT__(,) __VA_ARGS__); \
	_LogWarning(Fmt __VA_OPT__(,) __VA_ARGS__)

/** Log display and print green color. */
#define qPrint(Fmt, ...) \
	_PrintStringInner(false, Green, Fmt __VA_OPT__(,) __VA_ARGS__); \
	_LogDisplay(Fmt __VA_OPT__(,) __VA_ARGS__)

///////////////////////////// Log Not Implemented Interface //////////////////////////////////////

/** Behave the same as `qErr`.
 * Helps make code compile while leaving a hint that this implementation should be handled ASAP.
 * Consider using regex expressions to replace the leading brace `{` with `{ qNotImpl();`, or hooking script after IDE generating function definition.
 */
#define qNotImpl() \
	_PrintStringInner(false, Red, "%hs(): " _NOT_IMPL_HINT " [\"" __FILE__ ":" PREPROCESSOR_TO_STRING(__LINE__) "\"]", __FUNCTION__); \
	_LogError(_NOT_IMPL_HINT)

///////////////////////////// Formatter helpers //////////////////////////////////////

/** Bool to String. */
#define qB2S(bVar) (bVar ? TEXT("true") : TEXT("false"))

// TODO: Use template SFINAE to auto deduce type to support TArrays, FGameplayTagContainers, etc. And ensure type safety.

///////////////////////////// Change default log category //////////////////////////////////////

/** TODO: Currently you cannot re-define a macros inside a macros body. So we need to define them manually in each .cpp file you want to change.
 * e.g.:
 * #include "UnrealQuickLog/qLogs.h"
 *
 * #define _DEFAULT_LOG_CATEGORY NewLogCategory (after log header)
 *
 * ... several lines ...
 *
 * #define _DEFAULT_LOG_CATEGORY DefaultLogCategory (at the end of file)
 *
 * Because of the existence of Unity Build, multiple .cpp file may be combined in preprocessing stage.
 * Your changing may flee to other .cpp file and affect log category in their logging.
 * So `#undef _DEFAULT_LOG_CATEGORY` is not a good idea, because that will make the macros undeclared due to the same reason in Unity Build.
 * 
 * We have a problem for now is that the macros `PRAGMA_DISABLE_MACRO_REDEFINED_WARNINGS` itself will also flee to other .cpp files,
 * which may lead every .cpp file ignore redefine macro warnings. However, currently we don't have any better choice but to accept this.
 * 
 * Real work is always dirty work.
 */
// #define CHANGE_LOG_CATEGORY(LogCategoryName) \
// 	#define _DEFAULT_LOG_CATEGORY LogCategoryName
//
// PRAGMA_ENABLE_MACRO_REDEFINED_WARNINGS

///////////////////////////// Quick Early Out //////////////////////////////////////

/**
 * Creates a local variable and check if it is null.
 * You need to add a log message, and log verbosity is `Error`.
 * Note that this is not a recommended style in C++, please use with caution.
 * TODO: Reimplement this with template function and chain-call.
 */
#define qNullableRet(Var, Val, DefaultRet, Fmt, ...) \
	auto Var = (Val); \
	/* Use `Var == nullptr` rather than `!Var` to try to avoid unexpected user-defined implicit conversion (e.g.: TSharedPtr, TUniquePtr, TWeakInterfacePtr). */ \
	if (Var == nullptr) \
	{ \
		qErr(Fmt __VA_OPT__(,) __VA_ARGS__); \
		return DefaultRet; \
	} \

/** Creates a local variable and check if it is null, return value is `void`. */
#define qNullable(Var, Val, Fmt, ...)			qNullableRet(Var, Val, , Fmt __VA_OPT__(,) __VA_ARGS__)

/** Creates a local variable and check if it is null, return value is `nullptr` (for any pointer type). */
#define qNullableRetNull(Var, Val, Fmt, ...)	qNullableRet(Var, Val, nullptr, Fmt __VA_OPT__(,) __VA_ARGS__)

/** Creates a local variable and check if it is null, return value is `false` (for booleans). */
#define qNullableRetFalse(Var, Val, Fmt, ...)	qNullableRet(Var, Val, false, Fmt __VA_OPT__(,) __VA_ARGS__)

/** Creates a local variable and check if it is null, return value is `0` (0, 0L, 0.0f, 0.0). */
#define qNullableRetZero(Var, Val, Fmt, ...)	qNullableRet(Var, Val, 0, Fmt __VA_OPT__(,) __VA_ARGS__)

/** Creates a local variable and check if it is null, return value is `-1` (-1, -1L, -1.0f, -1.0). */
#define qNullableRetNeg1(Var, Val, Fmt, ...)	qNullableRet(Var, Val, -1, Fmt __VA_OPT__(,) __VA_ARGS__)

// clang-format on
