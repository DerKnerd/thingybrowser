//
// Created by imanuel on 08.10.21.
//

#include "thingyverseSettingsPagePanel.h"
#include "../MainApp.h"
#include "wx/hyperlink.h"

thingyverseSettingsPagePanel::thingyverseSettingsPagePanel(wxWindow *parent) : wxPanel(parent) {
    auto apiKeyLabel = new wxStaticText(this, wxID_ANY, _("Thingyverse API key"));
    apiKey = new wxTextCtrl(this, wxID_ANY);
    apiKey->SetLabel(_("Thingyverse API Key"));
    apiKey->SetMinSize(wxSize(400, -1));
    apiKey->SetWindowStyleFlag(wxTE_PASSWORD);

    auto thingyverseDevConsoleLabel = new wxStaticText(this, wxID_ANY, _("How to get API key"));
    auto thingyverseDevConsole = new wxHyperlinkCtrl(this, wxID_ANY, _("Log in and choose Desktop App"),
                                                     "https://www.thingiverse.com/apps/create");

    auto sizer = new wxFlexGridSizer(2);
    sizer->SetFlexibleDirection(wxBOTH);
    sizer->SetHGap(WXC_FROM_DIP(5));
    sizer->SetVGap(WXC_FROM_DIP(5));

    sizer->Add(apiKeyLabel, wxSizerFlags().Border());
    sizer->Add(apiKey, wxSizerFlags().Border());
    sizer->Add(thingyverseDevConsoleLabel, wxSizerFlags().Border());
    sizer->Add(thingyverseDevConsole, wxSizerFlags().Border());

    SetSizerAndFit(sizer);

    apiKey->Bind(wxEVT_TEXT, &thingyverseSettingsPagePanel::handleApiKeyChanged, this);
}

bool thingyverseSettingsPagePanel::TransferDataToWindow() {
    currentSettings = MainApp::getInstance()->GetSettings();
    apiKey->SetValue(currentSettings.thingyverseApiKey);
    return true;
}

bool thingyverseSettingsPagePanel::TransferDataFromWindow() {
    MainApp::getInstance()->UpdateSettings(currentSettings);
    return true;
}

void thingyverseSettingsPagePanel::updateSettingsIfNecessary() {
    if (wxPreferencesEditor::ShouldApplyChangesImmediately()) {
        MainApp::getInstance()->UpdateSettings(currentSettings);
    }
}

void thingyverseSettingsPagePanel::handleApiKeyChanged(wxCommandEvent &event) {
    currentSettings.thingyverseApiKey = event.GetString();
    updateSettingsIfNecessary();
}
