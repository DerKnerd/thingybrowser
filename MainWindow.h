//
// Created by imanuel on 13.03.22.
//

#ifndef THINGYBROWSER_MAINWINDOW_H
#define THINGYBROWSER_MAINWINDOW_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif

#include <coroutine>
#include <wx/filesys.h>
#include <wx/fs_mem.h>
#include <wx/mstream.h>

#ifdef WXC_FROM_DIP
#undef WXC_FROM_DIP
#endif
#if wxVERSION_NUMBER >= 3100
#define WXC_FROM_DIP(x) wxWindow::FromDIP(x, NULL)
#else
#define WXC_FROM_DIP(x) x
#endif
struct promise;
struct coroutine : std::coroutine_handle<promise> {
    using promise_type = struct promise;
};

struct promise {
    coroutine get_return_object() { return {coroutine::from_promise(*this)}; }

    std::suspend_always initial_suspend() noexcept { return {}; }

    std::suspend_never final_suspend() noexcept { return {}; }

    void return_void() {}

    void unhandled_exception() {}
};

enum MainWindowActions {
    MainWindowActionLoadImage
};


class MainWindow : public wxFrame {
private:
    wxStaticBitmap *sampleImage;
    wxToolBar *toolbar;

    void loadData(wxCommandEvent &event);

public:
    MainWindow();
};


#endif //THINGYBROWSER_MAINWINDOW_H
