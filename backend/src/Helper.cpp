#include "Helper.hpp"


tl::optional<std::string> utils::loadFileFromRoot(const std::string& inputPath) {
    std::string normalizedInputPath = inputPath;
    if (normalizedInputPath.at(0) == '/') {
        normalizedInputPath.erase(0, 1);
    }
    const auto filePath = std::filesystem::path(ROOT_PATH) / normalizedInputPath;
    if (!std::filesystem::exists(filePath)) {
        return tl::nullopt;
    }
    const auto temp = oatpp::String::loadFromFile(filePath.c_str()).getValue("");
    if (temp.empty()) {
        return tl::nullopt;
    }

    return temp;
};

//from: https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
void utils::replaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
}

std::string utils::errorTemplate(oatpp::web::protocol::http::Status status, utils::ReplaceFields& additionalFields) {
    const auto errorPagePath = std::string("/error/" + std::to_string(status.code) + ".html");
    auto loadedTemplate = loadFileFromRoot(errorPagePath);
    auto rawTemplate =
            loadedTemplate.has_value() ? loadedTemplate.value() : "FATAL ERROR in loading error file" + errorPagePath;

    additionalFields.emplace_back("HOSTNAME", HOSTNAME);
    for (const auto& [key, value] : additionalFields) {
        const auto replaceKey = "%{" + key + "}";
        utils::replaceStringInPlace(rawTemplate, replaceKey, value);
    }

    return rawTemplate;
}


std::string utils::errorTemplate(oatpp::web::protocol::http::Status status) {
    auto temp = utils::ReplaceFields{};
    return utils::errorTemplate(status, temp);
}
