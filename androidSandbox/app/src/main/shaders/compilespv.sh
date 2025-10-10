#!/usr/bin/bash

if [ -z "$1" ]; then
    search_dir="*"
else
    search_dir="$1"
fi



count=0

for entry in ${search_dir}/*.vert
do
    if [ -e "$entry" ]; then
        count=$((count+1))
        echo Compile ${entry}
        glslc.exe ${entry} -o ${entry}.spv
    fi
done
for entry in ${search_dir}/*.frag
do
    if [ -e "$entry" ]; then
        count=$((count+1))
        echo Compile ${entry}
        glslc.exe ${entry} -o ${entry}.spv
    fi
done
for entry in ${search_dir}/*.comp
do
    if [ -e "$entry" ]; then
        count=$((count+1))
        echo Compile ${entry}
        glslc.exe ${entry} -o ${entry}.spv
    fi
done

echo Compiling finishes. Total compiled: ${count}

read

# *** What does this script do ***
# Compile all .vert files within the shaders/ folder into .spv files
# Compile all .frag files within the shaders/ folder into .spv files
# Compile all .comp files within the shaders/ folder into .spv files

# *** Below are .bat script ***
# @echo off
# for /d /r %%i in (*) do (
#     if exist %%i/shader.vert (%VULKAN_SDK%/Bin/glslc.exe %%i/shader.vert -o %%i/vert.spv)
#     if exist %%i/shader.frag (%VULKAN_SDK%/Bin/glslc.exe %%i/shader.frag -o %%i/frag.spv)
#     if exist %%i/shader.comp (%VULKAN_SDK%/Bin/glslc.exe %%i/shader.comp -o %%i/comp.spv)
#     if exist %%i/shaderTranspose.comp (%VULKAN_SDK%/Bin/glslc.exe %%i/shaderTranspose.comp -o %%i/compTranspose.spv)
#     if exist %%i/shaderOptimizeMatMul.comp (%VULKAN_SDK%/Bin/glslc.exe %%i/shaderOptimizeMatMul.comp -o %%i/compOptimizeMatMul.spv)
# )

