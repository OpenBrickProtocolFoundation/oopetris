#pragma once

#include <filesystem>
#include <oatpp/core/Types.hpp>
#include <oatpp/core/data/mapping/type/Primitive.hpp>
#include <oatpp/web/protocol/http/Http.hpp>
#include <string>
#include <tl/optional.hpp>
#include <utility>
#include <vector>

namespace utils {

    using ReplaceFields = std::vector<std::pair<std::string, std::string>>;


    tl::optional<std::string> loadFileFromRoot(const std::string& inputPath);

    //from: https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
    void replaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace);

    std::string errorTemplate(oatpp::web::protocol::http::Status status);
    std::string errorTemplate(oatpp::web::protocol::http::Status status, ReplaceFields& additionalFields);


}; // namespace utils
