//
// Created by imanuel on 15.04.22.
//

#include "DesignersPage.h"
#include "../../helper.h"
#include "wx/mstream.h"
#include <libthingy.h>

void DesignersPage::internalLoad(int page, const wxString &apiKey, wxEvtHandler *sink, const wxString &keyword) {
    size_t designerCount = 0;
    try {
        auto client = thingy::ThingiverseClient(apiKey);
        auto users = client.getUsers(page, 20, keyword);

        for (auto i = 0; i < users.size(); ++i) {
            auto user = users.at(i);
            auto httpClient = httplib::Client("cdn.thingiverse.com");
            auto response = httpClient.Get(user.thumbnail.c_str());
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
            wxQueueEvent(sink, new tbLoadedEvent(i, bmp, user.fullName));
        }
        designerCount = users.size();
    } catch (thingy::ThingiverseException &e) {
        wxLogStatus(e.what());
    } catch (std::exception &e) {
        wxLogStatus(e.what());
    }
    wxQueueEvent(sink, new tbPagingDoneEvent(designerCount));
}

DesignersPage::DesignersPage(wxWindow *parent) : tbButtonGridPage(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0,
                                                                  _("Designers")) {
}

void DesignersPage::handleClick(int idx) {
}
