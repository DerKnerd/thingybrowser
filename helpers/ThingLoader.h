//
// Created by imanuel on 19.05.22.
//

#ifndef THINGYBROWSER_THINGLOADER_H
#define THINGYBROWSER_THINGLOADER_H

#include <functional>
#include <string>
#include <libthingy.h>

#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include "wx/wx.h"

#endif

class ThingLoader {
public:
    static void downloadThingFilesAndImages(wxWindow *sink, const thingy::entities::Thing &thing,
                                            const std::string &path, const std::string &apiKey);
};


#endif //THINGYBROWSER_THINGLOADER_H
