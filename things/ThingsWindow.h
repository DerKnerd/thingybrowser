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

#include <libthingy.h>

#define CPPHTTPLIB_OPENSSL_SUPPORT

enum ThingsWindowActions {
    ThingsWindowDownloadThing,
};

class twThingLoaded : public wxThreadEvent {
public:
    explicit twThingLoaded(thingy::entities::Thing thing);

    thingy::entities::Thing thing;
};

class twImageLoaded : public wxThreadEvent {
public:
    explicit twImageLoaded(const wxBitmap& image);

    wxBitmap image;
};

class twAllImagesLoaded : public wxThreadEvent {
public:
    explicit twAllImagesLoaded(int height);

    int height;
};

class twLogMessage : public wxThreadEvent {
public:
    explicit twLogMessage(const wxString &message);

    wxString message;
};

wxDEFINE_EVENT(twEVT_THING_LOADED, twThingLoaded);
wxDEFINE_EVENT(twEVT_IMAGE_LOADED, twImageLoaded);
wxDEFINE_EVENT(twEVT_ALL_IMAGES_LOADED, twAllImagesLoaded);
wxDEFINE_EVENT(twEVT_LOG_MESSAGE, twLogMessage);

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

    void displayThing();
};


#endif //THINGYBROWSER_THINGSWINDOW_H
