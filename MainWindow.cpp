//
// Created by imanuel on 13.03.22.
//

#include "MainWindow.h"
#include "MainApp.h"
#include "pages/mainwindow/tbDataPage.h"
#include "pages/mainwindow/CollectionsPage.h"
#include "pages/mainwindow/DesignersPage.h"
#include "things/ThingDetailWindow.h"
#include "collections/CollectionDetailWindow.h"
#include "pages/mainwindow/ThingsPage.h"
#include "helper.h"

MainWindow::MainWindow() : wxFrame(nullptr, wxID_ANY, _("Thingybrowser - Browse the Thingiverse"), wxDefaultPosition,
                                   wxSize(1280, 800), wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN) {
    auto panel = new wxPanel(this);
    mainWindowToolbar = new wxAuiToolBar(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_HORIZONTAL | wxAUI_TB_TEXT);
    mainWindowToolbar->AddTool(MainWindowActions::MainWindowOpenThingOverview, _("Open thing list"), wxNullBitmap);
    mainWindowToolbar->AddTool(MainWindowActions::MainWindowOpenCollectionOverview, _("Open collection list"),
                               wxNullBitmap);
    mainWindowToolbar->AddTool(MainWindowActions::MainWindowOpenDesignerOverview, _("Open designer list"),
                               wxNullBitmap);
    mainWindowToolbar->AddSeparator();
    mainWindowToolbar->AddTool(MainWindowActions::MainWindowOpenThingById, _("Open thing by id"), wxNullBitmap);
    mainWindowToolbar->AddTool(MainWindowActions::MainWindowOpenThingByUrl, _("Open thing by URL"), wxNullBitmap);
    mainWindowToolbar->AddSeparator();
    mainWindowToolbar->AddTool(MainWindowActions::MainWindowOpenCollectionById, _("Open collection by id"),
                               wxNullBitmap);
    mainWindowToolbar->Realize();

    thingToolbar = new wxAuiToolBar(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                    wxAUI_TB_HORIZONTAL | wxAUI_TB_TEXT);
    thingToolbar->AddTool(ThingsWindowDownloadThing, _("Download thing files"), wxNullBitmap);
    thingToolbar->AddTool(ThingsWindowOpenOnThingiverse, _("Open on thingiverse"), wxNullBitmap);
    thingToolbar->Realize();

    collectionToolbar = new wxAuiToolBar(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_HORIZONTAL | wxAUI_TB_TEXT);
    collectionToolbar->AddTool(CollectionsWindowDownloadThings, _("Download things"), wxNullBitmap);
    collectionToolbar->AddTool(CollectionsWindowOpenOnThingiverse, _("Open on thingiverse"), wxNullBitmap);
    collectionToolbar->Realize();

    auiManager.SetManagedWindow(panel);
    SetMinSize(wxSize(1280, 800));

    auto menuBar = new wxMenuBar();
    this->SetMenuBar(menuBar);

    CreateStatusBar();

    auto fileMenu = new wxMenu();
    fileMenu->Append(MainWindowActions::MainWindowThingybrowserSettings, _("Settings"));
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, _("Exit"));

    auto thingMenu = new wxMenu();
    thingMenu->Append(MainWindowActions::MainWindowOpenThingById, _("Open thing by id"));
    thingMenu->Append(MainWindowActions::MainWindowOpenThingByUrl, _("Open thing by URL"));

    auto collectionMenu = new wxMenu();
    collectionMenu->Append(MainWindowActions::MainWindowOpenCollectionById, _("Open collection by id"));

    menuBar->Append(fileMenu, _("File"));
    menuBar->Append(thingMenu, _("Things"));
    menuBar->Append(collectionMenu, _("Collections"));

    contentNotebook = new wxAuiNotebook(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, -1),
                                        wxAUI_NB_CLOSE_BUTTON | wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT |
                                        wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_MIDDLE_CLICK_CLOSE |
                                        wxAUI_NB_WINDOWLIST_BUTTON);
    contentNotebook->AddPage(new ThingsPage(contentNotebook), _("Things"), true);
    contentNotebook->AddPage(new CollectionsPage(contentNotebook), _("Collections"));
    contentNotebook->AddPage(new DesignersPage(contentNotebook), _("Designers"));

    logOutput = new wxListBox(panel, wxID_ANY);
    downloadProgress = new wxGauge(panel, wxID_ANY, 1);

    auiManager.AddPane(mainWindowToolbar, wxAuiPaneInfo()
            .Name("mainWindowToolbar")
            .Caption(_("Thingbrowser actions"))
            .ToolbarPane()
            .Top()
            .Gripper());
    auiManager.AddPane(thingToolbar, wxAuiPaneInfo()
            .Name("thingToolbar")
            .Caption(_("Thing actions"))
            .ToolbarPane()
            .Top()
            .Hide()
            .Gripper());
    auiManager.AddPane(collectionToolbar, wxAuiPaneInfo()
            .Name("collectionToolbar")
            .Caption(_("Collection actions"))
            .ToolbarPane()
            .Top()
            .Hide()
            .Gripper());
    auiManager.AddPane(logOutput, wxAuiPaneInfo()
            .Name("logOutput")
            .Bottom()
            .Caption(_("Download log"))
            .Layer(1)
            .Position(1)
            .Row(2)
            .CloseButton(false)
            .MinSize(-1, WXC_FROM_DIP(150))
            .Show());
    auiManager.AddPane(downloadProgress, wxAuiPaneInfo()
            .Name("downloadProgress")
            .Bottom()
            .CloseButton(false)
            .CaptionVisible(false)
            .Layer(1)
            .Position(1)
            .Row(1)
            .Gripper(false)
            .DockFixed()
            .Show());
    auiManager.AddPane(contentNotebook, wxAuiPaneInfo()
            .Name("content_notebook")
            .CenterPane()
            .PaneBorder(false)
            .Show());
    auiManager.Update();

    Bind(wxEVT_MENU, [this](wxCommandEvent &) {
        contentNotebook->AddPage(new ThingsPage(contentNotebook), _("Things"), true);
    }, MainWindowOpenThingOverview);
    Bind(wxEVT_MENU, [this](wxCommandEvent &) {
        contentNotebook->AddPage(new CollectionsPage(contentNotebook), _("Collections"), true);
    }, MainWindowOpenCollectionOverview);
    Bind(wxEVT_MENU, [this](wxCommandEvent &) {
        contentNotebook->AddPage(new DesignersPage(contentNotebook), _("Designers"), true);
    }, MainWindowOpenDesignerOverview);
    Bind(wxEVT_SHOW, &MainWindow::handleShow, this);
    Bind(wxEVT_MENU, [this](wxCommandEvent &) {
        MainApp::getInstance()->DismissPreferencesEditor();
        Close(true);
    }, wxID_EXIT);
    Bind(wxEVT_MENU, [this](wxCommandEvent &) {
        MainApp::getInstance()->ShowPreferencesEditor(this);
    }, MainWindowThingybrowserSettings);
    Bind(wxEVT_MENU, [this](wxCommandEvent &) {
        auto dialog = wxTextEntryDialog(this, "", _("Please enter a collection ID"), "", wxOK | wxCANCEL);
        dialog.SetTextValidator(wxFILTER_DIGITS);
        if (dialog.ShowModal() == wxID_OK) {
            auto collectionId = 0ULL;
            dialog.GetValue().ToULongLong(&collectionId);
            auto collection = thingy::ThingiverseClient(
                    MainApp::getInstance()->GetSettings().thingyverseApiKey).getCollection(collectionId);
            addCollectionPage(collection.id, collection.count, collection.name);
        }
    }, MainWindowOpenCollectionById);
    Bind(wxEVT_MENU, [this](wxCommandEvent &) {
        auto dialog = wxTextEntryDialog(this, "", _("Please enter a thing ID"), "", wxOK | wxCANCEL);
        dialog.SetTextValidator(wxFILTER_DIGITS);
        if (dialog.ShowModal() == wxID_OK) {
            auto thingId = 0ULL;
            dialog.GetValue().ToULongLong(&thingId);
            auto apiKey = MainApp::getInstance()->GetSettings().thingyverseApiKey;
            auto client = thingy::ThingiverseClient(apiKey);
            auto thing = client.getThing(thingId);
            addThingPage(thing.id, thing.name);
        }
    }, MainWindowOpenThingById);
    Bind(wxEVT_MENU, [this](wxCommandEvent &) {
        auto dialog = wxTextEntryDialog(this, "", _("Please enter a thing URL"), "", wxOK | wxCANCEL);
        if (dialog.ShowModal() == wxID_OK) {
            auto thingId = 0ULL;
            auto result = dialog.GetValue().AfterLast(':');
            result.ToULongLong(&thingId);
            auto apiKey = MainApp::getInstance()->GetSettings().thingyverseApiKey;
            auto client = thingy::ThingiverseClient(apiKey);
            auto thing = client.getThing(thingId);
            addThingPage(thing.id, thing.name);
        }
    }, MainWindowOpenThingByUrl);

    Bind(mwEVT_LOG_MESSAGE, [this](mwLogMessageEvent &event) {
        log(event.message);
    });
    Bind(mwEVT_FILE_DOWNLOADED, [this](mwFileDownloadedEvent &event) {
        log(event.message);
    });
    Bind(mwEVT_FILE_DOWNLOADING, [this](mwFileDownloadingEvent &event) {
        log(event.message);
    });
    Bind(mwEVT_FILES_COUNTED, [this](mwFilesCountedEvent &event) {
        log(std::to_string(event.count) + " files to download");
    });
    Bind(mwEVT_THING_DOWLOADED, [this](mwThingDownloadedEvent &event) {
        log(event.message);
        thingsDownloaded++;
        downloadProgress->SetValue(thingsDownloaded);
    });
    Bind(mwEVT_THING_DOWLOADING, [this](mwThingDownloadingEvent &event) {
        log(event.message);
    });
    Bind(mwEVT_THINGS_COUNTED, [this](mwThingsCountedEvent &event) {
        totalThingsToDownload += event.thingCount;
        downloadProgress->SetRange(totalThingsToDownload);
    });

    auto apiKey = MainApp::getInstance()->GetSettings().thingyverseApiKey;
    Bind(wxEVT_MENU, [this, apiKey](wxCommandEvent &) {
        auto fileOpenDialog = new wxDirDialog(this, _("Save thing"), wxEmptyString, wxDD_DEFAULT_STYLE);
        fileOpenDialog->Bind(wxEVT_WINDOW_MODAL_DIALOG_CLOSED, [this, apiKey](const wxWindowModalDialogEvent &event) {
            if (event.GetReturnCode() == wxID_OK) {
                auto dialog = dynamic_cast<wxDirDialog *>(event.GetDialog());
                totalThingsToDownload++;
                downloadProgress->SetRange(totalThingsToDownload);
                std::thread([this](const unsigned long long id, const std::string &path, const std::string &apiKey) {
                    this->downloadThingFilesAndImages(id, path, apiKey);
                }, thingId, dialog->GetPath(), apiKey).detach();
            }
        });

        fileOpenDialog->ShowWindowModal();
    }, ThingsWindowDownloadThing);
    Bind(wxEVT_MENU, [this](wxCommandEvent &) {
        wxLaunchDefaultBrowser("https://www.thingiverse.com/thing:" + std::to_string(this->thingId));
    }, ThingsWindowOpenOnThingiverse);
    Bind(wxEVT_MENU, [this, apiKey](wxCommandEvent &) {
        auto fileOpenDialog = new wxDirDialog(this, _("Save things"), wxEmptyString, wxDD_DEFAULT_STYLE);
        fileOpenDialog->Bind(wxEVT_WINDOW_MODAL_DIALOG_CLOSED, [this, apiKey](const wxWindowModalDialogEvent &event) {
            if (event.GetReturnCode() == wxID_OK) {
                auto dialog = dynamic_cast<wxDirDialog *>(event.GetDialog());
                std::thread([this](const wxString &path, const std::string &apiKey, unsigned long long collectionId) {
                    auto cdnClient = httplib::Client("https://api.thingiverse.com");
                    cdnClient.set_read_timeout(5 * 60);
                    cdnClient.set_connection_timeout(5 * 60);
                    cdnClient.set_bearer_token_auth(apiKey.c_str());
                    cdnClient.set_follow_location(true);

                    auto apiClient = httplib::Client("https://api.thingiverse.com");
                    apiClient.set_read_timeout(5 * 60);
                    apiClient.set_connection_timeout(5 * 60);
                    apiClient.set_bearer_token_auth(apiKey.c_str());
                    apiClient.set_follow_location(true);

                    auto thingyClient = thingy::ThingiverseClient(apiKey);
                    auto things = thingyClient.getThingsByCollection(collectionId);
                    wxQueueEvent(this, new mwThingsCountedEvent(things.size()));
                    for (const auto &thing: things) {
                        wxQueueEvent(this, new mwThingDownloadingEvent("Downloading thing " + thing.name));
                        this->downloadThingFilesAndImages(thing.id,
                                                          path + "/" + std::to_string(thing.id) + " - " + thing.name +
                                                          "/", apiKey);
                        wxQueueEvent(this, new mwThingDownloadedEvent("Downloaded thing " + thing.name));
                    }
                }, dialog->GetPath(), apiKey, collectionId).detach();
            }
        });

        fileOpenDialog->ShowWindowModal();
    }, CollectionsWindowDownloadThings);
    Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent &event) {
        if (totalThingsToDownload > thingsDownloaded) {
            if (wxMessageBox(_("Not all things are downloaded, do you still want to close Thingybrowser?"),
                             _("Please confirm exit"), wxICON_WARNING | wxYES_NO | wxNO_DEFAULT) != wxYES) {
                event.Veto();
                return;
            }
        }
        event.Skip();
    });

    contentNotebook->Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, [this](wxAuiNotebookEvent &) {
        if (contentNotebook->GetSelection() != -1) {
            auto nbPage = contentNotebook->GetPage(contentNotebook->GetSelection());
            auto page = dynamic_cast<tbButtonGridPage *>(nbPage);
            if (page != nullptr) {
                page->loadData();
            }
            auto pageAsThingPage = dynamic_cast<ThingDetailWindow *>(nbPage);
            if (pageAsThingPage != nullptr) {
                thingId = pageAsThingPage->thingId;
                auiManager.GetPane(thingToolbar).Show();
            } else {
                auiManager.GetPane(thingToolbar).Hide();
            }

            auto pageAsCollectionPage = dynamic_cast<CollectionDetailWindow *>(nbPage);
            if (pageAsCollectionPage != nullptr) {
                collectionId = pageAsCollectionPage->collectionId;
                auiManager.GetPane(collectionToolbar).Show();
            } else {
                auiManager.GetPane(collectionToolbar).Hide();
            }

            auiManager.Update();
        }
    });
    contentNotebook->Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, [this](wxAuiNotebookEvent &event) {
        if (contentNotebook->GetPageCount() == 1) {
            event.Veto();
            wxMessageBox(_("You cannot close the last tab"));
        }
    });
}

