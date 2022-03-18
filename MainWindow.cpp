//
// Created by imanuel on 13.03.22.
//

#include "MainWindow.h"
#include <libthingy.h>

void MainWindow::loadData(wxCommandEvent &event) {
    auto client = thingy::ThingiverseClient("10e4e2ef41ebdb349b72975474c5f2e0");
    auto thing = client.getThing(2064029);
    auto httpClient = httplib::Client("cdn.thingiverse.com");
    auto response = httpClient.Get(thing.thumbnail.c_str());
    auto stream = wxMemoryInputStream(response->body.c_str(), response->body.length());
    auto img = wxImage(stream);
    auto height = static_cast<int>(static_cast<double>(img.GetHeight()) / (static_cast<double>(img.GetWidth()) / 240.0));
    auto bmp = wxBitmap(img.Scale(240, height));
    sampleImage->SetBitmap(bmp);
}

MainWindow::MainWindow() : wxFrame(nullptr, wxID_ANY, _("Filament Manager"), wxDefaultPosition, wxSize(1280, 600),
                                   wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN) {
    wxInitAllImageHandlers();
    wxFileSystem::AddHandler(new wxMemoryFSHandler());
    toolbar = CreateToolBar(wxTB_HORIZONTAL | wxTB_HORZ_LAYOUT | wxTB_TEXT | wxTB_NOICONS);
    toolbar->AddTool(MainWindowActions::MainWindowActionLoadImage, _("Bild laden"), wxNullBitmap);
    toolbar->Realize();

    auto panel = new wxPanel(this);
    auto contentSizer = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(contentSizer);

    sampleImage = new wxStaticBitmap(panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(240, 240),
                                     wxCLIP_CHILDREN);
    contentSizer->Add(sampleImage, 0, 0, WXC_FROM_DIP(5));

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

    Bind(wxEVT_MENU, &MainWindow::loadData, this, MainWindowActions::MainWindowActionLoadImage);
}
