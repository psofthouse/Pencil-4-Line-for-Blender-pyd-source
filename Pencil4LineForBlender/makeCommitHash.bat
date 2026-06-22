for /f "usebackq tokens=*" %%i in (`git log -n 1 --pretty^=format:"%%H" -- . ../blender ../cereal ../pybind11 ../pencil4line_for_blender_mac`) do @set hash=%%i
echo %hash% > commitHash.txt
echo #define GIT_COMMIT_HASH "%hash%" > commitHash.h
