# Uncomment this if you're using STL in your project
# You can find more information here:
# https://developer.android.com/ndk/guides/cpp-support
APP_STL := c++_shared

APP_ABI := armeabi-v7a arm64-v8a x86 x86_64

# used SDK number
APP_PLATFORM := android-34

# support 16KB page sizes:
# see: https://developer.android.com/guide/practices/page-sizes 
APP_SUPPORT_FLEXIBLE_PAGE_SIZES := true
