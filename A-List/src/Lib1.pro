QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    author.cpp \
    book.cpp \
    dialogaddbook.cpp \
    dialogchangelibraryname.cpp \
    file_read_write.cpp \
    global_library.cpp \
    image_drawer.cpp \
    lib.cpp \
    library_main_class.cpp \
    main.cpp \
    string_process.cpp \
    tests.cpp \
    win_delete_book.cpp \
    win_dialog_save_if_edited.cpp

HEADERS += \
    author.h \
    book.h \
    dialogaddbook.h \
    dialogchangelibraryname.h \
    file_read_write.h \
    global_library.h \
    image_drawer.h \
    lib.h \
    library_main_class.h \
    string_process.h \
    tests.h \
    win_delete_book.h \
    win_dialog_save_if_edited.h

FORMS += \
    dialogaddbook.ui \
    dialogchangelibraryname.ui \
    lib.ui \
    win_delete_book.ui \
    win_dialog_save_if_edited.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
