//
// Created by imanuel on 15.04.22.
//

#ifndef THINGYBROWSER_COLLECTIONSPAGE_H
#define THINGYBROWSER_COLLECTIONSPAGE_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include "wx/wx.h"

#endif

#include "tbButtonGridPage.h"

class CollectionsPage : public tbButtonGridPage {
public:
    void internalLoad(int page, const wxString &apiKey, wxEvtHandler *sink, const wxString &keyword) override;

    explicit CollectionsPage(wxWindow *parent);

private:
    void handleClick(int idx) override;
    std::vector<unsigned long long> collectionItemCount;
};


#endif //THINGYBROWSER_COLLECTIONSPAGE_H
