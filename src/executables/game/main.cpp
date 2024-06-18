

#include "./parser.hpp"

#include <core/helper/errors.hpp>
#include <core/helper/utils.hpp>

#include "application.hpp"
#include "helper/constants.hpp"
#include "helper/graphic_utils.hpp"
#include "helper/logger.hpp"
#include "helper/message_box.hpp"

#include <exception>
#include <filesystem>
#include <fmt/format.h>
#include <memory>
#include <vector>

#include "helper/console_helpers.hpp"


namespace {

    int main_no_sdl_replace(int argc, char** argv) noexcept {

        std::shared_ptr<Window> window{ nullptr };

        try {

            console::debug_print("TEST 2\n");
            SYS_Report("%s\n", "TEST 3");


            logger::initialize();

            console::debug_print("TEST 4\n");
            spdlog::error("SPDLOG TESTSTSTSTS");
            SYS_Report("%s\n", "TEST 5");


            std::vector<std::string> arguments_vector{};
            arguments_vector.reserve(argc);
            for (auto i = 0; i < argc; ++i) {
                arguments_vector.emplace_back(argv[i]); //NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            }

            if (arguments_vector.empty()) {
                arguments_vector.emplace_back("oopetris");
            }

            console::debug_print("TEST 6\n");

            auto parsed_arguments = helper::parse_args(arguments_vector);

            console::debug_print("TEST 7\n");

            if (not parsed_arguments.has_value()) {

                console::debug_print("erro 1\n");
                //   spdlog::error("error parsing command line arguments: {}", parsed_arguments.error());
                return EXIT_FAILURE;
            }

            console::debug_print("TEST 8\n");

            auto arguments = std::move(parsed_arguments.value());

            [[maybe_unused]] constexpr auto window_name = constants::program_name.c_str();


            console::debug_print("TEST 9\n");

            try {
#if defined(__ANDROID__) or defined(__CONSOLE__)
                console::debug_print("TEST 10\n");
                window = std::make_shared<Window>(window_name, WindowPosition::Centered);
                console::debug_print("TEST 11\n");
#else
                [[maybe_unused]] static constexpr int width = 1280;
                [[maybe_unused]] static constexpr int height = 720;
                window = std::make_shared<Window>(window_name, WindowPosition::Centered, width, height);
#endif
            } catch (const helper::GeneralError& general_error) {
                console::debug_print("error 5" + general_error.message() + "\n");
                //     spdlog::error("Couldn't initialize window: {}", general_error.message());
            }

            console::debug_print(fmt::format("TEST 12 {}\n", static_cast<void*>(window.get())));


            if (window == nullptr) {
                console::debug_print("window nullptr\n");

                helper::MessageBox::show_simple(
                        helper::MessageBox::Type::Error, "Initialization Error", "failed to create SDL window", nullptr
                );
                return EXIT_FAILURE;
            }

            console::debug_print(fmt::format("TEST 13\n"));

            Application app{ std::move(window), std::move(arguments) };

            console::debug_print("before app run\n");

            app.run();
            return EXIT_SUCCESS;

        } catch (const helper::GeneralError& general_error) {
            console::debug_print("error 44:" + general_error.message() + "\n");
            // spdlog::error("{}", general_error.message());


            if (window == nullptr) {
                helper::MessageBox::show_simple(
                        helper::MessageBox::Type::Error, "Initialization Error", general_error.message(), nullptr
                );
            } else {
                window->show_simple(helper::MessageBox::Type::Error, "Initialization Error", general_error.message());
            }


            return EXIT_FAILURE;
        } catch (const utils::ExitException& exit_exception) {
            console::debug_print("error 45s\n");
            spdlog::debug("Requested exit with status code {}", exit_exception.status_code());
            return exit_exception.status_code();
        } catch (const std::system_error& error) {

            console::debug_print(fmt::format(
                    "Caught system_error with code [{}] meaning [{}]\n", (std::stringstream{} << error.code()).str(),
                    error.what()
            ));

            std::cerr << "Caught system_error with code [" << error.code() << "] meaning [" << error.what() << "]\n";

            return EXIT_FAILURE;

        } catch (const std::exception& error) {
            spdlog::error("error: {}", error.what());
            // this is the last resort, so using std::cerr and no sdl show_simple messagebox!
            std::cerr << error.what();
            return EXIT_FAILURE;
        }
    }


} // namespace


#include <gccore.h>
#include <wiiuse/wpad.h>

#include <fat.h>

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static void* xfb = NULL;
static GXRModeObj* rmode = NULL;

//---------------------------------------------------------------------------------
int main2() {
    //---------------------------------------------------------------------------------

    // Initialise the video system
    VIDEO_Init();

    // This function initialises the attached controllers
    WPAD_Init();

    // Obtain the preferred video mode from the system
    // This will correspond to the settings in the Wii menu
    rmode = VIDEO_GetPreferredMode(NULL);

    // Allocate memory for the display in the uncached region
    xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

    // Initialise the console, required for SYS_Report
    console_init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);

