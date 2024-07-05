#pragma once
#ifdef _WIN32

// clang-format off
#include "PlatformInitializer.h"
// clang-format on

class WindowInitializer : public PlatformInitializer
{
public:
    WindowInitializer();
    ~WindowInitializer();

    void initApplicationBefore() override;
    void init() override;
    void deinit() override;
};

#endif  // _WIN32
