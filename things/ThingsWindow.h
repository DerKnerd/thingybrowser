//
// Created by imanuel on 23.04.22.
//

#ifndef THINGYBROWSER_THINGSWINDOW_H
#define THINGYBROWSER_THINGSWINDOW_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include "wx/wx.h"

#endif

#include "wx/activityindicator.h"
#include "wx/html/htmlwin.h"
#include "wx/gbsizer.h"
#include "wx/progdlg.h"

#include <libthingy.h>

#define CPPHTTPLIB_OPENSSL_SUPPORT

enum ThingsWindowActions {
    ThingsWindowDownloadThing,
    ThingsWindowGoToDesigner,
};

class twThingLoadedEvent : public wxThreadEvent {
public:
    explicit twThingLoadedEvent(thingy::entities::Thing thing);

    thingy::entities::Thing thing;
};

class twImageLoadedEvent : public wxThreadEvent {
public:
    explicit twImageLoadedEvent(const wxBitmap &image);

    wxBitmap image;
};

class twAllImagesLoadedEvent : public wxThreadEvent {
public:
    explicit twAllImagesLoadedEvent(int height);

    int height;
};

class twFileDownloadedEvent : public wxThreadEvent {
public:
    explicit twFileDownloadedEvent(std::string message);

    std::string message;
};

class twFileDownloadingEvent : public wxThreadEvent {
public:
    explicit twFileDownloadingEvent(std::string message);

    std::string message;
};

class twLogMessageEvent : public wxThreadEvent {
public:
    explicit twLogMessageEvent(const wxString &message);

    wxString message;
};

class twFilesCountedEvent : public wxThreadEvent {
public:
    explicit twFilesCountedEvent(long count);

    long count;
};

wxDEFINE_EVENT(twEVT_THING_LOADED, twThingLoadedEvent);
wxDEFINE_EVENT(twEVT_IMAGE_LOADED, twImageLoadedEvent);
wxDEFINE_EVENT(twEVT_ALL_IMAGES_LOADED, twAllImagesLoadedEvent);
wxDEFINE_EVENT(twEVT_LOG_MESSAGE, twLogMessageEvent);
wxDEFINE_EVENT(twEVT_FILE_DOWNLOADED, twFileDownloadedEvent);
wxDEFINE_EVENT(twEVT_FILE_DOWNLOADING, twFileDownloadingEvent);
wxDEFINE_EVENT(twEVT_FILES_COUNTED, twFilesCountedEvent);

class ThingsWindow : public wxFrame {
public:
    ThingsWindow(wxWindow *parent, unsigned long long thingId);

private:
    thingy::entities::Thing thing;
    wxToolBar *toolbar;
    wxPanel *panel;
    wxScrolledWindow *scrolledWindow;

    wxStaticText *title;
    wxGridBagSizer *imageSizer;
    wxHtmlWindow *details;
    wxProgressDialog *downloadProgress;

    void displayThing();
};


#endif //THINGYBROWSER_THINGSWINDOW_H
