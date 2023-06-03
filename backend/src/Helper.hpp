#pragma once

#include <filesystem>
#include <oatpp/core/Types.hpp>
#include <oatpp/core/data/mapping/type/Primitive.hpp>
#include <oatpp/web/protocol/http/Http.hpp>
#include <string>
#include <utility>
#include <vector>

namespace utils {

    using ReplaceFields = std::vector<std::pair<std::string, std::string>>;


    //TODO use tl::optonal
    oatpp::String loadFileFromRoot(const oatpp::String& inputPath);

    //from: https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
    void replaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace);

    oatpp::String errorTemplate(oatpp::web::protocol::http::Status status);
    oatpp::String errorTemplate(oatpp::web::protocol::http::Status status, ReplaceFields& additionalFields);


}; // namespace utils
