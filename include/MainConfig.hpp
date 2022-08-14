#pragma once

#include "config-utils/shared/config-utils.hpp"

DECLARE_CONFIG(MainConfig,
    CONFIG_VALUE(Mod_active, bool, "Enabled", true);
    CONFIG_VALUE(In_Menu, bool, "Enabled", false);

    

    CONFIG_INIT_FUNCTION(
        CONFIG_INIT_VALUE(Mod_active);
        CONFIG_INIT_VALUE(In_Menu);

    )
)