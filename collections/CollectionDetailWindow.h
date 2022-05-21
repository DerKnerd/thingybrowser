//
// Created by imanuel on 15.05.22.
//

#ifndef THINGYBROWSER_COLLECTIONDETAILWINDOW_H
#define THINGYBROWSER_COLLECTIONDETAILWINDOW_H

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

wxDEFINE_EVENT(cwEVT_COLLECTION_LOADED, cwCollectionLoadedEvent);
wxDEFINE_EVENT(cwEVT_THING_LOADED, cwThingLoadedEvent);
wxDEFINE_EVENT(cwEVT_THINGS_LOADED, cwThingsLoadedEvent);

class CollectionDetailWindow : public wxPanel {
public:
    CollectionDetailWindow(wxWindow *parent, unsigned long long int collectionId, unsigned long long int thingCount);

    unsigned long long collectionId;
private:
    wxStaticText *title;
    wxProgressDialog *downloadProgress;
    std::vector<wxBitmapButton *> thingButtons;
    std::vector<unsigned long long> ids;
};


#endif //THINGYBROWSER_COLLECTIONDETAILWINDOW_H
