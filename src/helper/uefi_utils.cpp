

#if !defined(__UEFI__)
#error "Only supported on UEFI"
#endif

#include <core/helper/utils.hpp>

#include "./uefi_utils.hpp"

#include <filesystem>
#include <functional>
#include <memory>
#include <string>

#include <sys/threads.h>
#include <sys/time.h>

extern "C" {
#include <Guid/FileInfo.h>
#include <Library/BaseLib.h>
#include <Library/SynchronizationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include <Uefi.h>

#include <sys/dirent.h>
}

struct OnScopeEnd {
private:
    std::function<void()> m_destructor;

public:
    OnScopeEnd(std::function<void()> destructor) : m_destructor{ destructor } {
        //
    }
    OnScopeEnd(const OnScopeEnd& other) = delete;
    OnScopeEnd& operator=(const OnScopeEnd& other) = delete;


    OnScopeEnd(OnScopeEnd&& other) noexcept = delete;

    OnScopeEnd& operator=(OnScopeEnd&& other) noexcept = delete;

    ~OnScopeEnd() {
        this->m_destructor();
    }
};


std::shared_ptr<spdlog::sinks::callback_sink_mt> uefi::get_debug_sink() {
    efi_threads_init();

    return std::make_shared<spdlog::sinks::callback_sink_mt>([](const spdlog::details::log_msg& msg) {
        const std::string message = std::string{ msg.payload.begin(), msg.payload.end() };

        UINTN thread_id = efi_thread_id();

        switch (msg.level) {
            case spdlog::level::off:
                return;
            case spdlog::level::trace:
            case spdlog::level::debug:
                EFI_DEBUG((DEBUG_VERBOSE, "[thread %lu] %a\n", thread_id, message.c_str()));
                break;
            case spdlog::level::info:
                EFI_DEBUG((DEBUG_INFO, "[thread %lu] %a\n", thread_id, message.c_str()));
                break;
            case spdlog::level::warn:
                EFI_DEBUG((DEBUG_WARN, "[thread %lu] %a\n", thread_id, message.c_str()));
                break;
            case spdlog::level::err:
            case spdlog::level::critical:
                EFI_DEBUG((DEBUG_ERROR, "[thread %lu] %a\n", thread_id, message.c_str()));
                break;
            default:
                return;
        }
    });
}


