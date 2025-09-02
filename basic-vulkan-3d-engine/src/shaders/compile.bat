cd /d "%~dp0"
echo Compiling shaders...

rem Loop through all .vert files
for %%f in (*.vert) do (
    echo %%f
    C:\VulkanSDK\1.4.321.1\Bin\glslc.exe "%%f" -o "output-files\%%f.spv" || exit /b 1
)

rem Loop through all .frag files
for %%f in (*.frag) do (
    echo %%f
    C:\VulkanSDK\1.4.321.1\Bin\glslc.exe "%%f" -o "output-files\%%f.spv" || exit /b 1
)

echo Done!
exit /b 0