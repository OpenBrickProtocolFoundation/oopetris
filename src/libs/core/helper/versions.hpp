#include <string>


namespace utils {


#define STRINGIFY(a) STRINGIFY_HELPER_(a) //NOLINT(cppcoreguidelines-macro-usage)
#define STRINGIFY_HELPER_(a) #a           //NOLINT(cppcoreguidelines-macro-usage)


#if  !defined(OOPETRIS_VERSION)
#error "OOPETRIS_VERSION not defined"
#endif



    [[nodiscard]] inline std::string version() {


        return STRINGIFY(OOPETRIS_VERSION);
    }


#undef STRINGIFY
#undef STRINGIFY_HELPER_


} // namespace utils
