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
.\SimpleTriangle.exe
```
Run all tests:
```
.\autotest.bat
```