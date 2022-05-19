//
// Created by imanuel on 15.05.22.
//

#ifndef THINGYBROWSER_COLLECTIONSWINDOW_H
#define THINGYBROWSER_COLLECTIONSWINDOW_H

#include <libthingy.h>
#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include "wx/wx.h"

#endif

#include "wx/gbsizer.h"
#include "wx/progdlg.h"

class cwCollectionLoadedEvent : public wxThreadEvent {
public:
    explicit cwCollectionLoadedEvent(thingy::entities::Collection collection);

    thingy::entities::Collection collection;
};

class cwThingLoadedEvent : public wxThreadEvent {
public:
    explicit cwThingLoadedEvent(int buttonIndex, const wxBitmap &bitmap, const wxString &title, unsigned long long id);

    int buttonIndex;
    wxBitmap bitmap;
    wxString title;
    unsigned long long id;
};

class cwThingsLoadedEvent : public wxThreadEvent {
public:
    explicit cwThingsLoadedEvent();
};

class cwLogMessageEvent : public wxThreadEvent {
public:
    explicit cwLogMessageEvent(const wxString &message);

    wxString message;
};

class cwThingDownloadedEvent : public wxThreadEvent {
public:
    explicit cwThingDownloadedEvent(std::string message);

    std::string message;
};

class cwThingDownloadingEvent : public wxThreadEvent {
public:
    explicit cwThingDownloadingEvent(std::string message);

    std::string message;
};

wxDEFINE_EVENT(cwEVT_COLLECTION_LOADED, cwCollectionLoadedEvent);
wxDEFINE_EVENT(cwEVT_THING_LOADED, cwThingLoadedEvent);
wxDEFINE_EVENT(cwEVT_THINGS_LOADED, cwThingsLoadedEvent);
wxDEFINE_EVENT(cwEVT_LOG_MESSAGE, cwLogMessageEvent);
wxDEFINE_EVENT(cwEVT_THING_DOWLOADED, cwThingDownloadedEvent);
wxDEFINE_EVENT(cwEVT_THING_DOWLOADING, cwThingDownloadingEvent);

enum CollectionsWindowActions {
    CollectionsWindowDownloadThings,
    CollectionsWindowGoToDesigner,
};

class CollectionsWindow : public wxFrame {
public:
    CollectionsWindow(wxWindow *parent, unsigned long long int collectionId, const std::string &title,
                      unsigned long long int thingCount);

private:
    wxToolBar *toolbar;
    wxStaticText *title;
    wxPanel *panel;
    wxProgressDialog *downloadProgress;
    std::vector<wxBitmapButton *> thingButtons;
    std::vector<unsigned long long> ids;
};


#endif //THINGYBROWSER_COLLECTIONSWINDOW_H