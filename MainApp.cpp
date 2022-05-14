//
// Created by imanuel on 13.03.22.
//

#include "MainApp.h"
#include "settings/thingyverseSettingsPage.h"
#include "wx/filesys.h"
#include "wx/fs_mem.h"
#include "wx/fs_inet.h"

MainApp::MainApp() {
    mainWindow = nullptr;
    configuration = wxConfigBase::Get();
    configuration->SetAppName("thingybrowser");
    configuration->SetVendorName("DerKnerd");

    appSettings = thingybrowserSettings();
    appSettings.thingyverseApiKey = configuration->Read("/thingy/apikey");
}

bool MainApp::OnInit() {
    if (!wxApp::OnInit()) {
        return false;
    }

    wxInitAllImageHandlers();
    wxFileSystem::AddHandler(new wxMemoryFSHandler());
    wxFileSystem::AddHandler(new wxInternetFSHandler());
    SetAppDisplayName(_("Thingybrowser - Browse the thingiverse"));
    mainWindow = new MainWindow();

    SetTopWindow(mainWindow);

    return GetTopWindow()->Show();
}

MainApp *MainApp::getInstance() {
    auto app = wxApp::GetInstance();

    return dynamic_cast<MainApp *>(app);
}

void MainApp::ShowPreferencesEditor(wxWindow *parent) {
    if (!preferenceEditor) {
        preferenceEditor.reset(new wxPreferencesEditor());
    }

    preferenceEditor->AddPage(new thingyverseSettingsPage());
    preferenceEditor->Show(parent);
}

void MainApp::DismissPreferencesEditor() {
    if (preferenceEditor) {
        preferenceEditor->Dismiss();
    }
}

void MainApp::UpdateSettings(const thingybrowserSettings &settings) {
    appSettings = settings;
    configuration->Write("/thingy/apikey", wxString(appSettings.thingyverseApiKey));
    mainWindow->loadData();
}

int MainApp::OnExit() {
    delete wxConfigBase::Set((wxConfigBase *) nullptr);
    return wxAppBase::OnExit();
}