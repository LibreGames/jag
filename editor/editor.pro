# -------------------------------------------------
# Project created by QtCreator 2009-05-16T12:44:58
# -------------------------------------------------

unix: {
    TARGET = ./bin/jag-editor
    target.path = /usr/local/bin/
    INSTALLS += target
}
win32: {
    TARGET = ../../bin/jag-editor
    RC_FILE = res.rc
}

TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    levelwidget.cpp \
    levelpack.cpp
HEADERS += mainwindow.h \
    defines.h \
    levelwidget.h \
    levelpack.h
FORMS += mainwindow.ui \
    levelpack.ui
RESOURCES += editor.qrc
