#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QDebug>
#include <QThread>
#include <QSettings>
#include <QTranslator>
#include <QtGlobal>

#include "gamewidget.h"
#include "gamestock.h"
#include "gameprofile.h"
#include "defines.h"

#include "displaywrapper.h"
#include "scaler.h"

#ifdef Q_OS_WIN32
#include "SDL.h"
#include "SDL_mixer.h"
#else
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#endif

#undef main

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QApplication::setGraphicsSystem("raster");
#endif
    QApplication a(argc, argv);

    // resources
    QString resourcePath = a.applicationDirPath();
    resourcePath.chop(3);
    resourcePath += "data/"; // bin -> data

  // on X11, we'll check if data directory exists locally first
#ifdef Q_OS_UNIX
  if (!QDir(resourcePath).exists())
    resourcePath = "/usr/share/games/jag/data/";
#endif

  if (!QDir(resourcePath).exists()) {
      QMessageBox::critical(0, QString("Files are missing"),
                               QString("Cannot find data folder<br><b>%1</b><br>%2 will exit now.")
                               .arg(resourcePath, GameName),
                               QMessageBox::Close);
      a.quit();
      return -1;
  }

    // display wrapper
    DisplayWrapper::init();

    // SDL sound initialization
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        qDebug() << "Unable to initialize SDL: " << SDL_GetError();
        //fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
        return -1;
    }

    // settings of audio (could be changed)
    int audio_rate = 22050;
    Uint16 audio_format = MIX_DEFAULT_FORMAT;
    int audio_channels = 2;
    int audio_buffers = 1024;

    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0)
    {
        qDebug() << "Unable to initialize audio: " << Mix_GetError();
        //fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
        return -1;
    }

    // prioritize thread
    QThread::currentThread()->setPriority(QThread::HighPriority);

    // scaler initialization
    Scaler::init();

    // translation
    QSettings settings("xlabsoft","jag");
    QString lang = settings.value("Language", "").toString();
    QTranslator translator;
    if (!lang.isEmpty()) {
        QString langpath = resourcePath + "lang/";
        QString filename = langpath + "jag_" + lang + ".qm";
        //QMessageBox::about(0, "", filename);
        if (!langpath.isEmpty() && QFile::exists(filename)) {
            translator.load(filename);
            a.installTranslator(&translator);
        }
    }

    // main widget & startup
    GameWidget w(resourcePath);

    int result = a.exec();

    // deinitialization of SDL
    Mix_CloseAudio();
    SDL_Quit();

    // deinitialization of DisplayWrapper
    DisplayWrapper::restoreVideoMode();

    return result;
}
