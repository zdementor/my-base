cd package
echo Binding lua...
..\..\..\..\bin\tolua++.exe -n MyLuaBind -o ../MyLuaBind.cpp -H ../MyLuaBind.h my.pkg
pause
