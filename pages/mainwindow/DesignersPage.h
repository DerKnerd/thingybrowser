//
// Created by imanuel on 15.04.22.
//

#ifndef THINGYBROWSER_DESIGNERSPAGE_H
#define THINGYBROWSER_DESIGNERSPAGE_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include "wx/wx.h"

#endif

#include "tbButtonGridPage.h"

class DesignersPage : public tbButtonGridPage {
public:
    void internalLoad(int page, const wxString &apiKey, wxEvtHandler *sink, const wxString &keyword) override;

    explicit DesignersPage(wxWindow *parent);

private:
    void handleClick(int idx) override;
};


#endif //THINGYBROWSER_DESIGNERSPAGE_H
