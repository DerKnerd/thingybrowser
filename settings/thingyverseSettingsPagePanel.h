//
// Created by imanuel on 08.10.21.
//

#ifndef WXOCTO_OCTOPRINTSERVERSETTINGSPAGEPANEL_H
#define WXOCTO_OCTOPRINTSERVERSETTINGSPAGEPANEL_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP

#include <wx/wx.h>
#include <wx/preferences.h>
#include "thingybrowserSettings.h"

#endif

class thingyverseSettingsPagePanel : public wxPanel {
public:
    explicit thingyverseSettingsPagePanel(wxWindow *parent);

    bool TransferDataToWindow() override;

    bool TransferDataFromWindow() override;

private:
    void updateSettingsIfNecessary();

    void handleApiKeyChanged(wxCommandEvent &event);

    wxTextCtrl *apiKey;
    thingybrowserSettings currentSettings;
};


#endif //WXOCTO_OCTOPRINTSERVERSETTINGSPAGEPANEL_H