void MainWindow::handleShow(wxShowEvent &event) {
    if (!MainApp::getInstance()->GetSettings().thingyverseApiKey.empty()) {
        loadData();
    } else {
        MainApp::getInstance()->ShowPreferencesEditor(this);
    }
}

void MainWindow::loadData() {
    if (contentNotebook->GetSelection() != -1) {
        auto page = dynamic_cast<tbButtonGridPage *>(contentNotebook->GetPage(contentNotebook->GetSelection()));
        if (page != nullptr) {
            page->initialLoad();
        }
    }
}

void MainWindow::addThingPage(unsigned long long int thingId, const wxString &caption) {
    auto window = new ThingDetailWindow(contentNotebook, thingId);
    window->Bind(mwEVT_THING_LOADED, [this](mwThingLoadedEvent &event) {
        thingToolbar->SetToolLabel(ThingsWindowGoToDesigner, _("More from ") + event.thing.creator.username);
        thingToolbar->Layout();
        auiManager.Update();
    });
    contentNotebook->AddPage(window, caption, true);
}

void MainWindow::addCollectionPage(unsigned long long int collectionId, unsigned long long thingsCount,
                                   const wxString &caption) {
    contentNotebook->AddPage(new CollectionDetailWindow(contentNotebook, collectionId, thingsCount), caption, true);
}

