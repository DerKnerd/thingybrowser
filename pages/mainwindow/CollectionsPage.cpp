//
// Created by imanuel on 15.04.22.
//

#include <libthingy.h>
#include "CollectionsPage.h"
#include "wx/mstream.h"
#include "../../helper.h"
#include "../../collections/CollectionsWindow.h"

void CollectionsPage::internalLoad(int page, const wxString &apiKey, wxEvtHandler *sink, const wxString &keyword) {
    size_t collectionCount = 0;
    try {
        auto client = thingy::ThingiverseClient(apiKey);
        auto collections = client.getCollections(page, 20, keyword);

        for (auto i = 0; i < collections.size(); ++i) {
            auto collection = collections.at(i);
            auto httpClient = httplib::Client("cdn.thingiverse.com");
            auto response = httpClient.Get(collection.thumbnail.c_str());
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
                    img.Scale(width, height, wxIMAGE_QUALITY_HIGH).Size(wxSize(WXC_FROM_DIP(240), WXC_FROM_DIP(240)),
                                                                        wxDefaultPosition));
            wxQueueEvent(sink, new tbLoadedEvent(i, bmp, collection.name, collection.id));
            collectionItemCount.emplace_back(collection.count);
        }
        collectionCount = collections.size();
    } catch (thingy::ThingiverseException &e) {
        wxLogStatus(e.what());
    } catch (std::exception &e) {
        wxLogStatus(e.what());
    }
    wxQueueEvent(sink, new tbPagingDoneEvent(collectionCount));
}

CollectionsPage::CollectionsPage(wxWindow *parent) : tbButtonGridPage(parent, wxID_ANY, wxDefaultPosition,
                                                                      wxDefaultSize, 0,
                                                                      _("Collections")) {
}

void CollectionsPage::handleClick(int idx) {
    auto collectionId = ids[idx];
    auto window = new CollectionsWindow(nullptr, collectionId, tiles[idx]->GetLabel().ToStdString(), collectionItemCount[idx]);
    window->Show();
}
