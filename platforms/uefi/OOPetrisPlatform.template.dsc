[Defines]
  PLATFORM_NAME           = OOPetrisModule
  PLATFORM_GUID           = 6e7aa053-beda-40a8-b729-375bd9c71e2c
  PLATFORM_VERSION        = 1.0
  DSC_SPECIFICATION       = 0x0001001B
  OUTPUT_DIRECTORY        = Build/OOPetrisModule
  SUPPORTED_ARCHITECTURES = X64
  BUILD_TARGETS           = DEBUG|RELEASE
  SKUID_IDENTIFIER        = DEFAULT


!ifndef OOPETRIS_RUNTIME_TARGET
  !error "OOPETRIS_RUNTIME_TARGET must be set"
!endif

  #
  # RUNTIME TARGET related defintions
  #

!if $(OOPETRIS_RUNTIME_TARGET) == "hardware"
  DEFINE PLAT_QEMU               = FALSE
  DEFINE QEMU_PV_VARS            = FALSE
  DEFINE DEBUG_ON_SERIAL_PORT    = FALSE
!elseif $(OOPETRIS_RUNTIME_TARGET) == "emulator"
  DEFINE PLAT_QEMU               = TRUE
  DEFINE QEMU_PV_VARS            = FALSE
  DEFINE DEBUG_ON_SERIAL_PORT    = TRUE
!else
  !error "OOPETRIS_RUNTIME_TARGET has invalid value"
!endif

  #
  # Default definitions
  #

  DEFINE TTY_TERMINAL            = FALSE
  DEFINE DEBUG_TO_MEM            = FALSE

  #
  # Network definition
  #
  DEFINE NETWORK_SNP_ENABLE       = FALSE
  DEFINE NETWORK_IP6_ENABLE       = TRUE
  DEFINE NETWORK_TLS_ENABLE       = TRUE
  DEFINE NETWORK_HTTP_BOOT_ENABLE = FALSE
  DEFINE NETWORK_ALLOW_HTTP_CONNECTIONS  = TRUE
  DEFINE NETWORK_HTTP_ENABLE      = TRUE
  DEFINE NETWORK_ISCSI_ENABLE     = FALSE
  DEFINE SECURE_BOOT_ENABLE       = FALSE
  DEFINE NETWORK_PXE_BOOT_ENABLE         = FALSE

  #
  # Other defintions
  #

  DEFINE CUSTOM_STACK_CHECK_LIB = "STATIC"



  #
  # Redfish definition
  #
  DEFINE REDFISH_ENABLE = FALSE

!include MdePkg/MdeLibs.dsc.inc

[Packages]
  MdePkg/MdePkg.dec
  UefiCpuPkg/UefiCpuPkg.dec

[LibraryClasses]
  #
  # Entry point
  #
  PeiCoreEntryPoint|MdePkg/Library/PeiCoreEntryPoint/PeiCoreEntryPoint.inf
  PeimEntryPoint|MdePkg/Library/PeimEntryPoint/PeimEntryPoint.inf
  DxeCoreEntryPoint|MdePkg/Library/DxeCoreEntryPoint/DxeCoreEntryPoint.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf

  #
  # Basic
  #
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
  PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf
  PciSegmentLib|MdePkg/Library/BasePciSegmentLibPci/BasePciSegmentLibPci.inf
  CacheMaintenanceLib|MdePkg/Library/BaseCacheMaintenanceLib/BaseCacheMaintenanceLib.inf
  PeCoffLib|MdePkg/Library/BasePeCoffLib/BasePeCoffLib.inf
  PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  SortLib|MdeModulePkg/Library/BaseSortLib/BaseSortLib.inf

  #
  # UEFI & PI
  #
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiDecompressLib|MdePkg/Library/BaseUefiDecompressLib/BaseUefiDecompressLib.inf
  UefiBootManagerLib|MdeModulePkg/Library/UefiBootManagerLib/UefiBootManagerLib.inf
  VariablePolicyLib|MdeModulePkg/Library/VariablePolicyLib/VariablePolicyLib.inf

  #
  # Generic Modules
  #
  UefiUsbLib|MdePkg/Library/UefiUsbLib/UefiUsbLib.inf
  UefiScsiLib|MdePkg/Library/UefiScsiLib/UefiScsiLib.inf
  SecurityManagementLib|MdeModulePkg/Library/DxeSecurityManagementLib/DxeSecurityManagementLib.inf
  ## see below: TimerLib|MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf

!if $(OOPETRIS_RUNTIME_TARGET) == "hardware"
  # DebugLib
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf-TODO-not-implemented-and-checked-yet

  TODO|not implemented and checked yet
  # SupportLib and TimerLib
  #TODO: doesn't work on qemu, but maybe on CPU??
  TimerLib|UefiCpuPkg/Library/CpuTimerLib/BaseCpuTimerLib.inf
  LibUEfiSupportNanosleep|SupportLib/Library/Default/SupportLibDefaultNanosleep.inf
  LibUEfiSupportClock|SupportLib/Library/TimerLib/SupportLibTimerClock.inf
!else
  # DebugLib
  DebugLib|OvmfPkg/Library/PlatformDebugLibIoPort/PlatformDebugLibIoPort.inf
  MemDebugLogLib|OvmfPkg/Library/MemDebugLogLib/MemDebugLogLibNull.inf

  # Support and TimerLib
  LibUEfiSupportNanosleep|SupportLib/Library/Default/SupportLibDefaultNanosleep.inf
  LibUEfiSupportClock|SupportLib/Library/TimerLib/SupportLibTimerClock.inf
  #LibUEfiSupportClock|LibraryPkg/SupportLib/Library/Default/SupportLibDefaultClock.inf
  #LibUEfiSupportClock|LibraryPkg/SupportLib/Library/Null/SupportLibNullClock.inf

  ## doesn't work on qemu, setup (cpuid leaf 0x15) error
  ## TimerLib|UefiCpuPkg/Library/CpuTimerLib/BaseCpuTimerLib.inf

  ## not supported for DXE or UEFI_APPLICATION:
  ## TimerLib|OvmfPkg/Library/AcpiTimerLib/BaseAcpiTimerLib.inf

  ## NOT WORKING on some machines, works with q35, but not the default (pc?),
  ## the reason is Pmba reading is incorrect in the timerlib Constructor :(
  ## TimerLib|OvmfPkg/Library/AcpiTimerLib/DxeAcpiTimerLib.inf

  ## not working: need special emulator?
  ## TimerLib|EmulatorPkg/Library/DxeTimerLib/DxeTimerLib.inf

  TimerLib|OvmfPkg/Library/AcpiTimerLib/DxeAcpiTimerLib.inf

!endif

  ShellCEntryLibDynamical|SupportLib/Library/UefiShellCEntryLibDynamical/UefiShellCEntryLibDynamical.inf


  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  CustomizedDisplayLib|MdeModulePkg/Library/CustomizedDisplayLib/CustomizedDisplayLib.inf
  FrameBufferBltLib|MdeModulePkg/Library/FrameBufferBltLib/FrameBufferBltLib.inf

  #
  # Misc
  #
  ## see above: DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  ReportStatusCodeLib|MdePkg/Library/BaseReportStatusCodeLibNull/BaseReportStatusCodeLibNull.inf
  PeCoffExtraActionLib|MdePkg/Library/BasePeCoffExtraActionLibNull/BasePeCoffExtraActionLibNull.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf
  PlatformHookLib|MdeModulePkg/Library/BasePlatformHookLibNull/BasePlatformHookLibNull.inf
  ResetSystemLib|MdeModulePkg/Library/BaseResetSystemLibNull/BaseResetSystemLibNull.inf
  SmbusLib|MdePkg/Library/DxeSmbusLib/DxeSmbusLib.inf
  S3BootScriptLib|MdeModulePkg/Library/PiDxeS3BootScriptLib/DxeS3BootScriptLib.inf
  CpuExceptionHandlerLib|MdeModulePkg/Library/CpuExceptionHandlerLibNull/CpuExceptionHandlerLibNull.inf
  PlatformBootManagerLib|MdeModulePkg/Library/PlatformBootManagerLibNull/PlatformBootManagerLibNull.inf
  PciHostBridgeLib|MdeModulePkg/Library/PciHostBridgeLibNull/PciHostBridgeLibNull.inf
  TpmMeasurementLib|MdeModulePkg/Library/TpmMeasurementLibNull/TpmMeasurementLibNull.inf
  TdxMeasurementLib|MdeModulePkg/Library/TdxMeasurementLibNull/TdxMeasurementLibNull.inf
  AuthVariableLib|MdeModulePkg/Library/AuthVariableLibNull/AuthVariableLibNull.inf
  VarCheckLib|MdeModulePkg/Library/VarCheckLib/VarCheckLib.inf
  FileExplorerLib|MdeModulePkg/Library/FileExplorerLib/FileExplorerLib.inf
  NonDiscoverableDeviceRegistrationLib|MdeModulePkg/Library/NonDiscoverableDeviceRegistrationLib/NonDiscoverableDeviceRegistrationLib.inf
  ImagePropertiesRecordLib|MdeModulePkg/Library/ImagePropertiesRecordLib/ImagePropertiesRecordLib.inf

  FmpAuthenticationLib|MdeModulePkg/Library/FmpAuthenticationLibNull/FmpAuthenticationLibNull.inf
  BmpSupportLib|MdeModulePkg/Library/BaseBmpSupportLib/BaseBmpSupportLib.inf
  SafeIntLib|MdePkg/Library/BaseSafeIntLib/BaseSafeIntLib.inf
  DisplayUpdateProgressLib|MdeModulePkg/Library/DisplayUpdateProgressLibGraphics/DisplayUpdateProgressLibGraphics.inf
  VariablePolicyHelperLib|MdeModulePkg/Library/VariablePolicyHelperLib/VariablePolicyHelperLib.inf
  MmUnblockMemoryLib|MdePkg/Library/MmUnblockMemoryLib/MmUnblockMemoryLibNull.inf
  VariableFlashInfoLib|MdeModulePkg/Library/BaseVariableFlashInfoLib/BaseVariableFlashInfoLib.inf
  IpmiCommandLib|MdeModulePkg/Library/BaseIpmiCommandLibNull/BaseIpmiCommandLibNull.inf
  SpiHcPlatformLib|MdeModulePkg/Library/BaseSpiHcPlatformLibNull/BaseSpiHcPlatformLibNull.inf