[[nodiscard]] std::string uefi::map_efi_status_to_string(EFI_STATUS status) {

    switch (status) {
        case EFI_SUCCESS:
            return "SUCCESS";
        case EFI_LOAD_ERROR:
            return "LOAD_ERROR";
        case EFI_INVALID_PARAMETER:
            return "INVALID_PARAMETER";
        case EFI_UNSUPPORTED:
            return "UNSUPPORTED";
        case EFI_BAD_BUFFER_SIZE:
            return "BAD_BUFFER_SIZE";
        case EFI_BUFFER_TOO_SMALL:
            return "BUFFER_TOO_SMALL";
        case EFI_NOT_READY:
            return "NOT_READY";
        case EFI_DEVICE_ERROR:
            return "DEVICE_ERROR";
        case EFI_WRITE_PROTECTED:
            return "WRITE_PROTECTED";
        case EFI_OUT_OF_RESOURCES:
            return "OUT_OF_RESOURCES";
        case EFI_VOLUME_CORRUPTED:
            return "VOLUME_CORRUPTED";
        case EFI_VOLUME_FULL:
            return "VOLUME_FULL";
        case EFI_NO_MEDIA:
            return "NO_MEDIA";
        case EFI_MEDIA_CHANGED:
            return "MEDIA_CHANGED";
        case EFI_NOT_FOUND:
            return "NOT_FOUND";
        case EFI_ACCESS_DENIED:
            return "ACCESS_DENIED";
        case EFI_NO_RESPONSE:
            return "NO_RESPONSE";
        case EFI_NO_MAPPING:
            return "NO_MAPPING";
        case EFI_TIMEOUT:
            return "TIMEOUT";
        case EFI_NOT_STARTED:
            return "NOT_STARTED";
        case EFI_ALREADY_STARTED:
            return "ALREADY_STARTED";
        case EFI_ABORTED:
            return "ABORTED";
        case EFI_ICMP_ERROR:
            return "ICMP_ERROR";
        case EFI_TFTP_ERROR:
            return "TFTP_ERROR";
        case EFI_PROTOCOL_ERROR:
            return "PROTOCOL_ERROR";
        case EFI_INCOMPATIBLE_VERSION:
            return "INCOMPATIBLE_VERSION";
        case EFI_SECURITY_VIOLATION:
            return "SECURITY_VIOLATION";
        case EFI_CRC_ERROR:
            return "CRC_ERROR";
        case EFI_END_OF_MEDIA:
            return "END_OF_MEDIA";
        case EFI_END_OF_FILE:
            return "END_OF_FILE";
        case EFI_INVALID_LANGUAGE:
            return "INVALID_LANGUAGE";
        case EFI_COMPROMISED_DATA:
            return "COMPROMISED_DATA";
        case EFI_IP_ADDRESS_CONFLICT:
            return "IP_ADDRESS_CONFLICT";
        case EFI_HTTP_ERROR:
            return "HTTP_ERROR";
        case EFI_WARN_UNKNOWN_GLYPH:
            return "WARN_UNKNOWN_GLYPH";
        case EFI_WARN_DELETE_FAILURE:
            return "WARN_DELETE_FAILURE";
        case EFI_WARN_WRITE_FAILURE:
            return "WARN_WRITE_FAILURE";
        case EFI_WARN_BUFFER_TOO_SMALL:
            return "WARN_BUFFER_TOO_SMALL";
        case EFI_WARN_STALE_DATA:
            return "WARN_STALE_DATA";
        case EFI_WARN_FILE_SYSTEM:
            return "WARN_FILE_SYSTEM";
        case EFI_WARN_RESET_REQUIRED:
            return "WARN_RESET_REQUIRED";
        default:
            return "<Unknown State>";
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"

#include <c-embed.h>

#pragma GCC diagnostic pop

extern "C" {
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Uefi.h>

// use stdlib internals, to register a new rom device under romfs:
#include <StdLibPrivateInternalFiles/Include/kfile.h>

#include <StdLibPrivateInternalFiles/Include/Device/Device.h>
}

// ([...('RwFs').split("").map(a=>a.charCodeAt(0).toString(16)), "0x"].reverse().join(""))
#define ROM_COOKIE 0x466d6f52 ///< 'RomF'
#define RW_COOKIE 0x73467752  ///< 'RwFs'


#ifdef __cplusplus
#define ASSERT_TYPE(expr, type) static_assert(std::is_same_v<decltype(expr), std::remove_cvref_t<type>>, "wrong type")
#else
#define ASSERT_TYPE(expr, type) static_assert(_Generic((expr), type: 1, default: 0), "wrong type")
#endif

#define VALIDATE_INSTANCE(type) \
    static_assert((sizeof(type) % 8) == 0, "instance structures MUST be a multiple of 8-bytes in length");

#define ROMFS_DEV_DATA_ASSIGN(data) ((void*) data)
#define ROMFS_DEV_DATA_TYPE EFILE
#define ROMFS_DEV_DATA_GET(data) ((ROMFS_DEV_DATA_TYPE*) data)


#define ROM_INSTANCE GenericInstance

VALIDATE_INSTANCE(ROM_INSTANCE)

/** EFI specific operations for close().

    @param[in]    filp    Pointer to a file descriptor structure.

    @retval      0      Successful completion.
    @retval     -1      Operation failed.  Further information is specified by errno.
**/
static int EFIAPI _f_romfs_Close(IN struct __filedes* filp) {
    ASSERT_TYPE(filp->devdata, void*);
    ROMFS_DEV_DATA_TYPE* file = ROMFS_DEV_DATA_GET(filp->devdata);

    eclose(file);
    return 0;
}

/** EFI specific operations for deleting a file or directory.

    @param[in]    filp    Pointer to a file descriptor structure.

    @retval      0      Successful completion.
    @retval     -1      Operation failed.  Further information is specified by errno.
**/
static int EFIAPI _f_romfs_Delete(struct __filedes* filp) {
    errno = ENOTSUP;
    return -1;
}

/** EFI specific operations for setting the position within a file.

    @param[in]    filp    Pointer to a file descriptor structure.
    @param[in]    offset  Relative position to move to.
    @param[in]    whence  Specifies the location offset is relative to: Beginning, Current, End.

    @return     Returns the new file position or EOF if the seek failed.
**/
static off_t EFIAPI _f_romfs_Seek(struct __filedes* filp, off_t offset, int whence) {
    ASSERT_TYPE(filp->devdata, void*);
    ROMFS_DEV_DATA_TYPE* file = ROMFS_DEV_DATA_GET(filp->devdata);

    //NOTE: eseek works differently than the expected seek, so map the behavior

    int result = eseek(file, offset, whence);
    if (result < 0) {
        errno = eerrno_to_errno(eerrno);
        return EOF;
    }

    long int pos = etell(file);
    if (pos < 0) {
        errno = EINVAL;
        return EOF;
    }

    return (off_t) pos;
}

/** The directory path is created with the access permissions specified by
    perms.

    The directory is closed after it is created.

    @param[in]    path      The directory to be created.
    @param[in]    perms     Access permissions for the new directory.

    @retval   0   The directory was created successfully.
    @retval  -1   An error occurred and an error code is stored in errno.
**/
static int EFIAPI _f_romfs_Mkdir(const char* path, __mode_t perms) {
    errno = ENOTSUP;
    return -1;
}

#define EFI_TIME_NULL              \
    ((EFI_TIME) { .Year = 0,       \
                  .Month = 0,      \
                  .Day = 0,        \
                  .Hour = 0,       \
                  .Minute = 0,     \
                  .Second = 0,     \
                  .Pad1 = 0,       \
                  .Nanosecond = 0, \
                  .TimeZone = 0,   \
                  .Daylight = 0,   \
                  .Pad2 = 0 })


//Normal to Wide
struct NameConversionNtoW {
private:
    const char* m_original;
    wchar_t* m_converted;

public:
    NameConversionNtoW(const char* value, wchar_t* converted) : m_original{ value }, m_converted{ converted } {
        //
    }

    static std::optional<NameConversionNtoW> init(const char* value) {

        wchar_t* converted = (wchar_t*) AllocateZeroPool(PATH_MAX + 1);
        if (converted == NULL) {
            errno = ENOMEM;
            return std::nullopt;
        }

        size_t convert_result = mbstowcs(converted, value, PATH_MAX);

        if (convert_result == ((size_t) -1) || convert_result > PATH_MAX) {
            errno = EINVAL;
            return std::nullopt;
        }

        converted[convert_result] = '\0';


        return NameConversionNtoW(value, converted);
    }

    NameConversionNtoW(const NameConversionNtoW& other) = delete;
    NameConversionNtoW& operator=(const NameConversionNtoW& other) = delete;


    NameConversionNtoW(NameConversionNtoW&& other) noexcept
        : m_original{ other.m_original },
          m_converted{ other.m_converted } {
        other.m_original = nullptr;
        other.m_converted = nullptr;
    }

    NameConversionNtoW& operator=(NameConversionNtoW&& other) noexcept {
        if (this != &other) {
            this->m_original = other.m_original;
            this->m_converted = other.m_converted;

            other.m_original = nullptr;
            other.m_converted = nullptr;
        }

        return *this;
    }


    [[nodiscard]] wchar_t* name() const {
        return m_converted;
    }

    ~NameConversionNtoW() {
        if (m_converted != nullptr) {
            FreePool(m_converted);
            m_converted = nullptr;
        }
    }
};


/** EFI specific operations for reading from a file.

    @param[in]    filp        Pointer to a file descriptor structure.
    @param[in]    offset      Offset into the file to begin reading at, or NULL.
    @param[in]    BufferSize  Number of bytes in Buffer.  Max number of bytes to read.
    @param[in]    Buffer      Pointer to a buffer to receive the read data.

    @return     Returns the number of bytes successfully read,
                or -1 if the operation failed.  Further information is specified by errno.
**/
static ssize_t EFIAPI
_f_romfs_Read(IN OUT struct __filedes* filp, IN OUT off_t* offset, IN size_t BufferSize, OUT VOID* Buffer) {
    ASSERT_TYPE(filp->devdata, void*);
    ROMFS_DEV_DATA_TYPE* file = ROMFS_DEV_DATA_GET(filp->devdata);

    DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));

    if (offset != NULL) {
        DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));

        off_t seek_size = _f_romfs_Seek(filp, *offset, SEEK_SET);
        if (seek_size == EOF) {
            DEBUG((DEBUG_ERROR, "%a %a:%d HERE %d\n", __func__, __FILE__, __LINE__, etell(file)));

            errno = eerrno_to_errno(eerrno);
            return -1;
        }
        DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));

        filp->f_offset = seek_size;

        *offset = seek_size;
    }


    int type = estreamtype(file);
    if (type == EMAP_ENTRY_TYPE_FILE) {
        DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));


        const size_t result = eread(Buffer, 1, BufferSize, file);

        if (eerrno != EERRCODE_SUCCESS) {
            errno = eerrno_to_errno(eerrno);
            return -1;
        }

        if (offset != nullptr) {
            *offset = *offset + static_cast<off_t>(result);
        }

        return result;
    } else if (type == EMAP_ENTRY_TYPE_DIR) {
        DEBUG((DEBUG_ERROR, "%a %a:%d HERE ISDIR\n", __func__, __FILE__, __LINE__));

        const size_t dir_offset = etell(file);
        DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));

        edirent ent;
        int res = ereaddir(file, &ent);
        DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));

        if (res == EREADDIR_FINISHED) {
            DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));
            return 0;
        }
        DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));

        if (res != EERRCODE_SUCCESS) {
            errno = eerrno_to_errno(res);
            return -1;
        }
        DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));

        UINT64 Attribute = DT_READ_ONLY;

        if (ent.type == EMAP_ENTRY_TYPE_DIR) {
            Attribute |= DT_DIRECTORY;
        }
        DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));

        const UINT64 dirent_struct_size = offsetof(struct dirent, FileName);
        DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));
        DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));

        // convert the ent name to CHAR16
        std::optional<NameConversionNtoW> conversion = NameConversionNtoW::init(ent.name);
        DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));

        if (not conversion.has_value()) {
            DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));

            // we can't convert the name to wchar
            int seek_res = eseek(file, dir_offset, SEEK_SET);
            if (seek_res < 0) {
                errno = eerrno_to_errno(eerrno);
                return -1;
            }

            errno = EINVAL;
            return -1;
        }
        DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));

        static_assert(sizeof(CHAR16) == sizeof(wchar_t), "libc types and std types don't match");
        const UINT64 name_len = wcslen(conversion->name());
        const UINT64 size = dirent_struct_size + ((1 + name_len) * sizeof(CHAR16));

        DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));
        DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));


        if (size > BufferSize) {
            DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));

            // we can't place this in the Buffer anymore
            int seek_res = eseek(file, dir_offset, SEEK_SET);
            if (seek_res < 0) {
                errno = eerrno_to_errno(eerrno);
                return -1;
            }

            // we have not enough space for the buffer
            errno = ENOMEM;
            return -1;
        }
        DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));

        struct dirent dir_result = {
            .Size = size,
            .FileSize = ent.size,
            .PhysicalSize = ent.size,
            .CreateTime = EFI_TIME_NULL,
            .LastAccessTime = EFI_TIME_NULL,
            .ModificationTime = EFI_TIME_NULL,
            .Attribute = Attribute,
            .FileName = { L'\0' },
        };

        // copy the struct and the filename into the buffer
        char* const location = (char*) Buffer;

        memcpy(location, &dir_result, dirent_struct_size);
        memcpy(location + dirent_struct_size, conversion->name(), name_len * sizeof(CHAR16));
        *(((CHAR16*) (location + dirent_struct_size)) + name_len) = L'\0';


        DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));


        long int pos = etell(file);
        if (pos < 0) {
            // we can't place this in the Buffer anymore
            int seek_res = eseek(file, dir_offset, SEEK_SET);
            if (seek_res < 0) {
                errno = eerrno_to_errno(eerrno);
                return -1;
            }
            DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));

            errno = EINVAL;
            return -1;
        }

        if (offset != nullptr) {
            *offset = static_cast<off_t>(pos);
        }
        DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));

        return size;

    } else {
        errno = EINVAL;
        return -1;
    }
}

