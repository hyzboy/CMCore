@echo off
REM Run all 12 BidirectionalMap stress tests
setlocal enabledelayedexpansion

set "TEST_DIR=e:\ULRE\bin"
set "TOTAL=12"
set "PASSED=0"
set "FAILED=0"

echo ========================================
echo Running BidirectionalMap Stress Tests
echo ========================================
echo.

for /L %%i in (1,1,12) do (
    set "NUM=0%%i"
    set "NUM=!NUM:~-2!"
    
    echo [!NUM!/12] Running Stress Test !NUM!...
    
    if %%i==1 set "TEST=BidirectionalMapStressTest_01_SequentialAdd.exe"
    if %%i==2 set "TEST=BidirectionalMapStressTest_02_SequentialDelete.exe"
    if %%i==3 set "TEST=BidirectionalMapStressTest_03_SequentialChange.exe"
    if %%i==4 set "TEST=BidirectionalMapStressTest_04_RandomOperations.exe"
    if %%i==5 set "TEST=BidirectionalMapStressTest_05_AlternatingAddDelete.exe"
    if %%i==6 set "TEST=BidirectionalMapStressTest_06_BatchOperations.exe"
    if %%i==7 set "TEST=BidirectionalMapStressTest_07_QueryHeavy.exe"
    if %%i==8 set "TEST=BidirectionalMapStressTest_08_ChangeOrAdd.exe"
    if %%i==9 set "TEST=BidirectionalMapStressTest_09_BasicConsistency.exe"
    if %%i==10 set "TEST=BidirectionalMapStressTest_10_IteratorPerformance.exe"
    if %%i==11 set "TEST=BidirectionalMapStressTest_11_DeletePattern.exe"
    if %%i==12 set "TEST=BidirectionalMapStressTest_12_DenseRandom.exe"
    
    "%TEST_DIR%\!TEST!"
    
    if !ERRORLEVEL! EQU 0 (
        echo   ✓ PASSED
        set /A PASSED+=1
    ) else (
        echo   ✗ FAILED (exit code: !ERRORLEVEL!)
        set /A FAILED+=1
    )
    echo.
)

echo ========================================
echo Test Summary
echo ========================================
echo Total Tests:  %TOTAL%
echo Passed:       %PASSED%
echo Failed:       %FAILED%
echo ========================================

if %FAILED% GTR 0 (
    exit /b 1
) else (
    exit /b 0
)