[LibraryClasses.common.DXE_CORE]
  HobLib|MdePkg/Library/DxeCoreHobLib/DxeCoreHobLib.inf
  MemoryAllocationLib|MdeModulePkg/Library/DxeCoreMemoryAllocationLib/DxeCoreMemoryAllocationLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf

[LibraryClasses.common.DXE_DRIVER]
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxDxeLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibFmp/DxeCapsuleLib.inf

[LibraryClasses.common.UEFI_DRIVER]
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  #see above: DebugLib|MdePkg/Library/UefiDebugLibConOut/UefiDebugLibConOut.inf
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxDxeLib.inf


[LibraryClasses.common.UEFI_APPLICATION]
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  #see above: DebugLib|MdePkg/Library/UefiDebugLibStdErr/UefiDebugLibStdErr.inf
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf

#start crypto related libs
[LibraryClasses]
  ## TimerLib|MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
  RngLib|MdePkg/Library/BaseRngLibNull/BaseRngLibNull.inf
  OemHookStatusCodeLib|MdeModulePkg/Library/OemHookStatusCodeLibNull/OemHookStatusCodeLibNull.inf
  HashApiLib|CryptoPkg/Library/BaseHashApiLib/BaseHashApiLib.inf
  OpensslLib|CryptoPkg/Library/OpensslLib/OpensslLib.inf
  IntrinsicLib|CryptoPkg/Library/IntrinsicLib/IntrinsicLib.inf

[LibraryClasses.IA32, LibraryClasses.X64, LibraryClasses.AARCH64]
  RngLib|MdePkg/Library/BaseRngLib/BaseRngLib.inf

[LibraryClasses.AARCH64]
  ArmLib|MdePkg/Library/ArmLib/ArmBaseLib.inf

[LibraryClasses.common]
  BaseCryptLib|CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf
  TlsLib|CryptoPkg/Library/TlsLib/TlsLib.inf

#end crypto related libs



[PcdsFixedAtBuild]
!if $(OOPETRIS_RUNTIME_TARGET) == "hardware"
# nothing
!else
#define DEBUG_WARN      0x00000002       // Warnings
#define DEBUG_INFO      0x00000040       // Informational debug messages
#define DEBUG_VERBOSE   0x00400000       // Detailed debug messages that may
#define DEBUG_ERROR     0x80000000       // Error
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80400042

## This flag is used to control the destination port for PlatformDebugLibIoPort
# this is the same address as QEMU uses
  gUefiOvmfPkgTokenSpaceGuid.PcdDebugIoPort|0x402

#define DEBUG_PROPERTY_DEBUG_ASSERT_ENABLED       0x01
#define DEBUG_PROPERTY_DEBUG_PRINT_ENABLED        0x02
#define DEBUG_PROPERTY_DEBUG_CODE_ENABLED         0x04
#define DEBUG_PROPERTY_CLEAR_MEMORY_ENABLED       0x08
#define DEBUG_PROPERTY_ASSERT_BREAKPOINT_ENABLED  0x10
#define DEBUG_PROPERTY_ASSERT_DEADLOOP_ENABLED    0x20

  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x3F

!endif

  # disable auto initialize, initialize manually, and if it fails, use backup non shell code backup
  gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE


# inserted components / includes after that

