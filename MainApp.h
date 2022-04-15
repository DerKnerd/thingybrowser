//
// Created by imanuel on 13.03.22.
//

#ifndef THINGYBROWSER_MAINAPP_H
#define THINGYBROWSER_MAINAPP_H


#include <wx/wxprec.h>

#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif

#include "MainWindow.h"
#include "settings/thingybrowserSettings.h"
#include "wx/preferences.h"
#include "wx/confbase.h"

class MainApp : public wxApp {
private:
    MainWindow *mainWindow;
    wxScopedPtr<wxPreferencesEditor> preferenceEditor;
    thingybrowserSettings appSettings;
    wxConfigBase *configuration;

public:
    MainApp();

    ~MainApp() override = default;

    bool OnInit() override;

    int OnExit() override;

    static MainApp *getInstance();

    void ShowPreferencesEditor(wxWindow *parent);

    void DismissPreferencesEditor();

    const thingybrowserSettings &GetSettings() const { return appSettings; }

    void UpdateSettings(const thingybrowserSettings &settings);
};


#endif //THINGYBROWSER_MAINAPP_H
