//
// Created by imanuel on 13.03.22.
//

#include "MainWindow.h"
#include "MainApp.h"
#include "pages/mainwindow/tbDataPage.h"
#include "pages/mainwindow/CollectionsPage.h"
#include "pages/mainwindow/DesignersPage.h"
#include "things/ThingsWindow.h"
#include "collections/CollectionsWindow.h"

MainWindow::MainWindow() : wxFrame(nullptr, wxID_ANY, _("Thingybrowser - Browse the thingiverse"), wxDefaultPosition,
                                   wxSize(1280, 800), wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN) {
    toolbar = CreateToolBar(wxTB_HORIZONTAL | wxTB_HORZ_LAYOUT | wxTB_TEXT | wxTB_NOICONS);
    toolbar->AddTool(MainWindowActions::MainWindowOpenThingById, _("Open thing by id"), wxNullBitmap);
    toolbar->AddTool(MainWindowActions::MainWindowOpenThingByUrl, _("Open thing by URL"), wxNullBitmap);
    toolbar->AddSeparator();
    toolbar->AddTool(MainWindowActions::MainWindowOpenCollectionById, _("Open collection by id"), wxNullBitmap);
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
    thingMenu->Append(MainWindowActions::MainWindowOpenThingByUrl, _("Open thing by URL"));

    auto collectionMenu = new wxMenu();
    collectionMenu->Append(MainWindowActions::MainWindowOpenCollectionById, _("Open collection by id"));

    menuBar->Append(fileMenu, _("File"));
    menuBar->Append(thingMenu, _("Things"));
    menuBar->Append(collectionMenu, _("Collections"));

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
        auto dialog = wxTextEntryDialog(this, "", _("Please enter a collection ID"), "", wxOK | wxCANCEL);
        dialog.SetTextValidator(wxFILTER_DIGITS);
        if (dialog.ShowModal() == wxID_OK) {
            auto collectionId = 0ULL;
            dialog.GetValue().ToULongLong(&collectionId);
            auto collection = thingy::ThingiverseClient(
                    MainApp::getInstance()->GetSettings().thingyverseApiKey).getCollection(collectionId);
            auto window = new CollectionsWindow(this, collectionId, collection.count);
            window->Show();
        }
    }, MainWindowOpenCollectionById);
    Bind(wxEVT_MENU, [this](wxCommandEvent &) {
        auto dialog = wxTextEntryDialog(this, "", _("Please enter a thing ID"), "", wxOK | wxCANCEL);
        dialog.SetTextValidator(wxFILTER_DIGITS);
        if (dialog.ShowModal() == wxID_OK) {
            auto thingId = 0ULL;
            dialog.GetValue().ToULongLong(&thingId);
            auto window = new ThingsWindow(this, thingId);
            window->Show();
        }
    }, MainWindowOpenThingById);
    Bind(wxEVT_MENU, [this](wxCommandEvent &) {
        auto dialog = wxTextEntryDialog(this, "", _("Please enter a thing URL"), "", wxOK | wxCANCEL);
        if (dialog.ShowModal() == wxID_OK) {
            auto thingId = 0ULL;
            auto result = dialog.GetValue().AfterLast(':');
            result.ToULongLong(&thingId);
            auto window = new ThingsWindow(this, thingId);
            window->Show();
        }
    }, MainWindowOpenThingByUrl);
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

