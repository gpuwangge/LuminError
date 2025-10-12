# Build
```
mkdir build  
cd build
cmake -G "MinGW Makefiles" ..
cmake -E time make -j
```

# Run
```
cd build/bin  
```
Run SimpleTriangle:  
```
.\launcher.exe .\SimpleTriangle.dll
```
Run all tests:
```
.\autotest.bat
```