# Log-Console
Windows Command Line Log Injection

## Requierments:
Microsoft Detours Library - https://github.com/microsoft/Detours

**Compile:**
1. Unzip source code, open command line and enter to source directory
2. SET DETOURS_TARGET_PROCESSOR=X64
3. C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat
4. NMAKE

Add detours.lib to Linker additional libraries.

**Hooked Functions:**
- WriteConsoleW <br>
- ReadConsoleW <br>

Write file to tmp folder, Process using CSRSS (Inter Process Communiction) not trigger consoleReadW(), 
but command line still captured.
