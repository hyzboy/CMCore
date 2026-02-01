@echo off
setlocal enabledelayedexpansion

echo ========================================
echo Heisenbug 诊断测试套件
echo ========================================
echo.

set "TEST_EXE=e:\ULRE\build\out\Windows_64_Release\BidirectionalMapStressTest.exe"
set "LOG_DIR=e:\ULRE\heisenbug_logs"

REM 创建日志目录
if not exist "%LOG_DIR%" mkdir "%LOG_DIR%"

echo [1/3] 测试 Release 版本（无详细日志）...
echo.
"%TEST_EXE%" > "%LOG_DIR%\release_no_verbose.log" 2>&1
set "RESULT1=%ERRORLEVEL%"
if %RESULT1% EQU 0 (
    echo   ✓ Release 无详细日志: PASSED
) else (
    echo   ✗ Release 无详细日志: FAILED ^(exit code: %RESULT1%^)
    echo   查看日志: %LOG_DIR%\release_no_verbose.log
)
echo.

echo [2/3] 编译 Debug 版本...
cmake --build e:\ULRE\build --config Debug --target BidirectionalMapStressTest > "%LOG_DIR%\debug_build.log" 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo   ✗ Debug 编译失败
    goto :summary
)
echo   ✓ Debug 编译成功
echo.

echo [3/3] 测试 Debug 版本（无详细日志）...
set "DEBUG_EXE=e:\ULRE\build\out\Windows_64_Debug\BidirectionalMapStressTest.exe"
"%DEBUG_EXE%" > "%LOG_DIR%\debug_no_verbose.log" 2>&1
set "RESULT2=%ERRORLEVEL%"
if %RESULT2% EQU 0 (
    echo   ✓ Debug 无详细日志: PASSED
) else (
    echo   ✗ Debug 无详细日志: FAILED ^(exit code: %RESULT2%^)
    echo   查看日志: %LOG_DIR%\debug_no_verbose.log
)
echo.

:summary
echo ========================================
echo 诊断结果总结
echo ========================================
echo.
echo Release (无详细日志): 
if %RESULT1% EQU 0 (
    echo   状态: PASSED ✓
) else (
    echo   状态: FAILED ✗ ^(exit code: %RESULT1%^)
)
echo.
echo Debug (无详细日志):
if %RESULT2% EQU 0 (
    echo   状态: PASSED ✓
) else (
    echo   状态: FAILED ✗ ^(exit code: %RESULT2%^)
)
echo.
echo 日志文件位置: %LOG_DIR%
echo.

REM 分析结果
echo ========================================
echo 问题分析
echo ========================================
echo.

if %RESULT1% NEQ 0 if %RESULT2% NEQ 0 (
    echo 结论: Debug 和 Release 都崩溃
    echo 可能原因:
    echo   - BidirectionalMap 存在逻辑错误
    echo   - 验证代码 ^(VerifyState^) 导致的问题
    echo.
    echo 建议:
    echo   1. 关闭 kVerifyEachOp，只保留最终验证
    echo   2. 使用 AddressSanitizer 检测内存问题
    echo   3. 检查 BidirectionalMap.h 的边界检查
) else if %RESULT1% NEQ 0 if %RESULT2% EQU 0 (
    echo 结论: Release 崩溃，Debug 正常
    echo 可能原因:
    echo   - 编译器优化导致的问题
    echo   - 可能存在未定义行为 ^(UB^)
    echo   - 数组越界在 Debug 模式被掩盖
    echo.
    echo 建议:
    echo   1. 使用 /Od 禁用优化重新编译 Release
    echo   2. 使用 AddressSanitizer
    echo   3. 检查是否有未初始化的变量
    echo   4. 在关键位置添加内存屏障
) else if %RESULT1% EQU 0 if %RESULT2% NEQ 0 (
    echo 结论: Release 正常，Debug 崩溃 ^(罕见情况^)
    echo 可能原因:
    echo   - Debug 模式的断言或检查触发
    echo   - 栈检查或运行时检查导致
    echo.
    echo 建议:
    echo   1. 检查 Debug 日志中的断言失败
    echo   2. 检查是否栈溢出
) else (
    echo 结论: Debug 和 Release 都正常
    echo.
    echo 说明: 关闭详细日志后问题消失了！
    echo.
    echo 可能原因:
    echo   - 之前的崩溃可能是偶发的
    echo   - 或者是因为输出重定向的问题
    echo   - 建议多次运行确认稳定性
)

echo.
echo ========================================
pause
