#!/usr/bin/bash

if [ -z "$1" ]; then
    search_dir="*"
else
    search_dir="$1"
fi

pass=0
error=0
skipped=0

#--------------------------------------------------
# Return 0 if any directly included file is newer
# than the output SPV.
#--------------------------------------------------
include_newer() {

    local shader="$1"
    local output="$2"

    local shader_dir
    shader_dir="$(dirname "$shader")"

    local line
    local include
    local include_path

    while IFS= read -r line
    do
        # 跳过不是 #include 的行
        [[ "$line" != *"#include"* ]] && continue

        # 提取引号里的路径
        include="${line#*\"}"
        include="${include%%\"*}"

        [ -z "$include" ] && continue

        include_path="$shader_dir/$include"

        if [ -f "$include_path" ] && [ "$include_path" -nt "$output" ]; then
            return 0
        fi

    done < "$shader"

    return 1
}

compile_if_newer() {

    local source_file="$1"
    local output_file="${source_file}.spv"

    if [ ! -f "$output_file" ]; then

        echo "📦 Compile ${source_file} (new)"

        ./glslc.exe \
            --target-env=vulkan1.2 \
            --target-spv=spv1.4 \
            "$source_file" \
            -o "$output_file"

        if [ $? -ne 0 ]; then
            error=$((error+1))
        else
            pass=$((pass+1))
        fi

        return 0

    elif [ "$source_file" -nt "$output_file" ] || \
         include_newer "$source_file" "$output_file"; then

        echo "🔄 Compile ${source_file} (updated)"

        ./glslc.exe \
            --target-env=vulkan1.2 \
            --target-spv=spv1.4 \
            "$source_file" \
            -o "$output_file"

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

for ext in vert frag comp rgen rmiss rchit rahit rint
do
    for entry in ${search_dir}/*.${ext}
    do
        if [ -e "$entry" ]; then
            compile_if_newer "$entry"
        fi
    done
done

total=$((pass+error+skipped))

echo ""
echo "📊 Compilation summary:"
echo "   Error: ${error}/${total}"
echo "   Passed: ${pass}/${total}"
echo "   Skipped: ${skipped}/${total}"

echo ""
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

