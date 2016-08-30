cd package
echo Binding lua...
..\..\..\..\bin\x32\tolua++.exe -n MyLuaBind -o ../MyLuaBind.cpp -H ../MyLuaBind.h my.pkg
pause