void MainWindow::downloadThingFilesAndImages(unsigned long long thingId, const std::string &path,
                                             const std::string &apiKey) {
    if (!wxFileName::Mkdir(path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL)) {
        return;
    }
    try {
        auto files = thingy::ThingiverseClient(apiKey).getFilesByThing(thingId);
        auto images = thingy::ThingiverseClient(apiKey).getImagesByThing(thingId);
        auto filesCount = std::count_if(files.begin(), files.end(), [](const thingy::entities::File &file) {
            return !file.downloadUrl.empty();
        });
        auto imagesCount = images.size();
        wxQueueEvent(this, new mwFilesCountedEvent(filesCount + imagesCount));

        auto cdnClient = httplib::Client("https://cdn.thingiverse.com");
        cdnClient.set_read_timeout(5 * 60);
        cdnClient.set_connection_timeout(5 * 60);
        cdnClient.set_bearer_token_auth(apiKey.c_str());
        cdnClient.set_follow_location(true);

        auto apiClient = httplib::Client("https://api.thingiverse.com");
        apiClient.set_read_timeout(5 * 60);
        apiClient.set_connection_timeout(5 * 60);
        apiClient.set_bearer_token_auth(apiKey.c_str());
        apiClient.set_follow_location(true);
        for (const auto &file: files) {
            try {
                if (file.downloadUrl.empty()) { continue; }
                wxQueueEvent(this, new mwFileDownloadingEvent("Downloading file " + file.name));
                auto response = apiClient.Get(file.downloadUrl.c_str());
                if (response.error() != httplib::Error::Success) {
                    wxQueueEvent(this, new mwLogMessageEvent("Failed to download file"));
                    wxQueueEvent(this, new mwFileDownloadedEvent("Downloaded file " + file.name));
                    continue;
                }

                if (!wxFileName::Mkdir(wxString(path).Append("/files/"), wxS_DIR_DEFAULT,
                                       wxPATH_MKDIR_FULL)) {
                    wxQueueEvent(this, new mwLogMessageEvent("Failed to download file"));
                    wxQueueEvent(this, new mwFileDownloadedEvent("Downloaded file " + file.name));
                    continue;
                }
                auto finalPath = wxString(path).Append("/files/").Append(file.name);
                auto outputFile = wxFile();
                if (!((wxFile::Exists(finalPath) || outputFile.Create(finalPath)) &&
                      outputFile.Open(finalPath, wxFile::write))) {
                    wxQueueEvent(this, new mwLogMessageEvent("Failed to download file"));
                    return;
                }
                if (!(outputFile.Write(response->body.c_str(), response->body.size()) &&
                      outputFile.Flush() && outputFile.Close())) {
                    wxQueueEvent(this, new mwLogMessageEvent("Failed to download file"));
                } else {
                    wxQueueEvent(this, new mwLogMessageEvent("Saved thing file " + file.name));
                }
            } catch (thingy::ThingiverseException &exception) {
                wxQueueEvent(this, new mwLogMessageEvent("Failed to download file"));
            }
            wxQueueEvent(this, new mwFileDownloadedEvent("Downloaded file " + file.name));
        }
        wxQueueEvent(this, new mwLogMessageEvent("Downloaded all thing files"));

        for (const auto &image: images) {
            try {
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
                auto response = cdnClient.Get(url.c_str());
                wxQueueEvent(this, new mwFileDownloadingEvent("Downloading image " + image.name));

                if (!wxFileName::Mkdir(wxString(path).Append("/images/"), wxS_DIR_DEFAULT,
                                       wxPATH_MKDIR_FULL)) {
                    wxQueueEvent(this, new mwLogMessageEvent("Failed to download image"));
                    wxQueueEvent(this, new mwFileDownloadedEvent("Downloaded image " + image.name));
                    continue;
                }
                if (response.error() != httplib::Error::Success) {
                    wxQueueEvent(this, new mwLogMessageEvent("Failed to download image"));
                    return;
                }
                auto finalPath = wxString(path).Append("/images/").Append(image.name);
                auto outputFile = wxFile();
                if (!((wxFile::Exists(finalPath) || outputFile.Create(finalPath)) &&
                      outputFile.Open(finalPath, wxFile::write))) {
                    wxQueueEvent(this, new mwLogMessageEvent("Failed to download image"));
                    return;
                }
                if (!(outputFile.Write(response->body.c_str(), response->body.size()) &&
                      outputFile.Flush() && outputFile.Close())) {
                    wxQueueEvent(this, new mwLogMessageEvent("Failed to download image"));
                } else {
                    wxQueueEvent(this, new mwLogMessageEvent("Saved thing image " + image.name));
                }
            } catch (thingy::ThingiverseException &exception) {
                wxQueueEvent(this, new mwLogMessageEvent("Failed to download image"));
            }
            wxQueueEvent(this, new mwFileDownloadedEvent("Downloaded image " + image.name));
        }
        wxQueueEvent(this, new mwLogMessageEvent("Downloaded all thing images"));
    } catch (std::exception &exception) {
        wxQueueEvent(this, new mwLogMessageEvent(exception.what()));
    }

    try {
        auto thing = thingy::ThingiverseClient(apiKey).getThing(thingId);
        auto outputFile = wxFile();
        auto descriptionPath = path + "/description.htm";
        if (wxFile::Exists(descriptionPath) || outputFile.Create(descriptionPath)) {
            if (outputFile.Open(descriptionPath, wxFile::write)) {
                auto data =
                        "<html>"
                        "<head>"
                        "<link rel=\"stylesheet\" href=\"https://unpkg.com/@picocss/pico@latest/css/pico.min.css\">"
                        "</head>"
                        "<body>"
                        "<main>"
                        "<section>"
                        + thing.detailsHtml + thing.instructionsHtml +
                        "</section>"
                        "</main>"
                        "</body>"
                        "</html>";
                outputFile.Write(data.c_str(), data.size());
                outputFile.Flush();
                outputFile.Close();
            }
        }
    } catch (std::exception &exception) {
        wxQueueEvent(this, new mwLogMessageEvent(exception.what()));
    }
}

