//
// Created by imanuel on 13.03.22.
//

#ifndef THINGYBROWSER_MAINWINDOW_H
#define THINGYBROWSER_MAINWINDOW_H

#include <libthingy.h>
#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include "wx/wx.h"

#endif

#include "wx/aui/aui.h"
#include "things/ThingDetailWindow.h"

enum MainWindowActions {
    MainWindowThingybrowserSettings,
    MainWindowOpenThingById,
    MainWindowOpenThingByUrl,
    MainWindowOpenCollectionById,
    MainWindowOpenThingOverview,
    MainWindowOpenCollectionOverview,
    MainWindowOpenDesignerOverview,

    ThingsWindowDownloadThing,
    ThingsWindowOpenOnThingiverse,
    ThingsWindowGoToDesigner,

    ThingsByDesignerWindowDownloadThings,

    CollectionsWindowDownloadThings,
    CollectionsWindowGoToDesigner,
    CollectionsWindowOpenOnThingiverse,
};

class mwThingLoadedEvent : public wxCommandEvent {
public:
    explicit mwThingLoadedEvent(thingy::entities::Thing thing);

    thingy::entities::Thing thing;
};

class mwFileDownloadedEvent : public wxThreadEvent {
public:
    explicit mwFileDownloadedEvent(std::string message);

    std::string message;
};

class mwFileDownloadingEvent : public wxThreadEvent {
public:
    explicit mwFileDownloadingEvent(std::string message);

    std::string message;
};

class mwLogMessageEvent : public wxThreadEvent {
public:
    explicit mwLogMessageEvent(const wxString &message);

    wxString message;
};

class mwFilesCountedEvent : public wxThreadEvent {
public:
    explicit mwFilesCountedEvent(long count);

    long count;
};

class mwThingDownloadedEvent : public wxThreadEvent {
public:
    explicit mwThingDownloadedEvent(std::string message);

    std::string message;
};

class mwThingDownloadingEvent : public wxThreadEvent {
public:
    explicit mwThingDownloadingEvent(std::string message);

    std::string message;
};

class mwThingsCountedEvent : public wxThreadEvent {
public:
    explicit mwThingsCountedEvent(unsigned long long thingCount);

    unsigned long long thingCount = 0;
};

wxDEFINE_EVENT(mwEVT_THING_LOADED, mwThingLoadedEvent);
wxDEFINE_EVENT(mwEVT_LOG_MESSAGE, mwLogMessageEvent);
wxDEFINE_EVENT(mwEVT_FILE_DOWNLOADED, mwFileDownloadedEvent);
wxDEFINE_EVENT(mwEVT_FILE_DOWNLOADING, mwFileDownloadingEvent);
wxDEFINE_EVENT(mwEVT_FILES_COUNTED, mwFilesCountedEvent);
wxDEFINE_EVENT(mwEVT_THING_DOWLOADED, mwThingDownloadedEvent);
wxDEFINE_EVENT(mwEVT_THING_DOWLOADING, mwThingDownloadingEvent);
wxDEFINE_EVENT(mwEVT_THINGS_COUNTED, mwThingsCountedEvent);

class MainWindow : public wxFrame {
public:
    MainWindow();

    void loadData();

    void addThingPage(unsigned long long int thingId, const wxString &caption);

    void
    addCollectionPage(unsigned long long int collectionId, unsigned long long thingsCount, const wxString &caption);

    void downloadThingFilesAndImages(unsigned long long thingId, const std::string &path, const std::string &apiKey);

    void addThingsByDesignerPage(unsigned long long int userId, const wxString &caption);

private:
    unsigned long long totalThingsToDownload = 0;
    unsigned long long thingsDownloaded = 0;

    wxAuiToolBar *mainWindowToolbar;
    wxAuiToolBar *thingToolbar;
    wxAuiToolBar *collectionToolbar;
    wxAuiToolBar *thingsByDesignerToolbar;
    unsigned long long thingId;
    unsigned long long collectionId;
    unsigned long long userId;
    thingy::entities::Thing thing;

    wxListBox *logOutput;
    wxGauge *downloadProgress;

    void handleShow(wxShowEvent &event);

    void log(const wxString &message);

    wxAuiManager auiManager;
    wxAuiNotebook *contentNotebook;
};


#endif //THINGYBROWSER_MAINWINDOW_H
