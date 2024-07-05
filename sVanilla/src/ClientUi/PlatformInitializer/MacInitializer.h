#pragma once
#ifdef __APPLE__

// clang-format off
#include "PlatformInitializer.h"
// clang-format on

class MacInitializer : public PlatformInitializer
{
public:
    MacInitializer();
    ~MacInitializer();

    void initApplicationBefore() override;
    void init() override;
    void deinit() override;
};

#endif  // __APPLE__
