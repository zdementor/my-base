cd package
echo Binding lua...
tolua++.exe -n MyLuaBind -o ../MyLuaBind.cpp -H ../MyLuaBind.h my.pkg
pause
