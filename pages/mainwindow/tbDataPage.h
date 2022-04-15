//
// Created by imanuel on 15.04.22.
//

#ifndef THINGYBROWSER_TBDATAPAGE_H
#define THINGYBROWSER_TBDATAPAGE_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include "wx/wx.h"
#include "wx/notebook.h"

#endif

class tbDataPage : public wxPanel {
protected:
    virtual void internalLoad(int page, const wxString &apiKey, wxEvtHandler *sink, const wxString &keyword) = 0;

public:
    tbDataPage() = default;

    tbDataPage(wxWindow *parent, wxWindowID winid, const wxPoint &pos, const wxSize &size, long style,
               const wxString &name) : wxPanel(parent, winid, pos, size, style, name) {}
};


#endif //THINGYBROWSER_TBDATAPAGE_H
