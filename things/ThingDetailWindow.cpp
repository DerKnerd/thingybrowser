//
// Created by imanuel on 23.04.22.
//

#include "ThingDetailWindow.h"

#include <utility>
#include "../MainApp.h"
#include "../helper.h"
#include "../helpers/font.h"
#include "wx/gbsizer.h"
#include "wx/utils.h"
#include "../helpers/tbNoLinkHtmlWindow.h"
#include "wx/sstream.h"
#include "wx/mstream.h"

ThingDetailWindow::ThingDetailWindow(wxWindow *parent, unsigned long long thingId) : wxPanel(parent), thingId(thingId) {
    auto contentSizer = new wxGridBagSizer(5, 5);

    title = new wxStaticText(this, wxID_ANY, "", wxDefaultPosition, wxSize(-1, -1));
    title->SetFont(getTitleFont());

    createdBy = new wxHyperlinkCtrl(this, wxID_ANY, "More things from designer", " ");
    createdBy->SetFont(getTitleFont());

    contentSizer->Add(title, wxGBPosition(0, 0), wxGBSpan(1, 2),
                      wxSizerFlags().Proportion(1).Border(wxALL, WXC_FROM_DIP(5)).GetFlags());

    details = new twNoLinkHtmlWindow(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxHW_SCROLLBAR_AUTO);
    contentSizer->Add(createdBy, wxGBPosition(1, 1), wxGBSpan(1, 1),
                      wxSizerFlags().Border(wxALL, WXC_FROM_DIP(5)).Left().GetFlags());
    contentSizer->Add(details, wxGBPosition(2, 1), wxGBSpan(1, 1),
                      wxSizerFlags().Border(wxALL, WXC_FROM_DIP(5)).Expand().GetFlags());

    contentSizer->AddGrowableCol(1);
    contentSizer->AddGrowableRow(2);

    imageSizer = new wxGridBagSizer(5, 5);
    scrolledWindow = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxSize(500, -1), wxVSCROLL);
    scrolledWindow->SetSizer(imageSizer);
    scrolledWindow->SetScrollRate(5, 5);
    contentSizer->Add(scrolledWindow, wxGBPosition(1, 0), wxGBSpan(2, 1),
                      wxSizerFlags().Proportion(1).Expand().Border(wxALL, WXC_FROM_DIP(5)).GetFlags());

    SetName("ThingsWindow");
    this->SetSizer(contentSizer);

    Bind(twEVT_THING_LOADED, [this](twThingLoadedEvent &event) {
        this->thing = event.thing;
        this->displayThing();
        createdBy->Bind(wxEVT_HYPERLINK, [this](wxHyperlinkEvent &event) {
            MainApp::getInstance()->getMainWindow()->addThingsByDesignerPage(thing.creator.id, thing.creator.username);
        });
    });
    Bind(twEVT_IMAGE_LOADED, [this](twImageLoadedEvent &event) {
        if (event.image.IsOk()) {
            auto img = new wxStaticBitmap(scrolledWindow, wxID_ANY, wxNullBitmap);
            img->SetBitmap(event.image);
            this->imageSizer->Add(img, wxGBPosition(imageSizer->GetEffectiveRowsCount() + 1, 0), wxDefaultSpan,
                                  wxSizerFlags().Proportion(1).Expand().Border(wxALL, WXC_FROM_DIP(5)).GetFlags());
        }
    });
    Bind(twEVT_ALL_IMAGES_LOADED, [this](twAllImagesLoadedEvent &event) {
        this->SetVirtualSize(wxSize(this->GetClientSize().x, this->GetClientSize().y + event.height));
        scrolledWindow->SetVirtualSize(
                wxSize(this->GetClientSize().x, scrolledWindow->GetClientSize().y + event.height));
    });

    auto apiKey = MainApp::getInstance()->GetSettings().thingyverseApiKey;
    std::thread([](wxWindow *sink, unsigned long long thingId, const std::string &apiKey) {
        try {
            auto client = thingy::ThingiverseClient(apiKey);
            auto thing = client.getThing(thingId);
            wxQueueEvent(sink, new twThingLoadedEvent(thing));

            auto images = client.getImagesByThing(thingId);
            auto totalHeight = 0;
            for (const auto &image: images) {
                auto httpClient = httplib::Client("cdn.thingiverse.com");
                auto imageSizes = std::find_if(image.sizes.begin(), image.sizes.end(),
                                               [](const thingy::entities::ImageSize &size) {
                                                   if (size.size == "large" && size.type == "display") {
                                                       return true;
                                                   }
                                                   return false;
                                               });
                auto url = image.sizes.front().url;
                if (imageSizes.base() != nullptr) {
                    url = imageSizes.base()->url;
                }
                auto response = httpClient.Get(url.c_str());
                auto stream = wxMemoryInputStream(response->body.c_str(), response->body.length());
                auto img = wxImage(stream);
                auto width = WXC_FROM_DIP(480);
                auto height = static_cast<int>(static_cast<double>(img.GetHeight()) /
                                               (static_cast<double>(img.GetWidth()) / WXC_FROM_DIP(480)));
                auto bmp = wxBitmap(img.Scale(width, height, wxIMAGE_QUALITY_HIGH));
                wxQueueEvent(sink, new twImageLoadedEvent(bmp));
                totalHeight += height + 10;
            }
            wxQueueEvent(sink, new twAllImagesLoadedEvent(totalHeight));
        } catch (std::exception &exception) {
            wxLogStatus(exception.what());
        }
    }, this, thingId, apiKey).detach();
}

void ThingDetailWindow::displayThing() {
    details->SetPage("<html><body>" + thing.detailsHtml + "</body></html>");
    title->SetLabel("#" + std::to_string(thing.id) + " - " + thing.name + _(" by ") + thing.creator.username);
    wxQueueEvent(this, new mwThingLoadedEvent(thing));
}

twThingLoadedEvent::twThingLoadedEvent(thingy::entities::Thing thing) : thing(std::move(thing)),
                                                                        wxThreadEvent(twEVT_THING_LOADED) {}

twImageLoadedEvent::twImageLoadedEvent(const wxBitmap &image) : image(image), wxThreadEvent(twEVT_IMAGE_LOADED) {}

twAllImagesLoadedEvent::twAllImagesLoadedEvent(int height) : height(height), wxThreadEvent(twEVT_ALL_IMAGES_LOADED) {}
