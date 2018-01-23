RMDIR /S /Q temp
RMDIR /S /Q Lib
RMDIR /S /Q Obj
RMDIR /S /Q Exec\Debug
RMDIR /S /Q Exec\Release
RMDIR /S /Q ipch
RMDIR /S /Q pch
RMDIR /S /Q shader\Debug
RMDIR /S /Q shader\Release
RMDIR /S /Q shader\InternalShader\Debug
RMDIR /S /Q shader\InternalShader\Release
del /S /Q "NOISE 3D.VC.db"
del /S /Q "shader\InternalShader.VC.db"
RMDIR /S /Q "ExternalLib\FBXSDK\lib\x86\debug\"
RMDIR /S /Q "ExternalLib\FBXSDK\lib\x86\release\"