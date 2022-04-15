//
// Created by imanuel on 15.04.22.
//

#ifndef THINGYBROWSER_HELPER_H
#define THINGYBROWSER_HELPER_H

#ifdef WXC_FROM_DIP
#undef WXC_FROM_DIP
#endif
#if wxVERSION_NUMBER >= 3100
#define WXC_FROM_DIP(x) wxWindow::FromDIP(x, NULL)
#else
#define WXC_FROM_DIP(x) x
#endif

#endif //THINGYBROWSER_HELPER_H
