//
// Created by imanuel on 15.05.22.
//

#include "CollectionsWindow.h"

#include <utility>
#include "../helper.h"
#include "../MainApp.h"
#include "wx/mstream.h"
#include "wx/wrapsizer.h"
#include "../things/ThingsWindow.h"
#include "../helpers/ThingLoader.h"

CollectionsWindow::CollectionsWindow(wxWindow *parent, unsigned long long int collectionId,
                                     unsigned long long int thingCount) : wxFrame(
        parent, wxID_ANY, "", wxDefaultPosition, wxSize(1280, 800),
        wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN) {
    toolbar = CreateToolBar(wxTB_HORIZONTAL | wxTB_HORZ_LAYOUT | wxTB_TEXT | wxTB_NOICONS);
    toolbar->AddTool(CollectionsWindowActions::CollectionsWindowDownloadThings, _("Download all thing files"),
                     wxNullBitmap);
    toolbar->AddTool(CollectionsWindowActions::CollectionsWindowOpenOnThingiverse, _("Open on thingiverse"),
                     wxNullBitmap);
    toolbar->Realize();

    toolbar->EnableTool(CollectionsWindowDownloadThings, false);
    toolbar->EnableTool(CollectionsWindowOpenOnThingiverse, false);

    CreateStatusBar();

    panel = new wxPanel(this, wxID_ANY);
    auto contentSizer = new wxFlexGridSizer(2, 0, 5, 5);
    contentSizer->AddGrowableRow(1);
    contentSizer->AddGrowableCol(0);

    this->title = new wxStaticText(panel, wxID_ANY, "", wxDefaultPosition, wxSize(-1, -1));
    this->title->SetFont({20, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD});
    contentSizer->Add(this->title, wxSizerFlags().Expand().Border(wxALL, WXC_FROM_DIP(5)));

    SetName("CollectionsWindow");
    SetSize(wxDLG_UNIT(this, wxSize(-1, -1)));
    panel->SetSizer(contentSizer);
    auto thingListSizer = new wxWrapSizer(wxHORIZONTAL);
    auto scrolledWindow = new wxScrolledWindow(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxVSCROLL);
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
            auto window = new ThingsWindow(nullptr, thingId);
            window->Show();
        });
    }

    if (GetSizer()) {
        GetSizer()->Fit(panel);
    }
    if (GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
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
        toolbar->AddTool(CollectionsWindowGoToDesigner, _("More from ") + event.collection.creator.username,
                         wxNullBitmap);
        toolbar->EnableTool(CollectionsWindowOpenOnThingiverse, true);
        auto collection = event.collection;
        this->SetTitle(
                "#" + std::to_string(collection.id) + " - " + collection.name + _(" by ") +
                collection.creator.username);
        this->title->SetLabel("#" + std::to_string(collection.id) + " - " + collection.name + _(" by ") +
                              collection.creator.username);
        Bind(wxEVT_MENU, [event](wxCommandEvent &) {
            wxLaunchDefaultBrowser(event.collection.absoluteUrl);
        }, CollectionsWindowOpenOnThingiverse);
    });
    Bind(cwEVT_LOG_MESSAGE, [this](cwLogMessageEvent &event) {
        GetStatusBar()->SetStatusText(event.message);
    });
    Bind(twEVT_LOG_MESSAGE, [this](twLogMessageEvent &event) {
        GetStatusBar()->SetStatusText(event.message);
    });
    Bind(cwEVT_THINGS_LOADED, [this](cwThingsLoadedEvent &) {
        toolbar->EnableTool(CollectionsWindowDownloadThings, true);
    });
    Bind(cwEVT_THING_DOWLOADED, [this](cwThingDownloadedEvent &event) {
        if (downloadProgress != nullptr && downloadProgress->GetValue() < downloadProgress->GetRange()) {
            downloadProgress->Update(downloadProgress->GetValue() + 1, event.message);
        }
    });
    Bind(cwEVT_THING_DOWLOADING, [this](cwThingDownloadingEvent &event) {
        if (downloadProgress != nullptr) {
            downloadProgress->Update(downloadProgress->GetValue(), event.message);
        }
    });
    Bind(wxEVT_MENU, [this, apiKey](wxCommandEvent &) {
        auto fileOpenDialog = new wxDirDialog(this, _("Save things"), wxEmptyString, wxDD_DEFAULT_STYLE);
        fileOpenDialog->Bind(wxEVT_WINDOW_MODAL_DIALOG_CLOSED, [this, apiKey](const wxWindowModalDialogEvent &event) {
            if (event.GetReturnCode() == wxID_OK) {
                downloadProgress = new wxProgressDialog(_("Download progress"), _("Downloading files"),
                                                        std::count_if(ids.begin(), ids.end(),
                                                                      [](unsigned long long id) {
                                                                          return id != 0ULL;
                                                                      }), this, wxPD_AUTO_HIDE);
                auto dialog = dynamic_cast<wxDirDialog *>(event.GetDialog());
                std::thread([](wxWindow *sink, std::vector<unsigned long long> ids,
                               std::vector<wxBitmapButton *> thingButtons, const wxString &path,
                               const std::string &apiKey) {
                    auto cdnClient = httplib::Client("https://api.thingiverse.com");
                    cdnClient.set_read_timeout(5 * 60);
                    cdnClient.set_connection_timeout(5 * 60);
                    cdnClient.set_bearer_token_auth(apiKey.c_str());
                    cdnClient.set_follow_location(true);

                    auto apiClient = httplib::Client("https://api.thingiverse.com");
                    apiClient.set_read_timeout(5 * 60);
                    apiClient.set_connection_timeout(5 * 60);
                    apiClient.set_bearer_token_auth(apiKey.c_str());
                    apiClient.set_follow_location(true);
                    for (int idx = 0; idx < ids.size(); ++idx) {
                        if (ids[idx] == 0) { continue; }
                        auto thing = thingy::ThingiverseClient(apiKey).getThing(ids[idx]);
                        wxQueueEvent(sink, new cwThingDownloadingEvent("Downloading thing " + thing.name));
                        ThingLoader::downloadThingFilesAndImages(sink, thing,
                                                                 path + "/" + std::to_string(thing.id) + " - " +
                                                                 thing.name + "/", apiKey);
                        wxQueueEvent(sink, new cwThingDownloadedEvent("Downloaded thing " + thing.name));
                    }
                }, this, ids, thingButtons, dialog->GetPath(), apiKey).detach();
            }
        });

        fileOpenDialog->ShowWindowModal();
    }, CollectionsWindowDownloadThings);

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
                wxQueueEvent(sink,
                             new cwThingLoadedEvent(i, bmp, thing.name, thing.id));
            }
            wxQueueEvent(sink, new cwThingsLoadedEvent());
        } catch (thingy::ThingiverseException &e) {
            wxQueueEvent(sink, new cwLogMessageEvent(e.what()));
        } catch (std::exception &e) {
            wxQueueEvent(sink, new cwLogMessageEvent(e.what()));
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

cwLogMessageEvent::cwLogMessageEvent(const wxString &message) : message(message), wxThreadEvent(cwEVT_LOG_MESSAGE) {}

cwThingDownloadedEvent::cwThingDownloadedEvent(std::string message) : wxThreadEvent(cwEVT_THING_DOWLOADED),
                                                                      message(std::move(message)) {}

cwThingDownloadingEvent::cwThingDownloadingEvent(std::string message) : wxThreadEvent(cwEVT_THING_DOWLOADING),
                                                                        message(std::move(message)) {}

cwThingsLoadedEvent::cwThingsLoadedEvent() : wxThreadEvent(cwEVT_THINGS_LOADED) {}

cwCollectionLoadedEvent::cwCollectionLoadedEvent(thingy::entities::Collection collection) : collection(
        std::move(collection)), wxThreadEvent(cwEVT_COLLECTION_LOADED) {}
