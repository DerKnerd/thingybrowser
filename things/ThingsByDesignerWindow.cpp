//
// Created by imanuel on 15.05.22.
//

#include "ThingsByDesignerWindow.h"

#include <utility>
#include "../helper.h"
#include "../MainApp.h"
#include "wx/mstream.h"
#include "wx/wrapsizer.h"
#include "../things/ThingDetailWindow.h"

ThingsByDesignerWindow::ThingsByDesignerWindow(wxWindow *parent, unsigned long long int designerId) : wxPanel(parent),
                                                                                                      designerId(
                                                                                                              designerId) {
    auto contentSizer = new wxFlexGridSizer(2, 0, 5, 5);
    contentSizer->AddGrowableRow(1);
    contentSizer->AddGrowableCol(0);

    this->title = new wxStaticText(this, wxID_ANY, "", wxDefaultPosition, wxSize(-1, -1));
    this->title->SetFont({20, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD});
    contentSizer->Add(this->title, wxSizerFlags().Expand().Border(wxALL, WXC_FROM_DIP(5)));

    SetSizer(contentSizer);
    SetName("ThingsByDesignerWindow");
    SetSize(wxDLG_UNIT(this, wxSize(-1, -1)));
    auto thingListSizer = new wxWrapSizer(wxHORIZONTAL);
    auto scrolledWindow = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxVSCROLL);
    scrolledWindow->SetSizer(thingListSizer);
    scrolledWindow->SetScrollRate(5, 5);
    contentSizer->Add(scrolledWindow, wxSizerFlags().Expand().Border(wxALL, WXC_FROM_DIP(5)));

    auto apiKey = MainApp::getInstance()->GetSettings().thingyverseApiKey;
    auto user = thingy::ThingiverseClient(apiKey).getUser(designerId);
    auto thingCount = user.countOfDesigns;
    title->SetLabel("Things by " + user.username);

    ids = std::vector<unsigned long long>(thingCount);
    thingButtons = std::vector<wxBitmapButton *>(thingCount);
    for (int i = 0; i < thingCount; ++i) {
        thingButtons[i] = new wxBitmapButton(scrolledWindow, wxID_ANY, wxNullBitmap, wxDefaultPosition,
                                             wxSize(WXC_FROM_DIP(240), WXC_FROM_DIP(240)));
        thingListSizer->Add(thingButtons[i], wxSizerFlags()
                .Proportion(1)
                .Expand()
                .Border(wxALL, WXC_FROM_DIP(5))
                .ReserveSpaceEvenIfHidden());
        thingButtons[i]->Hide();
        thingButtons[i]->Bind(wxEVT_BUTTON, [this, i](wxCommandEvent &) {
            auto thingId = ids[i];
            auto caption = thingButtons[i]->GetLabel();
            MainApp::getInstance()->getMainWindow()->addThingPage(thingId, caption);
        });
    }

    Bind(tbdEVT_THING_LOADED, [this](const tbdThingLoadedEvent &event) {
        if (event.bitmap.IsOk()) {
            auto bmp = event.bitmap;
            auto idx = event.buttonIndex;
            auto id = event.id;
            thingButtons[idx]->SetBitmap(bmp);
            thingButtons[idx]->SetLabel(event.title);
            ids[idx] = id;
            thingButtons[idx]->Show();
        }
    });

    std::thread([](wxEvtHandler *sink, const std::string& username, unsigned long long thingCount,
                   const std::string &apiKey) {
        auto client = thingy::ThingiverseClient(apiKey);
        try {
            auto things = client.getThingsByUser(username, 1, thingCount);
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
                auto bmp = wxBitmap(img.Scale(width, height, wxIMAGE_QUALITY_HIGH).Size(
                        wxSize(WXC_FROM_DIP(240), WXC_FROM_DIP(240)), wxDefaultPosition));
                wxQueueEvent(sink, new tbdThingLoadedEvent(i, bmp, thing.name, thing.id));
            }
            wxQueueEvent(sink, new tbdThingsLoadedEvent());
        } catch (thingy::ThingiverseException &e) {
            wxLogStatus(e.what());
        } catch (std::exception &e) {
            wxLogStatus(e.what());
        }
    }, this, user.username, thingCount, apiKey).detach();
}

tbdThingLoadedEvent::tbdThingLoadedEvent(int buttonIndex, const wxBitmap &bitmap, const wxString &title,
                                         unsigned long long int id) : buttonIndex(buttonIndex), bitmap(bitmap),
                                                                      title(title), id(id),
                                                                      wxThreadEvent(tbdEVT_THING_LOADED) {}

tbdThingsLoadedEvent::tbdThingsLoadedEvent() : wxThreadEvent(tbdEVT_THINGS_LOADED) {}

tbdDesignerLoadedEvent::tbdDesignerLoadedEvent(thingy::entities::User designer) : designer(std::move(designer)),
                                                                                  wxThreadEvent(
                                                                                          tbdEVT_DESIGNER_LOADED) {}