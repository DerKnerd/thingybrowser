//
// Created by imanuel on 15.04.22.
//

#ifndef THINGYBROWSER_THINGSPAGE_H
#define THINGYBROWSER_THINGSPAGE_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include "wx/wx.h"

#endif

#include "tbButtonGridPage.h"

class ThingsPage : public tbButtonGridPage {
public:
    void internalLoad(int page, const wxString &apiKey, wxEvtHandler *sink, const wxString &keyword) override;

    explicit ThingsPage(wxWindow *parent);

private:
    void handleClick(int idx) override;
};

#endif //THINGYBROWSER_THINGSPAGE_H
