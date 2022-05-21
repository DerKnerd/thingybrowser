//
// Created by imanuel on 23.04.22.
//

#ifndef THINGYBROWSER_THINGDETAILWINDOW_H
#define THINGYBROWSER_THINGDETAILWINDOW_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include "wx/wx.h"

#endif

#include "wx/activityindicator.h"
#include "wx/html/htmlwin.h"
#include "wx/gbsizer.h"
#include "wx/progdlg.h"
#include "wx/aui/aui.h"
#include "wx/hyperlink.h"
#include <libthingy.h>

#define CPPHTTPLIB_OPENSSL_SUPPORT

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


wxDEFINE_EVENT(twEVT_THING_LOADED, twThingLoadedEvent);
wxDEFINE_EVENT(twEVT_IMAGE_LOADED, twImageLoadedEvent);
wxDEFINE_EVENT(twEVT_ALL_IMAGES_LOADED, twAllImagesLoadedEvent);

class ThingDetailWindow : public wxPanel {
public:
    ThingDetailWindow(wxWindow *parent, unsigned long long thingId);

    unsigned long long thingId;

    thingy::entities::Thing thing;

private:
    wxScrolledWindow *scrolledWindow;
    wxStaticText *title;
    wxGridBagSizer *imageSizer;
    wxHtmlWindow *details;
    wxProgressDialog *downloadProgress;
    wxHyperlinkCtrl *createdBy;

    void displayThing();
};


#endif //THINGYBROWSER_THINGDETAILWINDOW_H
