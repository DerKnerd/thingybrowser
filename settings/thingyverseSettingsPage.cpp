//
// Created by imanuel on 08.10.21.
//

#include "thingyverseSettingsPage.h"
#include "thingyverseSettingsPagePanel.h"

wxWindow *thingyverseSettingsPage::CreateWindow(wxWindow *parent) {
    return new thingyverseSettingsPagePanel(parent);
}
