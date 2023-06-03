#include "Helper.hpp"


//TODO use tl::optonal
oatpp::String utils::loadFileFromRoot(const oatpp::String& inputPath) {
    std::string normalizedInputPath = inputPath.getValue("index.html");
    if (normalizedInputPath.at(0) == '/') {
        normalizedInputPath.erase(0, 1);
    }
    const auto filePath = std::filesystem::path(ROOT_PATH) / normalizedInputPath;
    if (!std::filesystem::exists(filePath)) {
        return oatpp::String{ "" };
    }
    return oatpp::String::loadFromFile(filePath.c_str());
};

//from: https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
void utils::replaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
}

oatpp::String utils::errorTemplate(oatpp::web::protocol::http::Status status, utils::ReplaceFields& additionalFields) {
    const std::string errorPagePath = oatpp::String("/error/" + std::to_string(status.code) + ".html");
    std::string rawTemplate = loadFileFromRoot(errorPagePath);

    additionalFields.emplace_back("HOSTNAME", HOSTNAME);
    for (const auto& [key, value] : additionalFields) {
        const auto replaceKey = "%{" + key + "}";
        utils::replaceStringInPlace(rawTemplate, replaceKey, value);
    }

    return rawTemplate;
}


oatpp::String utils::errorTemplate(oatpp::web::protocol::http::Status status) { }