    // Set up the video registers with the chosen mode
    VIDEO_Configure(rmode);

    // Tell the video hardware where our display memory is
    VIDEO_SetNextFramebuffer(xfb);

    // Make the display visible
    VIDEO_SetBlack(true);

    // Flush the video register changes to the hardware
    VIDEO_Flush();

    // Wait for Video setup to complete
    VIDEO_WaitVSync();
    if (rmode->viTVMode & VI_NON_INTERLACE)
        VIDEO_WaitVSync();


    SYS_Report("TESTTESTETS\n");

    // The console understands VT terminal escape codes
    // This positions the cursor on row 2, column 0
    // we can use variables for this with format codes too
    // e.g. SYS_Report ("\x1b[%d;%dH", row, column );
    console::debug_print("\x1b[2;0H");

    if (!fatInitDefault()) {
        SYS_Report("fatInitDefault failure: terminating\n");
        goto error;
    }

    DIR* pdir;
    struct dirent* pent;
    struct stat statbuf;

    pdir = opendir(".");

    if (!pdir) {
        SYS_Report("opendir() failure; terminating\n");
        goto error;
    }

    while ((pent = readdir(pdir)) != NULL) {
        stat(pent->d_name, &statbuf);
        if (strcmp(".", pent->d_name) == 0 || strcmp("..", pent->d_name) == 0)
            continue;
        if (S_ISDIR(statbuf.st_mode))
            SYS_Report("%s <dir>\n", pent->d_name);
        if (!(S_ISDIR(statbuf.st_mode)))
            SYS_Report("%s %lld\n", pent->d_name, statbuf.st_size);
    }
    closedir(pdir);

error:
    while (1) {

        // Call WPAD_ScanPads each loop, this reads the latest controller states
        WPAD_ScanPads();

        // WPAD_ButtonsDown tells us which buttons were pressed in this loop
        // this is a "one shot" state which will not fire again until the button has been released
        u32 pressed = WPAD_ButtonsDown(0);

        // We return to the launcher application via exit
        if (pressed & WPAD_BUTTON_HOME)
            exit(0);

        // Wait for the next frame
        VIDEO_WaitVSync();
    }

    return 0;
}


#include <debug.h>

int main(int argc, char** argv) {


    //after your network is initialized using if_config() function, you can call
    // Init debug over BBA...change IPs to suite your needs
    tcp_localip = "10.0.0.106";
    tcp_netmask = "255.255.255.0";
    tcp_gateway = "192.168.1.1";


    DEBUG_Init(GDBSTUB_DEVICE_TCP, GDBSTUB_DEF_TCPPORT);

    _break();


    //  main2();
    int z = main_no_sdl_replace(argc, argv);
    console::debug_print("return value: " + std::to_string(z) + "\n");
    return z;
}


// target remote 127.0.0.1:2828
