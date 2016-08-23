del .\*.vcproj
del .\*.sln
del .\*.ncb
del .\*.suo
..\bin\premake4 %1 --os windows
..\build\x32\MyEngine_Sources_x32.sln