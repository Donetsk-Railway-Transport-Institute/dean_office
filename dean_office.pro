#-------------------------------------------------
#
# Project created by QtCreator 2019-12-09T11:18:21
#
#-------------------------------------------------
QT       += core gui xml xmlpatterns printsupport
QT       += multimedia multimediawidgets network qml
QT       += axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dean_office
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
        JSHighLighter.cpp \
        choice_group_dialog.cpp \
        edit_groups_dialog.cpp \
        edit_specialization_dialog.cpp \
        edit_specialty_dialog.cpp \
        edu_activity_dialog.cpp \
        find_dialog.cpp \
        main.cpp \
        order_recognition.cpp \
        other_activity_dialog.cpp \
        portfolio_dialog.cpp \
        prev_edu_dialog.cpp \
        print_preview.cpp \
        startup_data.cpp \
        students_table_widget.cpp \
        transfer_to_next_year.cpp \
        tt_widget.cpp \
        deanofficewindow.cpp \
        xml_read_write.cpp \
        camera.cpp \
        imagesettings.cpp \
        videosettings.cpp \
        qftp.cpp \
        qurlinfo.cpp \
        ftp_archiving_dlg.cpp \
        HTMLHighLighter.cpp \
        report_editor.cpp

HEADERS += \
        JSHighLighter.h \
        choice_group_dialog.h \
        edit_groups_dialog.h \
        edit_specialization_dialog.h \
        edit_specialty_dialog.h \
        edu_activity_dialog.h \
        find_dialog.h \
        order_recognition.h \
        other_activity_dialog.h \
        portfolio_dialog.h \
        prev_edu_dialog.h \
        print_preview.h \
        startup_data.h \
        students_table_widget.h \
        transfer_to_next_year.h \
        tt_widget.h \
        deanofficewindow.h \
        title_data.h \
        xml_read_write.h \
        camera.h \
        imagesettings.h \
        videosettings.h \
        qftp.h \
        qurlinfo.h \
        ftp_archiving_dlg.h \
        HTMLHighLighter.h \
        report_editor.h

FORMS += \
        deanofficewindow.ui \
        recognition_window.ui \
        user_password.ui \
        Portfolio_Dialog.ui \
        camera.ui \
        imagesettings.ui \
        videosettings.ui

RESOURCES += edit_report.qrc

CONFIG(debug, debug|release) {
    DESTDIR = $${PWD}/build/debug
    FILE_FROM = $${DESTDIR}/$${TARGET}.exe
    FILE_TO = $${PWD}/bin/debug/$${TARGET}.exe
}

RELEASE_DIR = D:/EDIT/Qt/dean_office_data

CONFIG(release, debug|release) {
    DESTDIR = $${PWD}/build/release
    FILE_FROM = $${DESTDIR}/$${TARGET}.exe
    FILE_TO = $${RELEASE_DIR}/$${TARGET}.exe
}

win32{
    DESTDIR ~= s,/,\\,g
    FILE_FROM ~= s,/,\\,g
    FILE_TO ~= s,/,\\,g
    QMAKE_POST_LINK += $$quote(cmd /c copy /y $${FILE_FROM} $${FILE_TO}$$escape_expand(\\n\\t))
}
