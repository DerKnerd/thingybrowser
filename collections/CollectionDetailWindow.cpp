//
// Created by imanuel on 15.05.22.
//

#include "CollectionDetailWindow.h"

#include <utility>
#include "../helper.h"
#include "../MainApp.h"
#include "wx/mstream.h"
#include "wx/wrapsizer.h"
#include "../things/ThingDetailWindow.h"

CollectionDetailWindow::CollectionDetailWindow(wxWindow *parent, unsigned long long int collectionId,
                                               unsigned long long int thingCount) : wxPanel(parent),
                                                                                    collectionId(collectionId) {
    auto contentSizer = new wxFlexGridSizer(2, 0, 5, 5);
    contentSizer->AddGrowableRow(1);
    contentSizer->AddGrowableCol(0);

    this->title = new wxStaticText(this, wxID_ANY, "", wxDefaultPosition, wxSize(-1, -1));
    this->title->SetFont({20, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD});
    contentSizer->Add(this->title, wxSizerFlags().Expand().Border(wxALL, WXC_FROM_DIP(5)));

    SetSizer(contentSizer);
    SetName("CollectionsWindow");
    SetSize(wxDLG_UNIT(this, wxSize(-1, -1)));
    auto thingListSizer = new wxWrapSizer(wxHORIZONTAL);
    auto scrolledWindow = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxVSCROLL);
    scrolledWindow->SetSizer(thingListSizer);
    scrolledWindow->SetScrollRate(5, 5);
    contentSizer->Add(scrolledWindow, wxSizerFlags().Expand().Border(wxALL, WXC_FROM_DIP(5)));

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
    auto apiKey = MainApp::getInstance()->GetSettings().thingyverseApiKey;

    Bind(cwEVT_THING_LOADED, [this](const cwThingLoadedEvent &event) {
        auto bmp = event.bitmap;
        auto idx = event.buttonIndex;
        auto id = event.id;
        thingButtons[idx]->SetBitmap(bmp);
        thingButtons[idx]->SetLabel(event.title);
        ids[idx] = id;
        thingButtons[idx]->Show();
    });
    Bind(cwEVT_COLLECTION_LOADED, [this](const cwCollectionLoadedEvent &event) {
        auto collection = event.collection;
        title->SetLabel("#" + std::to_string(collection.id) + " - " + collection.name + _(" by ") +
                        collection.creator.username);
        Bind(wxEVT_MENU, [event](wxCommandEvent &) {
            wxLaunchDefaultBrowser(event.collection.absoluteUrl);
        }, CollectionsWindowOpenOnThingiverse);
    });

    std::thread([](wxEvtHandler *sink, unsigned long long collectionId, const std::string &apiKey) {
        auto client = thingy::ThingiverseClient(apiKey);
        try {
            auto things = client.getThingsByCollection(collectionId);
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
                wxQueueEvent(sink, new cwThingLoadedEvent(i, bmp, thing.name, thing.id));
            }
            wxQueueEvent(sink, new cwThingsLoadedEvent());
        } catch (thingy::ThingiverseException &e) {
            wxLogStatus(e.what());
        } catch (std::exception &e) {
            wxLogStatus(e.what());
        }
    }, this, collectionId, apiKey).detach();
    std::thread([](wxEvtHandler *sink, unsigned long long collectionId, const std::string &apiKey) {
        auto client = thingy::ThingiverseClient(apiKey);
        auto collection = client.getCollection(collectionId);
        wxQueueEvent(sink, new cwCollectionLoadedEvent(collection));
    }, this, collectionId, apiKey).detach();
}

cwThingLoadedEvent::cwThingLoadedEvent(int buttonIndex, const wxBitmap &bitmap, const wxString &title,
                                       unsigned long long int id) : buttonIndex(buttonIndex), bitmap(bitmap),
                                                                    title(title), id(id),
                                                                    wxThreadEvent(cwEVT_THING_LOADED) {}

cwThingsLoadedEvent::cwThingsLoadedEvent() : wxThreadEvent(cwEVT_THINGS_LOADED) {}

cwCollectionLoadedEvent::cwCollectionLoadedEvent(thingy::entities::Collection collection) : collection(
        std::move(collection)), wxThreadEvent(cwEVT_COLLECTION_LOADED) {}