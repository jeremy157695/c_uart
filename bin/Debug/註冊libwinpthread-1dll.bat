@echo �}�l���U
copy libwinpthread-1.dll %windir%\system32\
regsvr32 %windir%\system32\libwinpthread-1.dll /s
@echo libwinpthread-1.dll�}�l���U
@pause