//
// Created by imanuel on 13.03.22.
//

#ifndef THINGYBROWSER_MAINWINDOW_H
#define THINGYBROWSER_MAINWINDOW_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include "wx/wx.h"

#endif

#include "wx/notebook.h"
#include "pages/mainwindow/ThingsPage.h"

enum MainWindowActions {
    MainWindowThingybrowserSettings,
    MainWindowOpenThingById,
    MainWindowOpenThingByUrl,
    MainWindowOpenCollectionById,
};

class MainWindow : public wxFrame {
private:
    wxToolBar *toolbar;
    wxNotebook *contentNotebook;

    void handleShow(wxShowEvent &event);

public:
    MainWindow();

    void loadData();
};


#endif //THINGYBROWSER_MAINWINDOW_H
