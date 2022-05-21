//
// Created by imanuel on 20.05.22.
//
#include <thread>
#include <libthingy.h>
#include "tbNoLinkHtmlWindow.h"
#include "../MainApp.h"
#include "../things/ThingDetailWindow.h"


void twNoLinkHtmlWindow::OnLinkClicked(const wxHtmlLinkInfo &link) {
    auto href = link.GetHref();
    if (href.Contains("thing:")) {
        auto thingId = href.AfterLast(':');
        auto thingIdAsUll = 0ULL;
        thingId.ToULongLong(&thingIdAsUll);
        auto apiKey = MainApp::getInstance()->GetSettings().thingyverseApiKey;
        auto client = thingy::ThingiverseClient(apiKey);
        auto thing = client.getThing(thingIdAsUll);
        MainApp::getInstance()->getMainWindow()->addThingPage(thing.id, thing.name);
    } else {
        wxLaunchDefaultBrowser(link.GetHref());
    }
}

twNoLinkHtmlWindow::twNoLinkHtmlWindow(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size,
                                       long style, const wxString &name) : wxHtmlWindow(parent, id, pos, size, style,
                                                                                        name) {
}

