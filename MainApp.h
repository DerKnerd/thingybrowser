//
// Created by imanuel on 13.03.22.
//

#ifndef THINGYBROWSER_MAINAPP_H
#define THINGYBROWSER_MAINAPP_H


#include <wx/wxprec.h>

#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif

#include "MainWindow.h"

class MainApp : public wxApp {
private:
    MainWindow *mainWindow;

public:
    MainApp();

    ~MainApp() override = default;

    bool OnInit() override;
};


#endif //THINGYBROWSER_MAINAPP_H
