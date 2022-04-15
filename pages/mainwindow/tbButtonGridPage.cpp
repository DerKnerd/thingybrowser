//
// Created by imanuel on 15.04.22.
//

#include <thread>
#include "tbButtonGridPage.h"
#include "wx/fs_mem.h"
#include "wx/filesys.h"
#include "wx/wrapsizer.h"
#include "../../helper.h"
#include "../../MainApp.h"

tbButtonGridPage::tbButtonGridPage(wxWindow *parent, wxWindowID winid, const wxPoint &pos, const wxSize &size,
                                   long style, const wxString &name) : tbDataPage(parent, winid, pos, size, style,
                                                                                  name) {
    auto sizer = new wxFlexGridSizer(2, 0, 5, 5);
    this->SetSizer(sizer);
    sizer->AddGrowableRow(0, 1);
    sizer->AddGrowableCol(0, 1);
    sizer->SetFlexibleDirection(wxHORIZONTAL);

    auto thingListSizer = new wxWrapSizer(wxHORIZONTAL);
    scrolledWindow = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxVSCROLL);
    scrolledWindow->SetSizer(thingListSizer);
    scrolledWindow->SetScrollRate(5, 5);

    sizer->Add(scrolledWindow, wxSizerFlags().Proportion(1).Expand().Border(wxALL, WXC_FROM_DIP(5)));

    tiles = std::vector<wxBitmapButton *>(20);
    for (auto i = 0; i < 20; ++i) {
        tiles[i] = new wxBitmapButton(scrolledWindow, wxID_ANY, wxNullBitmap, wxDefaultPosition,
                                      wxSize(WXC_FROM_DIP(240), WXC_FROM_DIP(240)));
        thingListSizer->Add(tiles[i], wxSizerFlags()
                .Proportion(1)
                .Expand()
                .Border(wxALL, WXC_FROM_DIP(5))
                .ReserveSpaceEvenIfHidden());
        tiles[i]->Hide();
    }
    scrolledWindow->SetVirtualSize(wxSize(GetClientSize().x, WXC_FROM_DIP(240) * 4 + WXC_FROM_DIP(5) * 3));

    auto pagingSizer = new wxGridSizer(0, 2, 0, 0);
    previousPage = new wxButton(this, wxID_ANY, _("Previous page"));
    previousPage->Enable(false);
    nextPage = new wxButton(this, wxID_ANY, _("Next page"));
    nextPage->Enable(false);

    pagingSizer->Add(previousPage, wxSizerFlags().Border(wxALL, WXC_FROM_DIP(5)).Proportion(1));
    pagingSizer->Add(nextPage,
                     wxSizerFlags().Proportion(1).Border(wxALL, WXC_FROM_DIP(5)).Expand().Align(wxALIGN_RIGHT));

    sizer->Add(pagingSizer, wxSizerFlags().Proportion(1).Border(wxALL, WXC_FROM_DIP(5)).Expand());
    this->SetSizer(sizer);

    Bind(tbEVT_LOADED, [this](tbLoadedEvent &event) {
        this->tiles[event.buttonIndex]->SetBitmap(event.bitmap);
        this->tiles[event.buttonIndex]->SetLabel(event.title);
        this->tiles[event.buttonIndex]->Show();
    });
    Bind(tbEVT_PAGING_DONE, [this](tbPagingDoneEvent &event) {
        this->previousPage->Enable(page > 1);
        this->nextPage->Enable(event.loadedThings == 20);
    });
    scrolledWindow->Bind(wxEVT_SIZE, [this](wxSizeEvent &event) {
        this->SetVirtualSize(wxSize(this->GetClientSize().x, -1));
    });
    scrolledWindow->Bind(wxEVT_SCROLL_CHANGED, [this](wxScrollEvent &event) {
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

void tbButtonGridPage::loadData() {
    this->previousPage->Enable(false);
    this->nextPage->Enable(false);
    for (auto button: tiles) {
        button->Hide();
    }
    auto apiKey = MainApp::getInstance()->GetSettings().thingyverseApiKey;
    auto th = std::thread([this](int page, const wxString &apiKey, wxEvtHandler *sink) {
        internalLoad(page, apiKey, sink);
    }, page, apiKey, this);
    th.detach();
}

tbLoadedEvent::tbLoadedEvent(int buttonIndex, const wxBitmap &bitmap, const wxString &title) : buttonIndex(buttonIndex),
                                                                                               bitmap(bitmap),
                                                                                               title(title),
                                                                                               wxThreadEvent(
                                                                                                       tbEVT_LOADED) {}

tbPagingDoneEvent::tbPagingDoneEvent(size_t loadedThings) : loadedThings(loadedThings),
                                                            wxThreadEvent(tbEVT_PAGING_DONE) {}
