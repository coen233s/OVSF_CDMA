@echo off
del /F /Q *.out 2> nul
del /F /Q *.diff*.* 2> nul
del /F /Q *.tick*.txt 2> nul

set v=variable_dynamic
call:RunTest
set v=fixed_once
set opts=-f -o
call:RunTest

goto FINISH

:RunTest
for /L %%t in (1,1,6) do (
	echo Release\TestCdma.exe %opts% < test%%t.txt
	Release\TestCdma.exe %opts% < test%%t.txt > nul
	for /L %%s in (2,1,5) do (
		echo ts %%t.%%s
	    if exist MobileStation.%%s.tick.txt (
			move MobileStation.%%s.tick.txt MobileStation.%%t.%%s.tick.%v%.txt
		)
		
		for /F %%f in ('dir /B *Station.%%s.out') do (
			if exist %%f (
				echo Comparing *Station.%%s.in %%f
				fc /B *Station.%%s.in %%f > %%t.%%s.diff.%v%.txt
			)
		)
	)
)
goto:EOF

:FINISH

type *.diff.txt
pause
type *tick.*
pause