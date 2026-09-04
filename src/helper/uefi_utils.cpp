

#if !defined(__UEFI__)
#error "Only supported on UEFI"
#endif

#include <core/helper/utils.hpp>

#include "./uefi_utils.hpp"

#include <memory>
#include <string>

extern "C" {
#include <Library/BaseLib.h>
#include <Library/SynchronizationLib.h>
}


std::shared_ptr<spdlog::sinks::callback_sink_mt> uefi::get_debug_sink() {
    return std::make_shared<spdlog::sinks::callback_sink_mt>([](const spdlog::details::log_msg& msg) {
        const std::string message = std::string{ msg.payload.begin(), msg.payload.end() };


        switch (msg.level) {
            case spdlog::level::off:
                return;
            case spdlog::level::trace:
            case spdlog::level::debug:
                EFI_DEBUG((DEBUG_VERBOSE, "%a\n", message.c_str()));
                break;
            case spdlog::level::info:
                EFI_DEBUG((DEBUG_INFO, "%a\n", message.c_str()));
                break;
            case spdlog::level::warn:
                EFI_DEBUG((DEBUG_WARN, "%a\n", message.c_str()));
                break;
            case spdlog::level::err:
            case spdlog::level::critical:
                EFI_DEBUG((DEBUG_ERROR, "%a\n", message.c_str()));
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

extern "C" {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"

#define rewind erewind
#include <c-embed.h>
#undef rewind

#pragma GCC diagnostic pop

#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Uefi.h>

// use stdlib internals, to register a new rom device under romfs:
#include <StdLibPrivateInternalFiles/Include/kfile.h>

#include <StdLibPrivateInternalFiles/Include/Device/Device.h>
}

#define ROM_COOKIE 0x464d4f52 ///< 'RomF'


/** EFI specific operations for close().

    @param[in]    Fp    Pointer to a file descriptor structure.

    @retval      0      Successful completion.
    @retval     -1      Operation failed.  Further information is specified by errno.
**/
static int EFIAPI _f_romfs_Close(IN struct __filedes* Fp) {
    eclose((EFILE*) (&Fp->devdata));
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

[[nodiscard]] static int eerrno_to_errno(int eerrno);

/** EFI specific operations for setting the position within a file.

    @param[in]    filp    Pointer to a file descriptor structure.
    @param[in]    offset  Relative position to move to.
    @param[in]    whence  Specifies the location offset is relative to: Beginning, Current, End.

    @return     Returns the new file position or EOF if the seek failed.
**/
static off_t EFIAPI _f_romfs_Seek(struct __filedes* filp, off_t offset, int whence) {
    EFILE* file = (EFILE*) (&filp->devdata);

    //NOTE: eseek works differently than the expected seek, so map the behavior

    int result = eseek(file, offset, whence);
    if (result != 0) {
        errno = eerrno_to_errno(eerrno);
        return EOF;
    }

    //NOTE: etell cannot fail
    return etell(file);
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

    EFILE* stream = (EFILE*) (&filp->devdata);

    if (offset != NULL) {
        //TODO: support
        errno = ENOTSUP;
        return -1;
    }
    // NOTE: eread cannot fail, it doesn't return negative values

    return (ssize_t) eread(Buffer, BufferSize, 1, stream);
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
    EFILE* file = (EFILE*) (&filp->devdata);


    // Got the info, now populate statbuf with it
    statbuf->st_size = file->size;
    statbuf->st_physsize = 0;
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


struct PathConversion {
private:
    const wchar_t* m_original;
    char* m_converted;

public:
    PathConversion(const wchar_t* value, char* converted) : m_original{ value }, m_converted{ converted } {
        //
    }

    static std::optional<PathConversion> init(const wchar_t* value) {
        char* converted = (char*) AllocateZeroPool(PATH_MAX + 1);
        if (converted == NULL) {
            DEBUG((DEBUG_ERROR, "%a %a:%d: IN PathConversion::init %s\n", __func__, __FILE__, __LINE__, value));
            errno = ENOMEM;
            return std::nullopt;
        }

        size_t convert_result = wcstombs(converted, value, PATH_MAX);

        if (convert_result == ((size_t) -1) || convert_result > PATH_MAX) {
            DEBUG((DEBUG_ERROR, "%a %a:%d: IN PathConversion::init %s\n", __func__, __FILE__, __LINE__, value));
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


        return PathConversion(value, converted);
    }

    PathConversion(const PathConversion& other) = delete;
    PathConversion& operator=(const PathConversion& other) = delete;


    PathConversion(PathConversion&& other) noexcept : m_original{ other.m_original }, m_converted{ other.m_converted } {
        other.m_original = nullptr;
        other.m_converted = nullptr;
    }

    PathConversion& operator=(PathConversion&& other) noexcept {
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

    ~PathConversion() {
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
    if (filp->Oflags != O_RDONLY) {
        DEBUG((DEBUG_ERROR, "%a %a:%d: IN OPEN %s\n", __func__, __FILE__, __LINE__, Path));
        errno = EINVAL;
        return -1;
    }


    std::optional<PathConversion> conversion = PathConversion::init(Path);

    if (not conversion.has_value()) {
        return -1;
    }

    DEBUG((DEBUG_ERROR, "%a %a:%d: path: %a\n", __func__, __FILE__, __LINE__, conversion->path()));

    // Call the EFI's Open function
    EFILE* file = eopen(conversion->path(), "r");
    if (file == NULL) {
        filp->f_iflags = 0; // Release our reservation on this FD
        // Set errno based upon Status
        errno = eerrno_to_errno(eerrno);
        DEBUG((DEBUG_ERROR, "%a %a:%d: IN OPEN %s: errno -> %a\n", __func__, __FILE__, __LINE__, Path, eerrstr(eerrno)));
        return -1;
    }


    // Successfully got a regular File (note c-embed doesnÄt support to open directories)
    filp->f_iflags |= S_IFREG;

    // Update the info in the fd
    filp->devdata = (void*) file;

    GenericInstance* Gip = (GenericInstance*) DevNode->InstanceList;
    filp->f_offset = 0;
    filp->f_ops = &Gip->Abstraction;

    DEBUG((DEBUG_ERROR, "%a %a:%d: IN OPEN %s\n", __func__, __FILE__, __LINE__, Path));
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

GenericInstance* _g_stream_instance = NULL;

RETURN_STATUS
EFIAPI
__ctor_rom_fs(void) {
    GenericInstance* Stream = (GenericInstance*) AllocateZeroPool(sizeof(GenericInstance));
    if (Stream == NULL) {
        return RETURN_OUT_OF_RESOURCES;
    }

    Stream->Cookie = ROM_COOKIE;
    Stream->InstanceNum = 1; // not used by this
    Stream->Dev = NULL;

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

    DeviceNode* Node = __DevRegister(ROMFS_NAME, NULL, &_f_romfs_Open, Stream, 1, sizeof(GenericInstance), O_RDONLY);
    RETURN_STATUS Status = EFIerrno;
    Stream->Parent = Node;

    _g_stream_instance = Stream;

    return Status;
}


[[nodiscard]] static int eerrno_to_errno(int eerrno) {
    switch (eerrno) {
        case EERRCODE_SUCCESS:
            return 0;
        case EERRCODE_NOFILE:
            return ENOENT;
        case EERRCODE_NOMAP:
            return ENODEV;
        case EERRCODE_NULLSTREAM:
            return EINVAL;
        case EERRCODE_OOBSTREAMPOS:
            return EINVAL;
        default:
            return EINVAL;
    };
}


RETURN_STATUS
EFIAPI
__dtor__rom_fs(void) {
    if (_g_stream_instance != NULL) {
        FreePool(_g_stream_instance);
    }
    return RETURN_SUCCESS;
}


void uefi::platform_init() {
    auto status = __ctor_rom_fs();
    if (EFI_ERROR(status)) {
        utils::throw_(
                std::runtime_error{ fmt::format("can't initialize ROM fs: {}", map_efi_status_to_string(status)) }
        );
    }
}

void uefi::platform_exit() {
    auto status = __dtor__rom_fs();
    if (EFI_ERROR(status)) {
        utils::throw_(
                std::runtime_error{ fmt::format("can't deinitialize ROM fs: {}", map_efi_status_to_string(status)) }
        );
    }
}