/** EFI specific operations for writing to a file.

    @param[in]    filp        Pointer to a file descriptor structure.
    @param[in]    offset      Offset into the file to begin writing at, or NULL.
    @param[in]    BufferSize  Number of bytes in Buffer.  Max number of bytes to write.
    @param[in]    Buffer      Pointer to a buffer containing the data to be written.

    @return     Returns the number of bytes successfully written,
                or -1 if the operation failed.  Further information is specified by errno.
**/
static ssize_t EFIAPI
_f_romfs_Write(IN struct __filedes* filp, IN off_t* offset, IN size_t BufferSize, IN const void* Buffer) {
    errno = ENOTSUP;
    return -1;
}


static int EFIAPI _f_romfs_Fcntl(struct __filedes* filp, UINT32 Cmd, void* p3, void* p4) {
    errno = ENOTSUP;
    return -1;
}

/** EFI specific operations for getting information about an open file.

    @param[in]    filp        Pointer to a file descriptor structure.
    @param[out]   statbuf     Buffer in which to store the file status.
    @param[in]    Something   This parameter is not used by this device.

    @retval      0      Successful completion.
    @retval     -1      Operation failed.  Further information is specified by errno.
**/
static int EFIAPI _f_romfs_Stat(struct __filedes* filp, struct stat* statbuf, void* Something) {
    ASSERT_TYPE(filp->devdata, void*);
    ROMFS_DEV_DATA_TYPE* file = ROMFS_DEV_DATA_GET(filp->devdata);

    DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));

    // Got the info, now populate statbuf with it

    // NOTE: we return the same size in the case of a directory, but it isn't the directories detailed size, but an implementation defined size
    const off_t size = (off_t) esize(file);

    statbuf->st_size = size;
    statbuf->st_physsize = size;
    statbuf->st_curpos = 0;

    statbuf->st_birthtime = 0;
    statbuf->st_atime = 0;
    statbuf->st_mtime = 0;

    statbuf->st_mode = filp->f_iflags;
    statbuf->st_blksize = S_BLKSIZE;

    return 0;
}

