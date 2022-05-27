# CameraStageController

## Dependencies
Need EDSDK API from developers canon. Europe link: https://developers.canon-europe.com/s/ <br>
Need pre-built opencv binaries.

Add ````EDSDK_64```` and ````EDSDK```` to CameraStageController folder: <br>
````
CameraStageController\EDSDK_64
CameraStageController\EDSDK
````

Link LIBS and DLL files in ````CameraStageController.pro```` file. <br>

Example:
````qmake
INCLUDEPATH += C:\OpenCV-4.5.5\opencv\\build\include

LIBS += $$PWD\EDSDK_64\Library\EDSDK.lib
dll_files.files += $$PWD\EDSDK_64\Dll\EDSDK.dll
dll_files.files += $$PWD\EDSDK_64\Dll\EdsImage.dll

CONFIG(debug, debug|release) {
  LIBS += C:\OpenCV-4.5.5\opencv\build\x64\vc15\lib\opencv_world455d.lib
  dll_files.files += C:\OpenCV-4.5.5\opencv\build\x64\vc15\bin\opencv_world455d.dll
  OUTDIR = debug
  CONFIG += console
}

CONFIG(release, debug|release) {
  LIBS += C:\OpenCV-4.5.5\opencv\build\x64\vc15\lib\opencv_world455.lib
  dll_files.files += C:\OpenCV-4.5.5\opencv\build\x64\vc15\bin\opencv_world455.dll
  OUTDIR = release
}
````
