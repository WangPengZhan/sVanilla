#ifdef __APPLE__
#pragma once

#include "PlatfromInitializer.h"

class MacInitializer : public PlatfromInitializer
{
public:
    MacInitializer();
    ~MacInitializer();

    void initApplicationBefore() override;
    void init() override;
    void deinit() override;
};

#endif  // __APPLE__
