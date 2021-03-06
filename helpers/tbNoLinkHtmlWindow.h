//
// Created by imanuel on 24.04.22.
//

#ifndef THINGYBROWSER_TBNOLINKHTMLWINDOW_H
#define THINGYBROWSER_TBNOLINKHTMLWINDOW_H

#include "wx/html/htmlwin.h"

class twNoLinkHtmlWindow : public wxHtmlWindow {
public:
    explicit twNoLinkHtmlWindow(wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint &pos = wxDefaultPosition,
                                const wxSize &size = wxDefaultSize, long style = wxHW_DEFAULT_STYLE,
                                const wxString &name = wxT("htmlWindow"));

    void OnLinkClicked(const wxHtmlLinkInfo &link) override;
};

#endif //THINGYBROWSER_TBNOLINKHTMLWINDOW_H
