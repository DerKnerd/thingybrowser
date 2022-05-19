//
// Created by imanuel on 19.05.22.
//

#include "ThingLoader.h"
#include "wx/filename.h"
#include "../things/ThingsWindow.h"

void
ThingLoader::downloadThingFilesAndImages(wxWindow *sink, const thingy::entities::Thing &thing, const std::string &path,
                                         const std::string &apiKey) {
    if (!wxFileName::Mkdir(path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL)) {
        return;
    }
    auto files = thingy::ThingiverseClient(apiKey).getFilesByThing(thing.id);
    auto images = thingy::ThingiverseClient(apiKey).getImagesByThing(thing.id);
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
    wxQueueEvent(sink, new twLogMessageEvent("Downloaded all thing files"));

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
    wxQueueEvent(sink, new twLogMessageEvent("Downloaded all thing images"));

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
}
