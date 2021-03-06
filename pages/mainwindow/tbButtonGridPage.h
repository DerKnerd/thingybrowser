//
// Created by imanuel on 15.04.22.
//

#ifndef THINGYBROWSER_TBBUTTONGRIDPAGE_H
#define THINGYBROWSER_TBBUTTONGRIDPAGE_H

#include "tbDataPage.h"
#include "wx/srchctrl.h"

class tbLoadedEvent : public wxThreadEvent {
public:
    tbLoadedEvent(int buttonIndex, const wxBitmap &bitmap, const wxString &title, unsigned long long id);

    int buttonIndex;
    wxBitmap bitmap;
    wxString title;
    unsigned long long id;
};

class tbPagingDoneEvent : public wxThreadEvent {
public:
    explicit tbPagingDoneEvent(size_t loadedThings);

    size_t loadedThings = 0;
};

wxDEFINE_EVENT(tbEVT_LOADED, tbLoadedEvent);
wxDEFINE_EVENT(tbEVT_PAGING_DONE, tbPagingDoneEvent);

class tbButtonGridPage : public tbDataPage {
public:
    virtual void handleClick(int idx) = 0;

protected:
    unsigned long long page = 1;
    wxString keyword = "";
    wxScrolledWindow *scrolledWindow;
    std::vector<wxBitmapButton *> tiles;
    wxButton *nextPage;
    wxButton *previousPage;
    wxSearchCtrl *searchCtrl;
    std::vector<unsigned long long> ids;
    bool initialLoadingDone = false;

public:
    void initialLoad();

    void loadData();

    tbButtonGridPage(wxWindow *parent, wxWindowID winid, const wxPoint &pos, const wxSize &size, long style,
                     const wxString &name);
};


#endif //THINGYBROWSER_TBBUTTONGRIDPAGE_H
