del .\*.vcproj
del .\*.sln
del .\*.ncb
del .\*.suo
..\bin\premake4 %1
..\bin\premake4 %1 arch64
..\build\x32\MyEngine_Sources.sln