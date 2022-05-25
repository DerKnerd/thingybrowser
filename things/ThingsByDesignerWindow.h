//
// Created by imanuel on 15.05.22.
//

#ifndef THINGYBROWSER_THINGSBYDESIGNER_H
#define THINGYBROWSER_THINGSBYDESIGNER_H

#include <libthingy.h>
#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include "wx/wx.h"

#endif

#include "wx/gbsizer.h"
#include "wx/progdlg.h"

class tbdDesignerLoadedEvent : public wxThreadEvent {
public:
    explicit tbdDesignerLoadedEvent(thingy::entities::User user);

    thingy::entities::User designer;
};

class tbdThingLoadedEvent : public wxThreadEvent {
public:
    explicit tbdThingLoadedEvent(int buttonIndex, const wxBitmap &bitmap, const wxString &title, unsigned long long id);

    int buttonIndex;
    wxBitmap bitmap;
    wxString title;
    unsigned long long id;
};

class tbdThingsLoadedEvent : public wxThreadEvent {
public:
    explicit tbdThingsLoadedEvent();
};

wxDEFINE_EVENT(tbdEVT_DESIGNER_LOADED, tbdDesignerLoadedEvent);
wxDEFINE_EVENT(tbdEVT_THING_LOADED, tbdThingLoadedEvent);
wxDEFINE_EVENT(tbdEVT_THINGS_LOADED, tbdThingsLoadedEvent);

class ThingsByDesignerWindow : public wxPanel {
public:
    ThingsByDesignerWindow(wxWindow *parent, unsigned long long int username);

    unsigned long long designerId;
private:
    wxStaticText *title;
    std::vector<wxBitmapButton *> thingButtons;
    std::vector<unsigned long long> ids;
};


#endif //THINGYBROWSER_THINGSBYDESIGNER_H
