//
// Created by imanuel on 13.03.22.
//

#include "MainWindow.h"
#include "wx/wrapsizer.h"
#include "MainApp.h"
#include <libthingy.h>

void MainWindow::loadData() {
    for (auto button: thingTiles) {
        button->Hide();
    }
    auto apiKey = MainApp::getInstance()->GetSettings().thingyverseApiKey;
    auto th = std::thread([](int page, wxString apiKey, wxEvtHandler *sink) {
        auto thingCount = 0;
        try {
            auto client = thingy::ThingiverseClient(apiKey);
            auto things = client.getThings(page);

            for (auto i = 0; i < things.size(); ++i) {
                auto thing = things.at(i);
                auto httpClient = httplib::Client("cdn.thingiverse.com");
                auto response = httpClient.Get(thing.thumbnail.c_str());
                auto stream = wxMemoryInputStream(response->body.c_str(), response->body.length());
                auto img = wxImage(stream);
                auto height = WXC_FROM_DIP(240);
                auto width = WXC_FROM_DIP(240);
                if (img.GetHeight() > img.GetWidth()) {
                    height = static_cast<int>(static_cast<double>(img.GetHeight()) /
                                              (static_cast<double>(img.GetWidth()) / WXC_FROM_DIP(240)));
                } else if (img.GetHeight() < img.GetWidth()) {
                    width = static_cast<int>(static_cast<double>(img.GetWidth()) /
                                             (static_cast<double>(img.GetHeight()) / WXC_FROM_DIP(240)));
                }
                auto bmp = wxBitmap(
                        img.Scale(width, height, wxIMAGE_QUALITY_HIGH).Size(
                                wxSize(WXC_FROM_DIP(240), WXC_FROM_DIP(240)), wxDefaultPosition));
                wxQueueEvent(sink, new tbThingLoadedEvent(i, bmp, thing.name));
            }
            thingCount = things.size();
        } catch (thingy::ThingiverseException &e) {
            wxLogStatus(e.what());
        }
        wxQueueEvent(sink, new tbThingPagingDoneEvent(page, thingCount));
    }, page, apiKey, this);
    th.detach();
}

MainWindow::MainWindow() : wxFrame(nullptr, wxID_ANY, _("Thingybrowser - Browse the thingiverse"), wxDefaultPosition,
                                   wxSize(1280, 600),
                                   wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN) {
    wxInitAllImageHandlers();
    wxFileSystem::AddHandler(new wxMemoryFSHandler());
    toolbar = CreateToolBar(wxTB_HORIZONTAL | wxTB_HORZ_LAYOUT | wxTB_TEXT | wxTB_NOICONS);
    toolbar->Realize();

    auto menuBar = new wxMenuBar();
    this->SetMenuBar(menuBar);

    auto fileMenu = new wxMenu();
    fileMenu->Append(MainWindowActions::MainWindowThingybrowserSettings, _("Settings"));
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, _("Exit"));

    menuBar->Append(fileMenu, _("File"));

    auto panel = new wxPanel(this, wxID_ANY);
    auto sizer = new wxFlexGridSizer(2, 0, 5, 5);
    panel->SetSizer(sizer);
    sizer->AddGrowableRow(0, 1);
    sizer->AddGrowableCol(0, 1);
    sizer->SetFlexibleDirection(wxHORIZONTAL);

    auto thingListSizer = new wxWrapSizer(wxHORIZONTAL);
    thingList = new wxScrolledWindow(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxVSCROLL);
    thingList->SetSizer(thingListSizer);
    thingList->SetScrollRate(5, 5);

    sizer->Add(thingList, wxSizerFlags().Proportion(1).Expand().Border(wxALL, WXC_FROM_DIP(5)));

    thingTiles = std::vector<wxBitmapButton *>(20);
    for (auto i = 0; i < 20; ++i) {
        thingTiles[i] = new wxBitmapButton(thingList, wxID_ANY, wxNullBitmap, wxDefaultPosition,
                                           wxSize(WXC_FROM_DIP(240), WXC_FROM_DIP(240)));
        thingListSizer->Add(thingTiles[i], wxSizerFlags().Proportion(1).Expand().Border(wxALL, WXC_FROM_DIP(
                5)).ReserveSpaceEvenIfHidden());
        thingTiles[i]->Hide();
    }
    thingList->SetVirtualSize(wxSize(GetClientSize().x, WXC_FROM_DIP(240) * 4 + WXC_FROM_DIP(5) * 3));

    auto pagingSizer = new wxGridSizer(0, 2, 0, 0);
    previousPage = new wxButton(panel, wxID_ANY, _("Previous page"));
    previousPage->Enable(false);
    nextPage = new wxButton(panel, wxID_ANY, _("Next page"));
    nextPage->Enable(false);

    pagingSizer->Add(previousPage, 1, wxALL, WXC_FROM_DIP(5));
    pagingSizer->Add(nextPage, 1, wxALL | wxALIGN_RIGHT, WXC_FROM_DIP(5));

    sizer->Add(pagingSizer, 1, wxALL | wxEXPAND, WXC_FROM_DIP(0));

    SetName("MainWindow");
    SetSize(wxDLG_UNIT(this, wxSize(-1, -1)));

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

    Bind(tbEVT_THING_LOADED, [this](tbThingLoadedEvent &event) {
        this->thingTiles[event.buttonIndex]->SetBitmap(event.bitmap);
        this->thingTiles[event.buttonIndex]->SetLabel(event.title);
        this->thingTiles[event.buttonIndex]->Show();
    });
    Bind(tbEVT_THING_PAGING_DONE, [this](tbThingPagingDoneEvent &event) {
        this->previousPage->Enable(page > 1);
        this->nextPage->Enable(event.loadedThings == 20);
    });
    thingList->Bind(wxEVT_SIZE, [this](wxSizeEvent &event) {
        this->SetVirtualSize(wxSize(this->GetClientSize().x, -1));
    });
    thingList->Bind(wxEVT_SCROLL_CHANGED, [this](wxScrollEvent &event) {
        this->SetVirtualSize(wxSize(this->GetClientSize().x, -1));
    });
    previousPage->Bind(wxEVT_BUTTON, [this](wxCommandEvent &event) {
        page--;
        loadData();
    });
    nextPage->Bind(wxEVT_BUTTON, [this](wxCommandEvent &event) {
        page++;
        loadData();
    });
}

void MainWindow::handleShow(wxShowEvent &event) {
    if (!MainApp::getInstance()->GetSettings().thingyverseApiKey.empty()) {
        loadData();
    }
}

tbThingLoadedEvent::tbThingLoadedEvent(int buttonIndex, const wxBitmap &bitmap, const wxString &title) : buttonIndex(
        buttonIndex),
                                                                                                         bitmap(bitmap),
                                                                                                         title(title),
                                                                                                         wxThreadEvent(
                                                                                                                 tbEVT_THING_LOADED) {}

tbThingPagingDoneEvent::tbThingPagingDoneEvent(int newPage, int loadedThings) : newPage(newPage),
                                                                                loadedThings(loadedThings),
                                                                                wxThreadEvent(
                                                                                        tbEVT_THING_PAGING_DONE) {}
