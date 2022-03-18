//
// Created by imanuel on 13.03.22.
//

#include "MainApp.h"

MainApp::MainApp() {

}

bool MainApp::OnInit() {
    if (!wxApp::OnInit()) {
        return false;
    }

    SetAppDisplayName(_("Test"));
    mainWindow = new MainWindow();

    SetTopWindow(mainWindow);

    return GetTopWindow()->Show();
}
