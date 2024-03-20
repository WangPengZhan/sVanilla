#pragma once
#ifdef __linux__

#include "PlatformInitializer.h"

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
