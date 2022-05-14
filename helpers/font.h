//
// Created by imanuel on 24.04.22.
//

#ifndef THINGYBROWSER_FONT_H
#define THINGYBROWSER_FONT_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include "wx/wx.h"

#endif

wxFont getTitleFont() {
    return {20, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD};
}

#endif //THINGYBROWSER_FONT_H
