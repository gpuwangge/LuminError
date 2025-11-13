#!/usr/bin/bash

if [ -z "$1" ]; then
    search_dir="*"
else
    search_dir="$1"
fi

pass=0
error=0
skipped=0

compile_if_newer() {
    local source_file="$1"
    local output_file="${source_file}.spv"
    
    if [ ! -f "$output_file" ]; then
        echo "📦 Compile ${source_file} (new)"
        glslc.exe "${source_file}" -o "${output_file}"
        if [ $? -ne 0 ]; then
            error=$((error+1))
        else
            pass=$((pass+1))
        fi
        return 0
    elif [ "$source_file" -nt "$output_file" ] || \
        [ "Common/constants.glsl" -nt "$output_file" ] || \
        [ "Common/graphicsGlobalUBO.glsl" -nt "$output_file" ] || \
        [ "Common/lightsUBO.glsl" -nt "$output_file" ] || \
        [ "Common/objectUBO.glsl" -nt "$output_file" ]; then
        echo "🔄 Compile ${source_file} (updated)"
        glslc.exe "${source_file}" -o "${output_file}"
        if [ $? -ne 0 ]; then
            error=$((error+1))
        else
            pass=$((pass+1))
        fi
        return 0
    else
        skipped=$((skipped+1))
        echo "✅ Skip ${source_file} (up to date)"
        return 1
    fi
}

echo "🔍 Scanning for shaders in: ${search_dir}"

for entry in ${search_dir}/*.vert
do
    if [ -e "$entry" ]; then
        compile_if_newer "$entry"
    fi
done

for entry in ${search_dir}/*.frag
do
    if [ -e "$entry" ]; then
        compile_if_newer "$entry"
    fi
done

for entry in ${search_dir}/*.comp
do
    if [ -e "$entry" ]; then
        compile_if_newer "$entry"
    fi
done

total=$((pass+error+skipped))

echo ""
echo "📊 Compilation summary:"
echo "   Error: ${error}/${total}"
echo "   Passed: ${pass}/${total}"
echo "   Skipped: ${skipped}/${total}"
echo "📊 Press ENTER to exit..."

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