/** EFI specific operations for low-level control of a file or device.

    @param[in]      filp    Pointer to a file descriptor structure.
    @param[in]      cmd     The command this ioctl is to perform.
    @param[in,out]  argp    Zero or more arguments as needed by the command.

    @retval      0      Successful completion.
    @retval     -1      Operation failed.  Further information is specified by errno.
**/
static int EFIAPI _f_romfs_Ioctl(struct __filedes* filp, ULONGN cmd, va_list argp) {
    errno = ENOTSUP;
    return -1;
}


// Wide to Normal
struct PathConversionWtoN {
private:
    const wchar_t* m_original;
    char* m_converted;

public:
    PathConversionWtoN(const wchar_t* value, char* converted) : m_original{ value }, m_converted{ converted } {
        //
    }

    static std::optional<PathConversionWtoN> init(const wchar_t* value) {
        char* converted = (char*) AllocateZeroPool(PATH_MAX + 1);
        if (converted == NULL) {
            errno = ENOMEM;
            return std::nullopt;
        }

        size_t convert_result = wcstombs(converted, value, PATH_MAX);

        if (convert_result == ((size_t) -1) || convert_result > PATH_MAX) {
            errno = EINVAL;
            return std::nullopt;
        }

        converted[convert_result] = '\0';

        // replace "\" with "/" for c-embed

        for (size_t i = 0; i < convert_result; ++i) {
            if (converted[i] == '\\') {
                converted[i] = '/';
            }
        }


        return PathConversionWtoN(value, converted);
    }

    PathConversionWtoN(const PathConversionWtoN& other) = delete;
    PathConversionWtoN& operator=(const PathConversionWtoN& other) = delete;


    PathConversionWtoN(PathConversionWtoN&& other) noexcept
        : m_original{ other.m_original },
          m_converted{ other.m_converted } {
        other.m_original = nullptr;
        other.m_converted = nullptr;
    }

    PathConversionWtoN& operator=(PathConversionWtoN&& other) noexcept {
        if (this != &other) {
            this->m_original = other.m_original;
            this->m_converted = other.m_converted;

            other.m_original = nullptr;
            other.m_converted = nullptr;
        }

        return *this;
    }


    [[nodiscard]] char* path() const {
        return m_converted;
    }

    ~PathConversionWtoN() {
        if (m_converted != nullptr) {
            FreePool(m_converted);
            m_converted = nullptr;
        }
    }
};

/** EFI specific operations for opening a file.

    @param[in]    DevNode       Pointer to the Device control structure for this stream.
    @param[in]    filp          Pointer to the new file control structure for this stream.
    @param[in]    DevInstance   Not used by this device.
    @param[in]    Path          File-system path to the file or directory.
    @param[in]    MPath         Not used by this device.

    @retval   0   This console stream has been successfully opened.
    @retval   -1  The DevNode or filp pointer is NULL.
    @retval   -1  DevNode does not point to a valid console stream device.
**/
int EFIAPI _f_romfs_Open(
        DeviceNode* DevNode,
        struct __filedes* filp,
        int DevInstance, /* Not used by romfs */
        wchar_t* Path,
        wchar_t* MPath
) {

    const int oflags_acc = filp->Oflags & O_ACCMODE;

    DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));

    if (oflags_acc != O_RDONLY) {
        DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));
        errno = EINVAL;
        return -1;
    }

    const int other_oflags = filp->Oflags & (~(O_ACCMODE));

    if (other_oflags == 0 || other_oflags == O_NONBLOCK) {
        // ok
    } else {
        DEBUG((DEBUG_ERROR, "%a %a:%d HERE 0x%x\n", __func__, __FILE__, __LINE__, other_oflags));
        errno = EINVAL;
        return -1;
    }

    std::optional<PathConversionWtoN> conversion = PathConversionWtoN::init(Path);

    if (not conversion.has_value()) {
        errno = EINVAL;
        return -1;
    }

    // Call the EFI's Open function
    ROMFS_DEV_DATA_TYPE* file = eopen(conversion->path(), "r");
    if (file == NULL) {
        filp->f_iflags = 0; // Release our reservation on this FD
        // Set errno based upon Status
        DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));
        errno = eerrno_to_errno(eerrno);
        return -1;
    }

    int type = estreamtype(file);
    if (type == EMAP_ENTRY_TYPE_FILE) {
        filp->f_iflags |= S_IFREG;
    } else if (type == EMAP_ENTRY_TYPE_DIR) {
        filp->f_iflags |= S_IFDIR;
    } else {
        DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));
        errno = EINVAL;
        return -1;
    }

    filp->f_iflags |= S_IROFS | S_IREADONLY;

    // Update the info in the fd
    ASSERT_TYPE(file, ROMFS_DEV_DATA_TYPE*);
    filp->devdata = ROMFS_DEV_DATA_ASSIGN(file);

    ROM_INSTANCE* Gip = (ROM_INSTANCE*) DevNode->InstanceList;
    filp->f_offset = 0;
    filp->f_ops = &(Gip->Abstraction);

    DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));
    return 0;
}

