#!/usr/bin/bash

# Remove all .spv files within the shaders/ folder

count=0
search_dir=SimpleTriangle
for entry in */*.spv
do
    count=$((count+1))
    echo Remove ${entry}
    rm ${entry}
done
echo Total removed: ${count}

read

# *** Below are .bat script ***
#@echo off
#for /d /r %%i in (*) do (
#    if exist %%i\vert.spv (del %%i\vert.spv)
#    if exist %%i\frag.spv (del %%i\frag.spv)
#    if exist %%i\comp.spv (del %%i\comp.spv)
#)