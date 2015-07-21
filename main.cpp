#include <QtGui/QApplication>
#include "gamewidget.h"
#include "gamestock.h"
#include "gameprofile.h"

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

// globals
QString resourcePath, langPath;

int main(int argc, char *argv[])
{
#if QT_VERSION >= 0x040500
    QApplication::setGraphicsSystem("raster");
#endif
    QApplication a(argc, argv);

  resourcePath = QApplication::applicationDirPath();
  resourcePath.chop(3);
  resourcePath += "data/"; // bin -> data

  // on X11, we'll check if data directory exists locally first
#ifdef Q_WS_X11
  if (!QDir(resourcePath).exists())
    resourcePath = "/usr/local/games/jag/data/";
#endif

  langPath = resourcePath + "lang/";

  if (!QDir(resourcePath).exists()) {
      QMessageBox::critical(0, QObject::tr("Files are missing"),
                               QObject::tr("Cannot find data folder<br><b>%1</b><br>JAG will exit now.").arg(resourcePath),
                               QMessageBox::Close);
      QApplication::quit();
      return -1;
  }

    // Display Wrapper
    DisplayWrapper::init();

    // SDL sound initialization
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        qDebug() << "Unable to initialize SDL: " << SDL_GetError();
        //fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
        QApplication::quit();
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
        QApplication::quit();
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
    if (!lang.isEmpty() && !langPath.isEmpty())
    {
        if (QFile::exists(langPath + lang))
        {
            translator.load(langPath + lang);
            a.installTranslator(&translator);
        }
    }

    // main widget & startup
    GameWidget w;

    int result = a.exec();

    // deinitialization of SDL
    Mix_CloseAudio();
    SDL_Quit();

    // deinitialization of DisplayWrapper
    DisplayWrapper::restoreVideoMode();

    return result;
}