/** Returns a bit mask describing which operations could be completed immediately.

    For now, assume the file system, via the shell, is always ready.

    (POLLIN | POLLRDNORM)   The file system is ready to be read.
    (POLLOUT)               The file system is ready for output.

    @param[in]    filp    Pointer to a file descriptor structure.
    @param[in]    events  Bit mask describing which operations to check.

    @return     The returned value is a bit mask describing which operations
                could be completed immediately, without blocking.
**/
static short EFIAPI _f_romfs_Poll(struct __filedes* filp, short events) {
    errno = ENOTSUP;
    return -1;
}

static int EFIAPI _f_romfs_Flush(struct __filedes* filp) {
    errno = ENOTSUP;
    return -1;
}


/** EFI specific operations for renaming a file.

    @param[in]    from    Name of the file to be renamed.
    @param[in]    to      New name for the file.

    @retval      0      Successful completion.
    @retval     -1      Operation failed.  Further information is specified by errno.
**/
static int EFIAPI _f_romfs_Rename(const char* from, const char* to) {
    errno = ENOTSUP;
    return -1;
}

/** EFI specific operations for deleting directories.

    @param[in]    filp    Pointer to a file descriptor structure.

    @retval      0      Successful completion.
    @retval     -1      Operation failed.  Further information is specified by errno.
**/
static int EFIAPI _f_romfs_Rmdir(struct __filedes* filp) {
    errno = ENOTSUP;
    return -1;
}

#define ROMFS_NAME ((const CHAR16*) L"romfs:")

ROM_INSTANCE* _g_rom_stream_instance = NULL;

RETURN_STATUS
EFIAPI
__ctor_rom_fs(void) {
    ROM_INSTANCE* Stream = (ROM_INSTANCE*) AllocateZeroPool(sizeof(ROM_INSTANCE));
    if (Stream == nullptr) {
        return RETURN_OUT_OF_RESOURCES;
    }

    Stream->Cookie = ROM_COOKIE;
    Stream->InstanceNum = 1; // not used by this
    Stream->Dev = nullptr;

    Stream->Abstraction.fo_close = &_f_romfs_Close;
    Stream->Abstraction.fo_read = &_f_romfs_Read;
    Stream->Abstraction.fo_write = &_f_romfs_Write;
    Stream->Abstraction.fo_fcntl = &_f_romfs_Fcntl;
    Stream->Abstraction.fo_poll = &_f_romfs_Poll;
    Stream->Abstraction.fo_flush = &_f_romfs_Flush;
    Stream->Abstraction.fo_stat = &_f_romfs_Stat;
    Stream->Abstraction.fo_ioctl = &_f_romfs_Ioctl;
    Stream->Abstraction.fo_delete = &_f_romfs_Delete;
    Stream->Abstraction.fo_rmdir = &_f_romfs_Rmdir;
    Stream->Abstraction.fo_mkdir = &_f_romfs_Mkdir;
    Stream->Abstraction.fo_rename = &_f_romfs_Rename;
    Stream->Abstraction.fo_lseek = &_f_romfs_Seek;

    DeviceNode* Node = __DevRegister(ROMFS_NAME, nullptr, &_f_romfs_Open, Stream, 1, sizeof(ROM_INSTANCE), O_RDONLY);
    RETURN_STATUS Status = EFIerrno;
    Stream->Parent = Node;

    _g_rom_stream_instance = Stream;

    return Status;
}


static RETURN_STATUS EFIAPI __dtor__rom_fs(void) {
    if (_g_rom_stream_instance != nullptr) {
        FreePool(_g_rom_stream_instance);
    }
    return RETURN_SUCCESS;
}

struct RWFileSystemImpl {
    EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* SimpleFs;
    EFI_FILE_PROTOCOL* Root;

    ~RWFileSystemImpl() {
        this->LoadedImage = nullptr;
        this->SimpleFs = nullptr;
        this->Root = nullptr;
    }
};

#define RW_INSTANCE RWFSInstance

typedef struct {
    GenericInstance parent;
    EFI_FILE_PROTOCOL* Root;
} RWFSInstance;

VALIDATE_INSTANCE(RW_INSTANCE)

struct RWFileSystemImplGlobal {
    std::optional<RWFileSystemImpl> impl;
    RW_INSTANCE* stream_instance;

    void reset() {
        DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));
        this->~RWFileSystemImplGlobal();
        *this = { std::nullopt, nullptr };
    }


    ~RWFileSystemImplGlobal() {
        DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));
        if (this->impl.has_value()) {
            this->impl.value().~RWFileSystemImpl();
            this->impl = std::nullopt;
        }
        if (this->stream_instance != nullptr) {
            FreePool(this->stream_instance);
            this->stream_instance = nullptr;
        }
    }
};


#define RWFS_DEV_DATA_ASSIGN(data) ((void*) data)
#define RWFS_DEV_DATA_TYPE EFI_FILE_PROTOCOL
#define RWFS_DEV_DATA_GET(data) ((RWFS_DEV_DATA_TYPE*) data)

