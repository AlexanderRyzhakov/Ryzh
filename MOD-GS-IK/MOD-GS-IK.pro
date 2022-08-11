QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

QMAKE_CXXFLAGS += -O3

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    analisator.cpp \
    file_manager.cpp \
    main.cpp \
    mainwindow.cpp \
    msu_gs.cpp \
    win_graphic.cpp

HEADERS += \
    analisator.h \
    check_fft.hpp \
    copy_array.hpp \
    error_handling.hpp \
    fft.h \
    fft.hpp \
    fft_impl.hpp \
    fft_settings.h \
    file_manager.h \
    global_consts.h \
    mainwindow.h \
    msu_gs.h \
    spline.h \
    win_graphic.h

FORMS += \
    mainwindow.ui \
    win_graphic.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
