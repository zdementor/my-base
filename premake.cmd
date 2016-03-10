del .\*.vcproj
del .\*.sln
del .\*.ncb
del .\*.suo
..\bin\premake --target %1 --os windows
..\build\MyEngine_Sources.sln