/** EFI specific operations for close().

    @param[in]    filp    Pointer to a file descriptor structure.

    @retval      0      Successful completion.
    @retval     -1      Operation failed.  Further information is specified by errno.
**/
static int EFIAPI _f_rwfs_Close(IN struct __filedes* filp) {
    ASSERT_TYPE(filp->devdata, void*);
    RWFS_DEV_DATA_TYPE* file = RWFS_DEV_DATA_GET(filp->devdata);

    file->Close(file);
    return 0;
}

/** EFI specific operations for deleting a file or directory.

    @param[in]    filp    Pointer to a file descriptor structure.

    @retval      0      Successful completion.
    @retval     -1      Operation failed.  Further information is specified by errno.
**/
static int EFIAPI _f_rwfs_Delete(struct __filedes* filp) {
    //TODO
    errno = ENOTSUP;
    return -1;
}

/** EFI specific operations for setting the position within a file.

    @param[in]    filp    Pointer to a file descriptor structure.
    @param[in]    offset  Relative position to move to.
    @param[in]    whence  Specifies the location offset is relative to: Beginning, Current, End.

    @return     Returns the new file position or EOF if the seek failed.
**/
static off_t EFIAPI _f_rwfs_Seek(struct __filedes* filp, off_t offset, int whence) {
    ASSERT_TYPE(filp->devdata, void*);
    RWFS_DEV_DATA_TYPE* file = RWFS_DEV_DATA_GET(filp->devdata);


    //TODO
    (void) file;
    return -1;
}

/** The directory path is created with the access permissions specified by
    perms.

    The directory is closed after it is created.

    @param[in]    path      The directory to be created.
    @param[in]    perms     Access permissions for the new directory.

    @retval   0   The directory was created successfully.
    @retval  -1   An error occurred and an error code is stored in errno.
**/
static int EFIAPI _f_rwfs_Mkdir(const char* path, __mode_t perms) {
    errno = ENOTSUP;
    return -1;
}

/** EFI specific operations for reading from a file.

    @param[in]    filp        Pointer to a file descriptor structure.
    @param[in]    offset      Offset into the file to begin reading at, or NULL.
    @param[in]    BufferSize  Number of bytes in Buffer.  Max number of bytes to read.
    @param[in]    Buffer      Pointer to a buffer to receive the read data.

    @return     Returns the number of bytes successfully read,
                or -1 if the operation failed.  Further information is specified by errno.
**/
static ssize_t EFIAPI
_f_rwfs_Read(IN OUT struct __filedes* filp, IN OUT off_t* offset, IN size_t BufferSize, OUT VOID* Buffer) {
    ASSERT_TYPE(filp->devdata, void*);
    RWFS_DEV_DATA_TYPE* file = RWFS_DEV_DATA_GET(filp->devdata);

    //TODO: support directory, how is read used there?

    //TODO
    (void) file;
    return -1;
}

/** EFI specific operations for writing to a file.

    @param[in]    filp        Pointer to a file descriptor structure.
    @param[in]    offset      Offset into the file to begin writing at, or NULL.
    @param[in]    BufferSize  Number of bytes in Buffer.  Max number of bytes to write.
    @param[in]    Buffer      Pointer to a buffer containing the data to be written.

    @return     Returns the number of bytes successfully written,
                or -1 if the operation failed.  Further information is specified by errno.
**/
static ssize_t EFIAPI
_f_rwfs_Write(IN struct __filedes* filp, IN off_t* offset, IN size_t BufferSize, IN const void* Buffer) {
    //TODO
    errno = ENOTSUP;
    return -1;
}


static int EFIAPI _f_rwfs_Fcntl(struct __filedes* filp, UINT32 Cmd, void* p3, void* p4) {
    //TODO: maybe used in readdir or opendir
    errno = ENOTSUP;
    return -1;
}

/** EFI specific operations for getting information about an open file.

    @param[in]    filp        Pointer to a file descriptor structure.
    @param[out]   statbuf     Buffer in which to store the file status.
    @param[in]    Something   This parameter is not used by this device.

    @retval      0      Successful completion.
    @retval     -1      Operation failed.  Further information is specified by errno.
**/
static int EFIAPI _f_rwfs_Stat(struct __filedes* filp, struct stat* statbuf, void* Something) {
    ASSERT_TYPE(filp->devdata, void*);
    RWFS_DEV_DATA_TYPE* file = RWFS_DEV_DATA_GET(filp->devdata);


    EFI_FILE_INFO* FileInfo = NULL;
    UINTN FileInfoSize = 0;

    EFI_STATUS Status = file->GetInfo(file, &gEfiFileInfoGuid, &FileInfoSize, FileInfo);

    if (Status == EFI_BUFFER_TOO_SMALL) {
        FileInfo = (EFI_FILE_INFO*) AllocatePool(FileInfoSize);
        if (FileInfo == NULL) {
            errno = ENOMEM;
            return -1;
        }
    }
    OnScopeEnd end{ [&FileInfo]() -> void { FreePool(FileInfo); } };


    Status = file->GetInfo(file, &gEfiFileInfoGuid, &FileInfoSize, FileInfo);

    if (EFI_ERROR(Status)) {
        errno = EINVAL;
        return -1;
    }


    // Got the info, now populate statbuf with it


    statbuf->st_size = FileInfo->FileSize;
    statbuf->st_physsize = FileInfo->PhysicalSize;
    statbuf->st_curpos = 0;

    statbuf->st_birthtime = Efi2Time(&(FileInfo->CreateTime));
    statbuf->st_atime = Efi2Time(&(FileInfo->LastAccessTime));
    statbuf->st_mtime = Efi2Time(&(FileInfo->ModificationTime));

    statbuf->st_mode = 0;
    statbuf->st_blksize = S_BLKSIZE;

    if ((FileInfo->Attribute & EFI_FILE_READ_ONLY) != 0) {
        statbuf->st_mode |= S_IREADONLY;
    }

    if ((FileInfo->Attribute & EFI_FILE_HIDDEN) != 0) {
        statbuf->st_mode |= S_IHIDDEN;
    }

    if ((FileInfo->Attribute & EFI_FILE_SYSTEM) != 0) {
        statbuf->st_mode |= S_ISYSTEM;
    }

    if ((FileInfo->Attribute & EFI_FILE_RESERVED) != 0) {
        //noop
    }

    if ((FileInfo->Attribute & EFI_FILE_DIRECTORY) != 0) {
        statbuf->st_mode |= S_IFDIR | S_IDIRECTORY;
    } else {
        statbuf->st_mode |= S_IFREG;
    }

    if ((FileInfo->Attribute & EFI_FILE_ARCHIVE) != 0) {
        statbuf->st_mode |= S_IARCHIVE;
    }


    return 0;
}

