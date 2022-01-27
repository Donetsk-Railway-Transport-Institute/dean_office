#-------------------------------------------------
#
# Project created by QtCreator 2019-12-09T11:18:21
#
#-------------------------------------------------
QT += core gui xml xmlpatterns printsupport
QT += multimedia multimediawidgets network qml
QT += axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = recognition
TEMPLATE = app

CONFIG += release

QMAKE_LFLAGS_RELEASE += -static -static-libgcc


# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        recognition.cpp \
        recognition_window.cpp \
        startup_data.cpp \
        xml_read_write.cpp

HEADERS += \
        recognition_window.h \
        startup_data.h \
        xml_read_write.h

FORMS += \
        recognition_window.ui

RESOURCES += edit_report.qrc


CONFIG(debug, debug|release) {
    DESTDIR = $${PWD}/build/debug
    FILE_FROM = $${DESTDIR}/$${TARGET}.exe
    FILE_TO = $${PWD}/bin/debug/$${TARGET}.exe
}

CONFIG(release, debug|release) {
    DESTDIR = $${PWD}/build/release
    FILE_FROM = $${DESTDIR}/$${TARGET}.exe
    FILE_TO = $${PWD}/bin/release/$${TARGET}.exe
}

win32{
    DESTDIR ~= s,/,\\,g
    FILE_FROM ~= s,/,\\,g
    FILE_TO ~= s,/,\\,g
    QMAKE_POST_LINK += $$quote(cmd /c copy /y $${FILE_FROM} $${FILE_TO}$$escape_expand(\\n\\t))
}
