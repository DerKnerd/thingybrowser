//
// Created by imanuel on 15.04.22.
//

#include "ThingsPage.h"
#include "../../helper.h"
#include "wx/mstream.h"
#include "../../things/ThingDetailWindow.h"
#include <libthingy.h>
#include "../../MainApp.h"

void ThingsPage::internalLoad(int page, const wxString &apiKey, wxEvtHandler *sink, const wxString &keyword) {
    size_t thingCount = 0;
    try {
        auto client = thingy::ThingiverseClient(apiKey);
        auto things = client.getThings(page, 20, keyword, thingy::SortBy::Popular);

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
                    img.Scale(width, height, wxIMAGE_QUALITY_HIGH).Size(wxSize(WXC_FROM_DIP(240), WXC_FROM_DIP(240)),
                                                                        wxDefaultPosition));
            wxQueueEvent(sink, new tbLoadedEvent(i, bmp, thing.name, thing.id));
        }
        thingCount = things.size();
    } catch (thingy::ThingiverseException &e) {
        wxLogStatus(e.what());
    } catch (std::exception &e) {
        wxLogStatus(e.what());
    }
    wxQueueEvent(sink, new tbPagingDoneEvent(thingCount));
}

ThingsPage::ThingsPage(wxWindow *parent) : tbButtonGridPage(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0,
                                                            _("Things")) {
}

void ThingsPage::handleClick(int idx) {
    auto thingId = ids[idx];
    auto caption = tiles[idx]->GetLabel();
    MainApp::getInstance()->getMainWindow()->addThingPage(thingId, caption);
}
