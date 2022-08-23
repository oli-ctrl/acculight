#pragma once

#include "config-utils/shared/config-utils.hpp"

DECLARE_CONFIG(MainConfig,
    CONFIG_VALUE(Mod_active, bool, "Enabled", true);
    CONFIG_VALUE(In_Menu, bool, "Enabled", true);
    CONFIG_VALUE(last_acc, float, "last accuracy", 0)
    CONFIG_VALUE(above_95, UnityEngine::Color, "above 95%", UnityEngine::Color(0.99609375,0.99609375,0.0,0.75));
    CONFIG_VALUE(above_90, UnityEngine::Color, "above 90%", UnityEngine::Color(0.4453125,0.828125,0.33984375,0.75));
    CONFIG_VALUE(above_80, UnityEngine::Color, "above 80%", UnityEngine::Color(0.4453125,0.828125,0.79296875,0.75));
    CONFIG_VALUE(above_70, UnityEngine::Color, "above 70%", UnityEngine::Color(0.29296875,0.5703125,0.99609375,0.75));
    CONFIG_VALUE(above_60, UnityEngine::Color, "above 60%", UnityEngine::Color(0.47265625,0.32421875,0.99609375,0.75));
    CONFIG_VALUE(above_50, UnityEngine::Color, "above 50%", UnityEngine::Color(0.9296875,0.5078125,0.9296875,0.75));
    CONFIG_VALUE(below_50, UnityEngine::Color, "below 50%", UnityEngine::Color(1.0, 1.0, 1.0, 0.75));





    CONFIG_INIT_FUNCTION(
        CONFIG_INIT_VALUE(Mod_active);
        CONFIG_INIT_VALUE(In_Menu);
        CONFIG_INIT_VALUE(last_acc);
        CONFIG_INIT_VALUE(above_95);
        CONFIG_INIT_VALUE(above_90);
        CONFIG_INIT_VALUE(above_80);
        CONFIG_INIT_VALUE(above_70);
        CONFIG_INIT_VALUE(above_60);
        CONFIG_INIT_VALUE(above_50);
        CONFIG_INIT_VALUE(below_50);
    )
)