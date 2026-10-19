#pragma once

#include <memory>
#include <stdexcept>

#include "LoginProxy.h"

namespace clientui::login
{
inline std::shared_ptr<LoginProxy> makeSharedLoginProxy(const LoginProxy& loginer)
{
    AbstractLogin& realLogin = loginer.realLogin();
    if (auto* apiLogin = dynamic_cast<AbstractLoginApi*>(&realLogin))
    {
        return std::make_shared<LoginProxy>(*apiLogin);
    }
    if (auto* webLogin = dynamic_cast<AbstractLoginWeb*>(&realLogin))
    {
        return std::make_shared<LoginProxy>(*webLogin);
    }

    throw std::logic_error("Unsupported login implementation type");
}
}  // namespace clientui::login
