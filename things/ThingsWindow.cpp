//
// Created by imanuel on 23.04.22.
//

#include "ThingsWindow.h"

#include <utility>
#include "../MainApp.h"
#include "../helper.h"
#include "../helpers/font.h"
#include "wx/gbsizer.h"
#include "wx/utils.h"
#include "../helpers/tbNoLinkHtmlWindow.h"
#include "wx/sstream.h"
#include "wx/mstream.h"

ThingsWindow::ThingsWindow(wxWindow *parent, unsigned long long thingId) : wxFrame(
        parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(1280, 800),
        wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN) {
    toolbar = CreateToolBar(wxTB_HORIZONTAL | wxTB_HORZ_LAYOUT | wxTB_TEXT | wxTB_NOICONS);
    toolbar->AddTool(ThingsWindowActions::ThingsWindowDownloadThing, _("Download thing files"), wxNullBitmap);
    toolbar->Realize();

    toolbar->EnableTool(ThingsWindowActions::ThingsWindowDownloadThing, false);

    CreateStatusBar();

    panel = new wxPanel(this, wxID_ANY);
    auto contentSizer = new wxGridBagSizer(5, 5);

    title = new wxStaticText(panel, wxID_ANY, "", wxDefaultPosition, wxSize(-1, -1));
    title->SetFont(getTitleFont());
    contentSizer->Add(title, wxGBPosition(0, 0), wxGBSpan(1, 2),
                      wxSizerFlags().Proportion(1).Border(wxALL, WXC_FROM_DIP(5)).GetFlags());

    details = new twNoLinkHtmlWindow(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxHW_SCROLLBAR_AUTO);
    contentSizer->Add(details, wxGBPosition(1, 1), wxGBSpan(1, 1),
                      wxSizerFlags().Proportion(1).Border(wxALL, WXC_FROM_DIP(5)).Expand().GetFlags());

    contentSizer->AddGrowableCol(1);
    contentSizer->AddGrowableRow(1);

    imageSizer = new wxGridBagSizer(5, 5);
    scrolledWindow = new wxScrolledWindow(panel, wxID_ANY, wxDefaultPosition, wxSize(500, -1), wxVSCROLL);
    scrolledWindow->SetSizer(imageSizer);
    scrolledWindow->SetScrollRate(5, 5);
    contentSizer->Add(scrolledWindow, wxGBPosition(1, 0), wxGBSpan(1, 1),
                      wxSizerFlags().Proportion(1).Expand().Border(wxALL, WXC_FROM_DIP(5)).GetFlags());

    SetName("ThingsWindow");
    SetSize(wxDLG_UNIT(this, wxSize(-1, -1)));
    panel->SetSizer(contentSizer);

    if (GetSizer()) {
        GetSizer()->Fit(panel);
    }
    if (GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }
    auto apiKey = MainApp::getInstance()->GetSettings().thingyverseApiKey;
    toolbar->Bind(wxEVT_MENU, [this, apiKey](wxCommandEvent &) {
        auto fileOpenDialog = new wxDirDialog(this, _("Save thing"), wxEmptyString, wxDD_DEFAULT_STYLE);
        fileOpenDialog->Bind(wxEVT_WINDOW_MODAL_DIALOG_CLOSED, [this, apiKey](const wxWindowModalDialogEvent &event) {
            if (event.GetReturnCode() == wxID_OK) {
                auto dialog = dynamic_cast<wxDirDialog *>(event.GetDialog());
                downloadProgress = new wxProgressDialog(_("Download progress"), _("Downloading files"), 100, this,
                                                        wxPD_AUTO_HIDE);
                std::thread([](wxWindow *sink, const unsigned long long id, const std::string &path,
                               const std::string &apiKey, const thingy::entities::Thing &thing) {
                    if (!wxFileName::Mkdir(path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL)) {
                        wxQueueEvent(sink, new twLogMessageEvent("Failed to download file"));
                        return;
                    }
                    auto files = thingy::ThingiverseClient(apiKey).getFilesByThing(id);
                    auto images = thingy::ThingiverseClient(apiKey).getImagesByThing(id);
                    auto filesCount = std::count_if(files.begin(), files.end(), [](const thingy::entities::File &file) {
                        return !file.downloadUrl.empty();
                    });
                    auto imagesCount = images.size();
                    wxQueueEvent(sink, new twFilesCountedEvent(filesCount + imagesCount));

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
                            wxQueueEvent(sink, new twFileDownloadingEvent("Downloading file " + file.name));
                            auto response = apiClient.Get(file.downloadUrl.c_str());
                            if (response.error() != httplib::Error::Success) {
                                wxQueueEvent(sink, new twLogMessageEvent("Failed to download file"));
                                wxQueueEvent(sink, new twFileDownloadedEvent("Downloaded file " + file.name));
                                continue;
                            }

                            if (!wxFileName::Mkdir(wxString(path).Append("/files/"), wxS_DIR_DEFAULT,
                                                   wxPATH_MKDIR_FULL)) {
                                wxQueueEvent(sink, new twLogMessageEvent("Failed to download file"));
                                wxQueueEvent(sink, new twFileDownloadedEvent("Downloaded file " + file.name));
                                continue;
                            }
                            auto finalPath = wxString(path).Append("/files/").Append(file.name);
                            auto outputFile = wxFile();
                            if (!((wxFile::Exists(finalPath) || outputFile.Create(finalPath)) &&
                                  outputFile.Open(finalPath, wxFile::write))) {
                                wxQueueEvent(sink, new twLogMessageEvent("Failed to download file"));
                                return;
                            }
                            if (!(outputFile.Write(response->body.c_str(), response->body.size()) &&
                                  outputFile.Flush() && outputFile.Close())) {
                                wxQueueEvent(sink, new twLogMessageEvent("Failed to download file"));
                            } else {
                                wxQueueEvent(sink, new twLogMessageEvent("Saved thing file " + file.name));
                            }
                        } catch (thingy::ThingiverseException &exception) {
                            wxQueueEvent(sink, new twLogMessageEvent("Failed to download file"));
                        }
                        wxQueueEvent(sink, new twFileDownloadedEvent("Downloaded file " + file.name));
                    }
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
                            wxQueueEvent(sink, new twFileDownloadingEvent("Downloading image " + image.name));

                            if (!wxFileName::Mkdir(wxString(path).Append("/images/"), wxS_DIR_DEFAULT,
                                                   wxPATH_MKDIR_FULL)) {
                                wxQueueEvent(sink, new twLogMessageEvent("Failed to download image"));
                                wxQueueEvent(sink, new twFileDownloadedEvent("Downloaded image " + image.name));
                                continue;
                            }
                            if (response.error() != httplib::Error::Success) {
                                wxQueueEvent(sink, new twLogMessageEvent("Failed to download image"));
                                return;
                            }
                            auto finalPath = wxString(path).Append("/images/").Append(image.name);
                            auto outputFile = wxFile();
                            if (!((wxFile::Exists(finalPath) || outputFile.Create(finalPath)) &&
                                  outputFile.Open(finalPath, wxFile::write))) {
                                wxQueueEvent(sink, new twLogMessageEvent("Failed to download image"));
                                return;
                            }
                            if (!(outputFile.Write(response->body.c_str(), response->body.size()) &&
                                  outputFile.Flush() && outputFile.Close())) {
                                wxQueueEvent(sink, new twLogMessageEvent("Failed to download image"));
                            } else {
                                wxQueueEvent(sink, new twLogMessageEvent("Saved thing image " + image.name));
                            }
                        } catch (thingy::ThingiverseException &exception) {
                            wxQueueEvent(sink, new twLogMessageEvent("Failed to download image"));
                        }
                        wxQueueEvent(sink, new twFileDownloadedEvent("Downloaded image " + image.name));
                    }

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
                                    + thing.detailsHtml + thing.instructionsHtml +
                                    "</main>"
                                    "</body>"
                                    "</html>";
                            outputFile.Write(data.c_str(), data.size());
                            outputFile.Flush();
                            outputFile.Close();
                        }
                    }
                    wxQueueEvent(sink, new twLogMessageEvent("Downloaded all thing images"));
                }, this, thing.id, dialog->GetPath(), apiKey, thing).detach();
            }
        });

        fileOpenDialog->ShowWindowModal();
    }, ThingsWindowActions::ThingsWindowDownloadThing);

    Bind(twEVT_THING_LOADED, [this](twThingLoadedEvent &event) {
        this->thing = event.thing;
        this->displayThing();
    });
    Bind(twEVT_IMAGE_LOADED, [this](twImageLoadedEvent &event) {
        auto img = new wxStaticBitmap(scrolledWindow, wxID_ANY, wxNullBitmap);
        img->SetBitmap(event.image);
        this->imageSizer->Add(img, wxGBPosition(imageSizer->GetEffectiveRowsCount() + 1, 0), wxDefaultSpan,
                              wxSizerFlags().Proportion(1).Expand().Border(wxALL, WXC_FROM_DIP(5)).GetFlags());
    });
    Bind(twEVT_ALL_IMAGES_LOADED, [this](twAllImagesLoadedEvent &event) {
        this->SetVirtualSize(wxSize(this->GetClientSize().x, this->GetClientSize().y + event.height));
        scrolledWindow->SetVirtualSize(
                wxSize(this->GetClientSize().x, scrolledWindow->GetClientSize().y + event.height));
    });
    Bind(twEVT_LOG_MESSAGE, [this](twLogMessageEvent &event) {
        this->GetStatusBar()->SetStatusText(event.message);
    });
    Bind(twEVT_FILE_DOWNLOADED, [this](twFileDownloadedEvent &event) {
        if (downloadProgress != nullptr && downloadProgress->GetValue() < downloadProgress->GetRange()) {
            downloadProgress->Update(downloadProgress->GetValue() + 1, event.message);
        }
    });
    Bind(twEVT_FILE_DOWNLOADING, [this](twFileDownloadingEvent &event) {
        if (downloadProgress != nullptr) {
            downloadProgress->Update(downloadProgress->GetValue(), event.message);
        }
    });
    Bind(twEVT_FILES_COUNTED, [this](twFilesCountedEvent &event) {
        if (downloadProgress != nullptr) {
            downloadProgress->Update(0, "");
            downloadProgress->SetRange(static_cast<int>(event.count));
        }
    });

    std::thread([](wxWindow *sink, unsigned long long thingId, const std::string &apiKey) {
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
    }, this, thingId, apiKey).detach();
}

