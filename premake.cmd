del .\*.vcproj
del .\*.sln
del .\*.ncb
del .\*.suo
..\bin\premake --target vs2008 --os windows
..\build\MyEngine_Sources.sln