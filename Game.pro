# -------------------------------------------------
# Project created by QtCreator 2009-05-07T18:37:02
# -------------------------------------------------
QT += xml \
    opengl

equals(QT_MAJOR_VERSION, 5) {
    QT += x11extras
}

unix: {
    TARGET = ./bin/jag
    target.path = /usr/games/
    INSTALLS += target
    datas.path = /usr/share/games/jag
    datas.files = data
    INSTALLS += datas
    desktop.path = /usr/share/applications/
    desktop.files = dist/jag.desktop
    INSTALLS += desktop
    icon.path = /usr/share/icons/hicolor/256x256/apps/
    icon.files = dist/jag.png
    INSTALLS += icon
    appdata.path = /usr/share/appdata/
    appdata.files = dist/jag.appdata.xml
    INSTALLS += appdata
    LIBS += -lXrandr
}
win32: {
    TARGET = ../bin/jag
    INCLUDEPATH += winlibs/SDL-1.2.13/include/SDL \
        winlibs/SDL_mixer-1.2.8
    LIBS += -Lwinlibs/SDL-1.2.13/lib \
        -Lwinlibs/SDL_mixer-1.2.8/lib \
        -Lwinlibs/libs \
        -lSDL.dll
    RC_FILE = res.rc
}
LIBS += -lX11 \
    -lSDL \
    -lSDL_mixer
TEMPLATE = app
SOURCES += main.cpp \
    gamewidget.cpp \
    gamescene.cpp \
    gameitem.cpp \
    gamepaint.cpp \
    gamecontrol.cpp \
    baseitem.cpp \
    gamemenu.cpp \
    gametools.cpp \
    hammertool.cpp \
    bighammertool.cpp \
    bombtool.cpp \
    gamesound.cpp \
    menucontrol.cpp \
    gameprofile.cpp \
    thundertool.cpp \
    smallhammertool.cpp \
    clocktool.cpp \
    gamestock.cpp \
    randomkilltool.cpp \
    mixertool.cpp \
    unblocktool.cpp \
    twintool.cpp \
    displaywrapper.cpp \
    gamebonus.cpp \
    gamestatics.cpp \
    gamestat.cpp \
    gamebackground.cpp \
    consttools.cpp \
    gamexml.cpp \
    scaler.cpp
HEADERS += gamewidget.h \
    gamescene.h \
    defines.h \
    gameitem.h \
    baseitem.h \
    gamemenu.h \
    gametools.h \
    hammertool.h \
    bighammertool.h \
    bombtool.h \
    gamesound.h \
    gameprofile.h \
    thundertool.h \
    smallhammertool.h \
    clocktool.h \
    gamestock.h \
    randomkilltool.h \
    mixertool.h \
    unblocktool.h \
    twintool.h \
    displaywrapper.h \
    gamebonus.h \
    scene_if.h \
    gamestat.h \
    gamebackground.h \
    consttools.h \
    scaler.h \
    version.h
RESOURCES += resources.qrc
FORMS += menu.ui
TRANSLATIONS += ./lang/jag_ru.ts
