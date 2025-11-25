@echo off
echo Dang bien dich cac file C...

gcc binary.c -o binary
if %errorlevel% neq 0 goto error

gcc huffman.c -o huffman
if %errorlevel% neq 0 goto error

gcc floyd.c -o floyd
if %errorlevel% neq 0 goto error

echo ------------------------------------------
echo BIEN DICH THANH CONG! (Binary, Huffman, Floyd)
echo ------------------------------------------
pause
exit /b

:error
echo ------------------------------------------
echo CO LOI XAY RA KHI BIEN DICH!
echo ------------------------------------------
pause