void MainWindow::log(const wxString &message) {
    logOutput->Append(message);
    logOutput->SetSelection(logOutput->GetCount() - 1);
}

mwThingLoadedEvent::mwThingLoadedEvent(thingy::entities::Thing thing) : thing(std::move(thing)),
                                                                        wxCommandEvent(mwEVT_THING_LOADED) {}

mwLogMessageEvent::mwLogMessageEvent(const wxString &message) : message(message), wxThreadEvent(mwEVT_LOG_MESSAGE) {}

mwFileDownloadedEvent::mwFileDownloadedEvent(std::string message) : message(std::move(message)),
                                                                    wxThreadEvent(mwEVT_FILE_DOWNLOADED) {}

mwFileDownloadingEvent::mwFileDownloadingEvent(std::string message) : message(std::move(message)),
                                                                      wxThreadEvent(mwEVT_FILE_DOWNLOADING) {}

mwFilesCountedEvent::mwFilesCountedEvent(long count) : count(count), wxThreadEvent(mwEVT_FILES_COUNTED) {}

mwThingsCountedEvent::mwThingsCountedEvent(unsigned long long thingCount) : thingCount(thingCount),
                                                                            wxThreadEvent(mwEVT_THINGS_COUNTED) {}

mwThingDownloadedEvent::mwThingDownloadedEvent(std::string message) : wxThreadEvent(mwEVT_THING_DOWLOADED),
                                                                      message(std::move(message)) {}

mwThingDownloadingEvent::mwThingDownloadingEvent(std::string message) : wxThreadEvent(mwEVT_THING_DOWLOADING),
                                                                        message(std::move(message)) {}