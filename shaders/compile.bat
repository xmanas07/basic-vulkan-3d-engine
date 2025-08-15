cd /d "%~dp0"
echo Compiling shaders...

rem Loop through all .vert files
for %%f in (*.vert) do (
    echo  - %%f
    C:\VulkanSDK\1.4.321.1\Bin\glslc.exe "%%f" -o "%%f.spv"
)

rem Loop through all .frag files
for %%f in (*.frag) do (
    echo  - %%f
    C:\VulkanSDK\1.4.321.1\Bin\glslc.exe "%%f" -o "%%f.spv"
)

echo Done!