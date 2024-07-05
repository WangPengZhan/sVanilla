#pragma once
#ifdef __linux__

// clang-format off
#include "PlatformInitializer.h"
// clang-format on

class LinuxInitializer : public PlatformInitializer
{
public:
    LinuxInitializer();
    ~LinuxInitializer();

    void initApplicationBefore() override;
    void init() override;
    void deinit() override;
};

#endif  // __linux__
