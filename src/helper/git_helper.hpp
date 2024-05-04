#include <string>


namespace utils {


    [[nodiscard]] inline std::string git_commit() {

#include "git_version.hpp"

        return GIT_VERSION;
    }

} // namespace utils
