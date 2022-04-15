//
// Created by imanuel on 13.03.22.
//

#ifndef THINGYBROWSER_MAINWINDOW_H
#define THINGYBROWSER_MAINWINDOW_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif

#include <wx/filesys.h>
#include <wx/fs_mem.h>
#include <wx/mstream.h>
#include "wx/vscroll.h"

#ifdef WXC_FROM_DIP
#undef WXC_FROM_DIP
#endif
#if wxVERSION_NUMBER >= 3100
#define WXC_FROM_DIP(x) wxWindow::FromDIP(x, NULL)
#else
#define WXC_FROM_DIP(x) x
#endif

enum MainWindowActions {
    MainWindowThingybrowserSettings
};

class tbThingLoadedEvent : public wxThreadEvent {
public:
    tbThingLoadedEvent(int buttonIndex, const wxBitmap &bitmap, const wxString &title);

    int buttonIndex;
    wxBitmap bitmap;
    wxString title;
};

class tbThingPagingDoneEvent : public wxThreadEvent {
public:
    explicit tbThingPagingDoneEvent(int newPage, int loadedThings);

    int newPage = -1;
    int loadedThings = 0;
};

wxDEFINE_EVENT(tbEVT_THING_LOADED, tbThingLoadedEvent);
wxDEFINE_EVENT(tbEVT_THING_PAGING_DONE, tbThingPagingDoneEvent);

class MainWindow : public wxFrame {
private:
    unsigned long long page = 1;
    wxScrolledWindow *thingList;
    std::vector<wxBitmapButton *> thingTiles;
    wxToolBar *toolbar;
    wxButton *nextPage;
    wxButton *previousPage;

    void handleShow(wxShowEvent &event);

public:
    MainWindow();

    void loadData();
};


#endif //THINGYBROWSER_MAINWINDOW_H
