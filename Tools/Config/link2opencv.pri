unix{
LIBS += -lopencv_core -lopencv_highgui -lopencv_ml -lopencv_imgproc
}
!unix{
SOURCES += $$PWD/opencv.cpp
}
