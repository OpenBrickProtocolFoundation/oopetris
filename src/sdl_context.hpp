#pragma once

struct SdlContext final {
    SdlContext();
    SdlContext(const SdlContext&) = delete;
    SdlContext(SdlContext&&) = delete;
    SdlContext& operator=(const SdlContext&) = delete;
    SdlContext& operator=(SdlContext&&) = delete;
    ~SdlContext();
};
