unix{
LIBS += -lopencv_core -lopencv_highgui -lopencv_ml -lopencv_imgproc
}
!unix{
    SOURCES += $$PWD/opencv.cpp
#    OPENCV_INCLUDE_PATH = $(OPENCV_INCLUDE_PATH)
#    OPENCV_LIB_PATH = $(OPENCV_LIB_PATH)
#    !isEmpty(OPENCV_INCLUDE_PATH) {
#        INCLUDEPATH += $(OPENCV_INCLUDE_PATH)
#    }
#    !isEmpty(OPENCV_LIB_PATH) {
#        LIBS += -L$(OPENCV_LIB_PATH)

#}
}