/** EFI specific operations for low-level control of a file or device.

    @param[in]      filp    Pointer to a file descriptor structure.
    @param[in]      cmd     The command this ioctl is to perform.
    @param[in,out]  argp    Zero or more arguments as needed by the command.

    @retval      0      Successful completion.
    @retval     -1      Operation failed.  Further information is specified by errno.
**/
static int EFIAPI _f_rwfs_Ioctl(struct __filedes* filp, ULONGN cmd, va_list argp) {
    errno = ENOTSUP;
    return -1;
}


/** EFI specific operations for opening a file.

    @param[in]    DevNode       Pointer to the Device control structure for this stream.
    @param[in]    filp          Pointer to the new file control structure for this stream.
    @param[in]    DevInstance   Not used by this device.
    @param[in]    Path          File-system path to the file or directory.
    @param[in]    MPath         Not used by this device.

    @retval   0   This console stream has been successfully opened.
    @retval   -1  The DevNode or filp pointer is NULL.
    @retval   -1  DevNode does not point to a valid console stream device.
**/
int EFIAPI _f_rwfs_Open(
        DeviceNode* DevNode,
        struct __filedes* filp,
        int DevInstance, /* Not used by romfs */
        wchar_t* Path,
        wchar_t* MPath
) {
    if ((filp == NULL) || (DevNode == NULL)) {
        errno = EINVAL;
        return -1;
    }

    RW_INSTANCE* Gip = (RW_INSTANCE*) DevNode->InstanceList;

    //TODO
    /*  RWFS_DEV_DATA_TYPE* file = Gip->Root->Open(Gip->Root, TODO, Path, filp->Oflags);
    if (file == NULL) {
        filp->f_iflags = 0; // Release our reservation on this FD
        // Set errno based upon Status
        errno = eerrno_to_errno(eerrno);
        return -1;
    }

    int type = estreamtype(file);
    if (type == EMAP_ENTRY_TYPE_FILE) {
        filp->f_iflags |= S_IFREG;
    } else if (type == EMAP_ENTRY_TYPE_DIR) {
        filp->f_iflags |= S_IFDIR | S_IDIRECTORY;
    } else {
        errno = EINVAL;
        return -1;
    }

    filp->f_iflags |= S_IROFS | S_IREADONLY;

    // Update the info in the fd
    ASSERT_TYPE(file, RWFS_DEV_DATA_TYPE*);
    filp->devdata = RWFS_DEV_DATA_ASSIGN(file);
 */

    filp->f_offset = 0;
    filp->f_ops = &(Gip->parent.Abstraction);

    return 0;
}

/** Returns a bit mask describing which operations could be completed immediately.

    For now, assume the file system, via the shell, is always ready.

    (POLLIN | POLLRDNORM)   The file system is ready to be read.
    (POLLOUT)               The file system is ready for output.

    @param[in]    filp    Pointer to a file descriptor structure.
    @param[in]    events  Bit mask describing which operations to check.

    @return     The returned value is a bit mask describing which operations
                could be completed immediately, without blocking.
**/
static short EFIAPI _f_rwfs_Poll(struct __filedes* filp, short events) {
    errno = ENOTSUP;
    return -1;
}

static int EFIAPI _f_rwfs_Flush(struct __filedes* filp) {
    errno = ENOTSUP;
    return -1;
}


/** EFI specific operations for renaming a file.

    @param[in]    from    Name of the file to be renamed.
    @param[in]    to      New name for the file.

    @retval      0      Successful completion.
    @retval     -1      Operation failed.  Further information is specified by errno.
**/
static int EFIAPI _f_rwfs_Rename(const char* from, const char* to) {
    errno = ENOTSUP;
    return -1;
}

/** EFI specific operations for deleting directories.

    @param[in]    filp    Pointer to a file descriptor structure.

    @retval      0      Successful completion.
    @retval     -1      Operation failed.  Further information is specified by errno.
**/
static int EFIAPI _f_rwfs_Rmdir(struct __filedes* filp) {
    errno = ENOTSUP;
    return -1;
}


#define RWFS_NAME ((const CHAR16*) L"root:")


static RWFileSystemImplGlobal g_rw_file_system = { std::nullopt, nullptr };


