//
// Created by imanuel on 13.03.22.
//

#include "MainWindow.h"
#include "MainApp.h"
#include "pages/mainwindow/tbDataPage.h"
#include "pages/mainwindow/CollectionsPage.h"
#include "pages/mainwindow/DesignersPage.h"
#include "things/ThingsWindow.h"

MainWindow::MainWindow() : wxFrame(nullptr, wxID_ANY, _("Thingybrowser - Browse the thingiverse"), wxDefaultPosition,
                                   wxSize(1280, 800), wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN) {
    toolbar = CreateToolBar(wxTB_HORIZONTAL | wxTB_HORZ_LAYOUT | wxTB_TEXT | wxTB_NOICONS);
    toolbar->Realize();

    auto menuBar = new wxMenuBar();
    this->SetMenuBar(menuBar);

    CreateStatusBar();

    auto fileMenu = new wxMenu();
    fileMenu->Append(MainWindowActions::MainWindowThingybrowserSettings, _("Settings"));
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, _("Exit"));

    auto thingMenu = new wxMenu();
    thingMenu->Append(MainWindowActions::MainWindowOpenThingById, _("Open thing by id"));

    menuBar->Append(fileMenu, _("File"));
    menuBar->Append(thingMenu, _("Things"));

    auto panel = new wxPanel(this, wxID_ANY);
    auto sizer = new wxBoxSizer(wxVERTICAL);
    contentNotebook = new wxNotebook(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, -1));
    sizer->Add(contentNotebook, wxSizerFlags().Proportion(1).Expand());
    contentNotebook->AddPage(new ThingsPage(contentNotebook), _("Things"), true);
    contentNotebook->AddPage(new CollectionsPage(contentNotebook), _("Collections"));
    contentNotebook->AddPage(new DesignersPage(contentNotebook), _("Designers"));

    SetName("MainWindow");
    SetSize(wxDLG_UNIT(this, wxSize(-1, -1)));
    panel->SetSizer(sizer);

    if (GetSizer()) {
        GetSizer()->Fit(panel);
    }
    if (GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }

    Bind(wxEVT_SHOW, &MainWindow::handleShow, this);
    Bind(wxEVT_MENU, [this](wxCommandEvent &) {
        MainApp::getInstance()->DismissPreferencesEditor();
        Close(true);
    }, wxID_EXIT);
    Bind(wxEVT_MENU, [this](wxCommandEvent &) {
        MainApp::getInstance()->ShowPreferencesEditor(this);
    }, MainWindowThingybrowserSettings);
    Bind(wxEVT_MENU, [this](wxCommandEvent &) {
        auto dialog = wxTextEntryDialog(this, "", _("Please enter a thing ID"), "", wxOK | wxCANCEL);
        dialog.SetTextValidator(wxFILTER_DIGITS);
        if (dialog.ShowModal() == wxID_OK) {
            unsigned long long thingId = 0;
            dialog.GetValue().ToULongLong(&thingId);
            auto window = new ThingsWindow(this, thingId);
            window->Show();
        }
    }, MainWindowOpenThingById);
    contentNotebook->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, [this](wxCommandEvent &) {
        if (contentNotebook->GetSelection() != -1) {
            auto page = dynamic_cast<tbButtonGridPage *>(contentNotebook->GetPage(contentNotebook->GetSelection()));
            if (page != nullptr) {
                page->loadData();
            }
        }
    });
}

void MainWindow::handleShow(wxShowEvent &event) {
    if (!MainApp::getInstance()->GetSettings().thingyverseApiKey.empty()) {
        loadData();
    } else {
        MainApp::getInstance()->ShowPreferencesEditor(this);
    }
}

void MainWindow::loadData() {
    if (contentNotebook->GetSelection() != -1) {
        auto page = dynamic_cast<tbButtonGridPage *>(contentNotebook->GetPage(contentNotebook->GetSelection()));
        if (page != nullptr) {
            page->loadData();
        }
    }
}

