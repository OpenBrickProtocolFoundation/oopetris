#include <string>


namespace utils {


    [[nodiscard]] inline std::string git_commit() {

#if defined(_HAS_GIT_COMMIT_INFORMATION)
#include "git_version.hpp"
        return GIT_VERSION;
#else
        return "Unknown";
#endif
    }

} // namespace utils