static RETURN_STATUS EFIAPI __ctor_rw_fs(EFI_FILE_PROTOCOL* Root) {
    RW_INSTANCE* Stream = (RW_INSTANCE*) AllocateZeroPool(sizeof(RW_INSTANCE));
    if (Stream == nullptr) {
        return RETURN_OUT_OF_RESOURCES;
    }

    GenericInstance* GI = &(Stream->parent);

    GI->Cookie = RW_COOKIE;
    GI->InstanceNum = 1; // not used by this
    GI->Dev = nullptr;

    GI->Abstraction.fo_close = &_f_rwfs_Close;
    GI->Abstraction.fo_read = &_f_rwfs_Read;
    GI->Abstraction.fo_write = &_f_rwfs_Write;
    GI->Abstraction.fo_fcntl = &_f_rwfs_Fcntl;
    GI->Abstraction.fo_poll = &_f_rwfs_Poll;
    GI->Abstraction.fo_flush = &_f_rwfs_Flush;
    GI->Abstraction.fo_stat = &_f_rwfs_Stat;
    GI->Abstraction.fo_ioctl = &_f_rwfs_Ioctl;
    GI->Abstraction.fo_delete = &_f_rwfs_Delete;
    GI->Abstraction.fo_rmdir = &_f_rwfs_Rmdir;
    GI->Abstraction.fo_mkdir = &_f_rwfs_Mkdir;
    GI->Abstraction.fo_rename = &_f_rwfs_Rename;
    GI->Abstraction.fo_lseek = &_f_rwfs_Seek;

    DeviceNode* Node = __DevRegister(RWFS_NAME, nullptr, &_f_rwfs_Open, Stream, 1, sizeof(RW_INSTANCE), O_RDWR);
    RETURN_STATUS Status = EFIerrno;
    GI->Parent = Node;

    Stream->Root = Root;

    g_rw_file_system.stream_instance = Stream;

    return Status;
}


static std::expected<RWFileSystemImpl, EFI_STATUS> get_rw_fs_impl() {

    EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;

    EFI_STATUS Status = gBS->HandleProtocol(gImageHandle, &gEfiLoadedImageProtocolGuid, (VOID**) &LoadedImage);

    if (EFI_ERROR(Status)) {
        return std::unexpected<EFI_STATUS>{ Status };
    }

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* SimpleFs;

    Status = gBS->HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID**) &SimpleFs);

    if (EFI_ERROR(Status)) {
        return std::unexpected<EFI_STATUS>{ Status };
    }

    EFI_FILE_PROTOCOL* Root;
    Status = SimpleFs->OpenVolume(SimpleFs, &Root);

    if (EFI_ERROR(Status)) {
        return std::unexpected<EFI_STATUS>{ Status };
    }

    return RWFileSystemImpl{ LoadedImage, SimpleFs, Root };
}

static RETURN_STATUS EFIAPI __copy_rom_automount_files(void) {


    const auto rw_automount_folder = std::filesystem::path{ "romfs:/rw_auto_mount" };

    if (not std::filesystem::exists(rw_automount_folder)) {
        return RETURN_NOT_FOUND;
    }


    std::cerr << "FOLDER: " << rw_automount_folder << "\n";

    for (const auto& file : std::filesystem::recursive_directory_iterator(rw_automount_folder)) {

        if (file.is_directory()) {
            std::cerr << "FOLDER: " << file << "\n";
        } else {
            std::cerr << "FILE: " << file << "\n";
        }
    }

    DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));

    return RETURN_SUCCESS;
}

static RETURN_STATUS EFIAPI __ctor_optional_rw_fs(void) {

    // find rw file system
    auto impl = get_rw_fs_impl();

    if (impl.has_value()) {
        auto impl_value = impl.value();
        g_rw_file_system = { impl_value, nullptr };

        // if we have one, mount it for libc
        auto status = __ctor_rw_fs(impl_value.Root);
        if (EFI_ERROR(status)) {
            g_rw_file_system.reset();
            return status;
        }

        // copy from auto mount to rw file system. if it not already exists
        status = __copy_rom_automount_files();
        if (EFI_ERROR(status)) {
            DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));
            g_rw_file_system.reset();
            DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));
            return status;
        }

        DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));
        return RETURN_SUCCESS;
    }

    g_rw_file_system.reset();

    return RETURN_SUCCESS;
}


static RETURN_STATUS EFIAPI __dtor__rw_fs(void) {
    g_rw_file_system.~RWFileSystemImplGlobal();
    return RETURN_SUCCESS;
}

void uefi::platform_init() {
    auto status = __ctor_rom_fs();
    if (EFI_ERROR(status)) {
        utils::throw_(
                std::runtime_error{ fmt::format("can't initialize ROM fs: {}", map_efi_status_to_string(status)) }
        );
    }

    status = __ctor_optional_rw_fs();
    if (EFI_ERROR(status)) {
        utils::throw_(
                std::runtime_error{ fmt::format("can't initialize RW fs: {}", map_efi_status_to_string(status)) }
        );
    }

    DEBUG((DEBUG_ERROR, "%a %a:%d HERE %d\n", __func__, __FILE__, __LINE__, status));

    //TODO: uefi related stuff
    // have dedicated uefi settings
    // set GOP mode, auto or 0-<max num>
    // set keyboard layout in sdl
    // find fat system where the file is, and than use it to store settings, uefi_settings and recordings
    // log things when we have a file system
    // make uefi settings page with info (built for hardware, emulator) cpuid and the settings from above!
}

void uefi::platform_exit() {
    auto status = __dtor__rom_fs();
    if (EFI_ERROR(status)) {
        utils::throw_(
                std::runtime_error{ fmt::format("can't deinitialize ROM fs: {}", map_efi_status_to_string(status)) }
        );
    }

    status = __dtor__rw_fs();
    if (EFI_ERROR(status)) {
        utils::throw_(
                std::runtime_error{ fmt::format("can't deinitialize RW fs: {}", map_efi_status_to_string(status)) }
        );
    }
}


OOPETRIS_GRAPHICS_EXPORTED std::optional<uefi::RWFileSystem> uefi::get_rw_file_system_info() {

    DEBUG((DEBUG_ERROR, "%a %a:%d HERE\n", __func__, __FILE__, __LINE__));

    if (g_rw_file_system.stream_instance != nullptr) {
        return RWFileSystem{};
    }

    return std::nullopt;
}
