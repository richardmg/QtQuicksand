/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#include <QApplication>
#include "mainwindow.h"

// Lock S60 orientation
#ifdef Q_OS_SYMBIAN
#include <eikenv.h>
#include <eikappui.h>
#include <aknenv.h>
#include <aknappui.h>
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Lock S60 orientation
#ifdef Q_OS_SYMBIAN
    CAknAppUi* appUi = dynamic_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
    TRAPD(error, 
    if (appUi) {
        appUi->SetOrientationL(CAknAppUi::EAppUiOrientationLandscape);
    }
    );
#endif

    MainWindow w;

#ifdef Q_OS_SYMBIAN
    w.showFullScreen();
#elif defined Q_WS_MAEMO_5
    w.showFullScreen();
#else
    w.showFullScreen();
#endif

   return a.exec();
}