void ThingsWindow::displayThing() {
    toolbar->EnableTool(ThingsWindowActions::ThingsWindowDownloadThing, true);
    details->SetPage(
            "<html><body>" + thing.detailsHtml + thing.instructionsHtml + "</body></html>");
    title->SetLabel("#" + std::to_string(thing.id) + " - " + thing.name + _(" by ") + thing.creator.username);
    this->SetTitle("#" + std::to_string(thing.id) + " - " + thing.name + _(" by ") + thing.creator.username);
    toolbar->AddTool(ThingsWindowActions::ThingsWindowGoToDesigner, _("More from ") + thing.creator.username,
                     wxNullBitmap);
}

twThingLoadedEvent::twThingLoadedEvent(thingy::entities::Thing thing) : thing(std::move(thing)),
                                                                        wxThreadEvent(twEVT_THING_LOADED) {}

twLogMessageEvent::twLogMessageEvent(const wxString &message) : message(message), wxThreadEvent(twEVT_LOG_MESSAGE) {}

twFileDownloadedEvent::twFileDownloadedEvent(std::string message) : message(std::move(message)),
                                                                    wxThreadEvent(twEVT_FILE_DOWNLOADED) {}

twFileDownloadingEvent::twFileDownloadingEvent(std::string message) : message(std::move(message)),
                                                                      wxThreadEvent(twEVT_FILE_DOWNLOADING) {}

twImageLoadedEvent::twImageLoadedEvent(const wxBitmap &image) : image(image), wxThreadEvent(twEVT_IMAGE_LOADED) {}

twAllImagesLoadedEvent::twAllImagesLoadedEvent(int height) : height(height), wxThreadEvent(twEVT_ALL_IMAGES_LOADED) {}

twFilesCountedEvent::twFilesCountedEvent(long count) : count(count), wxThreadEvent(twEVT_FILES_COUNTED) {}