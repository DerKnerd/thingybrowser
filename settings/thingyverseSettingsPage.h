//
// Created by imanuel on 08.10.21.
//

#ifndef WXOCTO_OCTOPRINTSERVERSETTINGSPAGE_H
#define WXOCTO_OCTOPRINTSERVERSETTINGSPAGE_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP

#include <wx/wx.h>
#include <wx/preferences.h>

#endif

class thingyverseSettingsPage : public wxStockPreferencesPage {
public:
    thingyverseSettingsPage() : wxStockPreferencesPage(Kind_General) {}

    wxWindow *CreateWindow(wxWindow *parent) override;
};


#endif //WXOCTO_OCTOPRINTSERVERSETTINGSPAGE_H
