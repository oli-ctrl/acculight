#include "main.hpp"
#include "GlobalNamespace/ResultsViewController.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/BeatmapData.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "GlobalNamespace/MenuLightsManager.hpp"
#include "UnityEngine/Color.hpp"
#include "GlobalNamespace/ScoreModel.hpp"
#include "UnityEngine/Resources.hpp"
#include "GlobalNamespace/LightWithIdManager.hpp"
#include "GlobalNamespace/LevelSearchViewController.hpp"
#include "GlobalNamespace/GameplaySetupViewController.hpp"
#include "MainConfig.hpp"
#include "questui/shared/QuestUI.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "GlobalNamespace/LevelCollectionViewController.hpp"
#include "GlobalNamespace/LevelSearchViewController.hpp"
#include "GlobalNamespace/SongController.hpp"
#include "System/Action.hpp"
#include "GlobalNamespace/GameplayLevelSceneTransitionEvents.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "GlobalNamespace/HealthWarningViewController.hpp"


DEFINE_CONFIG(MainConfig)

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;

float percentage, userScore, preview_acc;
bool updatelights;
GlobalNamespace::MenuLightsManager *menuLightsmanager;

// results screen ui 
MAKE_HOOK_MATCH(ResultsScreenUI_didactivate, &ResultsViewController::DidActivate, void, GlobalNamespace::ResultsViewController*self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    ResultsScreenUI_didactivate( self, firstActivation, addedToHierarchy, screenSystemEnabling);
    updatelights = true;
    UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
}

// health and safety 
MAKE_HOOK_MATCH(health_and_safety, &HealthWarningViewController::DidActivate, void, GlobalNamespace::HealthWarningViewController *self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    health_and_safety( self, firstActivation, addedToHierarchy, screenSystemEnabling);
    updatelights = false;
    UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
}


// calculate percentage things
MAKE_HOOK_MATCH(ResultsScreenUI_init, &ResultsViewController::Init, void, ResultsViewController* self, LevelCompletionResults* levelCompletionResults, IReadonlyBeatmapData* transformedBeatmapData,
IDifficultyBeatmap* difficultyBeatmap, bool practice, bool newHighScore){
    ResultsScreenUI_init(self,levelCompletionResults,transformedBeatmapData,difficultyBeatmap,practice,newHighScore);
    getLogger().info("RESULT SCREEN OPENED :D");
    

    
    userScore = levelCompletionResults->modifiedScore;
    auto maxScore = ScoreModel::ComputeMaxMultipliedScoreForBeatmap(self->transformedBeatmapData);
    percentage = (userScore /maxScore) * 100;

    getMainConfig().last_acc.SetValue(percentage, true);
    getLogger().info("userScorefor this song is %f", userScore);
    getLogger().info("The percentage is %f", percentage);

    updatelights = true;
    getLogger().info("result screen song complete %d", updatelights);
    UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
    if (levelCompletionResults != 0){
        UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
    }
    };


// song select open, toggle change light false unless 
MAKE_HOOK_MATCH(Song_select, &LevelCollectionViewController::DidActivate, void , LevelCollectionViewController *self , bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling){
    Song_select( self, firstActivation, addedToHierarchy, screenSystemEnabling);
    getLogger().info("song select open");
    updatelights = true;
  
}


//leave song select
MAKE_HOOK_MATCH(Song_select_exit, &LevelCollectionViewController::DidDeactivate, void, LevelCollectionViewController *self,bool removedFromHierarchy, bool screenSystemDisabling){
    Song_select_exit( self, removedFromHierarchy, screenSystemDisabling);
    getLogger().info("leaving main menu");
    updatelights = false;
}


// main menu 
MAKE_HOOK_MATCH(Main_menu, &GlobalNamespace::MainMenuViewController::DidActivate, void, GlobalNamespace::MainMenuViewController*self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
Main_menu( self, firstActivation, addedToHierarchy, screenSystemEnabling);
getLogger().info("main menu");
    updatelights = false;
    (getMainConfig().updatelights_preview.SetValue(false));
    UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
    getLogger().info("Main menu %d", updatelights);
}



//manage the lights and change depending on accuracy 
MAKE_HOOK_MATCH(LightsUpdater, &LightWithIdManager::SetColorForId, void, LightWithIdManager *self, int lightId, UnityEngine::Color color){
    getLogger().info("lights update hook: %d", updatelights);


    if (updatelights == true){

        
        if(getMainConfig().last_acc.GetValue() > 95) {
            //purple
            color = getMainConfig().above_95.GetValue();
            
        }
        else if(getMainConfig().last_acc.GetValue() > 90) {
            //green
            color = getMainConfig().above_90.GetValue();
            
        }
        else if(getMainConfig().last_acc.GetValue() > 80) {
            //cyan
            color = getMainConfig().above_80.GetValue();
            
        }
        else if(getMainConfig().last_acc.GetValue() > 70) {
            //blue
            color = getMainConfig().above_70.GetValue();
            
        }
        else if(getMainConfig().last_acc.GetValue() > 60) {
            //magenta
            color = getMainConfig().above_60.GetValue();
        }
        else if (getMainConfig().last_acc.GetValue() >50){
            color = getMainConfig().above_50.GetValue();
        }
        else if(getMainConfig().last_acc.GetValue() < 50 and getMainConfig().last_acc.GetValue() != 0) {
                color = getMainConfig().below_50.GetValue();
            
        }
    }
        // preview the lights in the color select screen
    if ((getMainConfig().updatelights_preview.GetValue()) == true){
        getLogger().info("updating lights");



        if(preview_acc > 95) {
            //purple
            color = getMainConfig().above_95.GetValue();
            
        }
        else if(preview_acc > 90) {
            //green
            color = getMainConfig().above_90.GetValue();
            
        }
        else if(preview_acc > 80) {
            //cyan
            color = getMainConfig().above_80.GetValue();
            
        }
        else if(preview_acc > 70) {
            //blue
            color = getMainConfig().above_70.GetValue();
        }
        else if(preview_acc > 60) {
            //magenta
            color = getMainConfig().above_60.GetValue();
        }
        else if (preview_acc){
            color = getMainConfig().above_50.GetValue();
        }
        else if(preview_acc < 50) {
                color = getMainConfig().below_50.GetValue();
            
        }
    }
    LightsUpdater(self, lightId, color);
}



// Loads the config from disk using our modInfo, then returns it for use
// other config tools such as config-utils don't use this config, so it can be removed if those are in use
Configuration& getConfig() {
    static Configuration config(modInfo);
    return config;
}


// ui stuffs
void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    // Create our UI elements only when shown for the first time.
    if(firstActivation) {
        // Create a container that has a scroll bar.
        GameObject* container = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(self->get_transform());


        QuestUI::BeatSaberUI::CreateText(container->get_transform(), "change the color presets here");
        auto colorPicker95 = BeatSaberUI::CreateColorPicker (container->get_transform(), "above 95 %", getMainConfig().above_95.GetValue(),[](UnityEngine::Color color) {
            getMainConfig().above_95.SetValue(color, true);
            (getMainConfig().updatelights_preview.SetValue(true));
            preview_acc = 96;
            UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
            (getMainConfig().updatelights_preview.SetValue(false));
        });
        
        QuestUI::BeatSaberUI::CreateUIButton(colorPicker95->get_transform(), "Reset above 95%",UnityEngine::Vector2(0,+0.1),UnityEngine::Vector2(30,8),
        [colorPicker95]()
            {
                getMainConfig().above_95.SetValue(UnityEngine::Color(0.99609375,0.99609375,0.0,0.75));
                colorPicker95->set_currentColor(getMainConfig().above_95.GetValue());
                preview_acc = 0;
                UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
            });

        auto colorPicker90 = BeatSaberUI::CreateColorPicker(container->get_transform(), "above 90 %", getMainConfig().above_90.GetValue(),[](UnityEngine::Color color) {
            getMainConfig().above_90.SetValue(color, true); 
            (getMainConfig().updatelights_preview.SetValue(true));
            preview_acc = 91;
            UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
            (getMainConfig().updatelights_preview.SetValue(false));
        });

        QuestUI::BeatSaberUI::CreateUIButton(colorPicker90->get_transform(), "Reset above 90%",UnityEngine::Vector2(0,+0.1),UnityEngine::Vector2(30,8),
        [colorPicker90]()
            {
                getMainConfig().above_90.SetValue(UnityEngine::Color(0.4453125,0.828125,0.33984375,0.75));
                colorPicker90->set_currentColor(getMainConfig().above_90.GetValue());
                preview_acc = 0;
                UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
            });

        auto colorPicker80 = BeatSaberUI::CreateColorPicker(container->get_transform(), "above 80 %", getMainConfig().above_80.GetValue(),[](UnityEngine::Color color) {
            getMainConfig().above_80.SetValue(color, true); 
            (getMainConfig().updatelights_preview.SetValue(true));
            preview_acc = 81;
            UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
            (getMainConfig().updatelights_preview.SetValue(false));
        });

        QuestUI::BeatSaberUI::CreateUIButton(colorPicker80->get_transform(), "Reset above 80%",UnityEngine::Vector2(0,+0.1),UnityEngine::Vector2(30,8),
        [colorPicker80]()
            {
                getMainConfig().above_80.SetValue(UnityEngine::Color(0.4453125,0.828125,0.79296875,0.75));
                colorPicker80->set_currentColor(getMainConfig().above_80.GetValue());
                preview_acc = 0;
                UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
            });

        auto colorPicker70 = BeatSaberUI::CreateColorPicker(container->get_transform(), "above 70 %", getMainConfig().above_70.GetValue(),[](UnityEngine::Color color) {
            getMainConfig().above_70.SetValue(color, true);
            (getMainConfig().updatelights_preview.SetValue(true));
            preview_acc = 71;
            UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
            (getMainConfig().updatelights_preview.SetValue(false));
        });

        QuestUI::BeatSaberUI::CreateUIButton(colorPicker70->get_transform(), "Reset above 70%",UnityEngine::Vector2(0,+0.1),UnityEngine::Vector2(30,8),
        [colorPicker70]()
            {
                getMainConfig().above_70.SetValue(UnityEngine::Color(0.29296875,0.5703125,0.99609375,0.75));
                colorPicker70->set_currentColor(getMainConfig().above_70.GetValue());
                preview_acc = 0;
                UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
            });
        auto colorPicker60 = BeatSaberUI::CreateColorPicker(container->get_transform(), "above 60 %", getMainConfig().above_60.GetValue(),[](UnityEngine::Color color) {
            getMainConfig().above_60.SetValue(color, true); 
            (getMainConfig().updatelights_preview.SetValue(true)); 
            preview_acc = 61;
            UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
            (getMainConfig().updatelights_preview.SetValue(false));
        });

        QuestUI::BeatSaberUI::CreateUIButton(colorPicker60->get_transform(), "Reset above 60%",UnityEngine::Vector2(0,+0.1),UnityEngine::Vector2(30,8),
        [colorPicker60]()
            {
                getMainConfig().above_60.SetValue(UnityEngine::Color(0.47265625,0.32421875,0.99609375,0.75));
                colorPicker60->set_currentColor(getMainConfig().above_60.GetValue());
                preview_acc = 0;
                UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
            });

        auto colorPicker50 = BeatSaberUI::CreateColorPicker(container->get_transform(), "above 50 %", getMainConfig().above_50.GetValue(),[](UnityEngine::Color color) {
            getMainConfig().above_50.SetValue(color, true);
            (getMainConfig().updatelights_preview.SetValue(true));
            preview_acc = 51;
            UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
            (getMainConfig().updatelights_preview.SetValue(false));
        });

        QuestUI::BeatSaberUI::CreateUIButton(colorPicker50->get_transform(), "Reset above 50%",UnityEngine::Vector2(0,+0.1),UnityEngine::Vector2(30,8),
        [colorPicker50]()
            {
                preview_acc = 51;
                getMainConfig().above_50.SetValue(UnityEngine::Color(0.9296875,0.5078125,0.9296875,0.75));
                colorPicker50->set_currentColor(getMainConfig().above_50.GetValue());
                UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
            });

        auto colorPicker_below_50 = BeatSaberUI::CreateColorPicker(container->get_transform(), "below 50 %", getMainConfig().below_50.GetValue(),[](UnityEngine::Color color) {
            getMainConfig().below_50.SetValue(color, true);
            preview_acc = 0;
            (getMainConfig().updatelights_preview.SetValue(true)); 
            UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
            (getMainConfig().updatelights_preview.SetValue(false));
        });

        QuestUI::BeatSaberUI::CreateUIButton(colorPicker_below_50->get_transform(), "Reset below 50%",UnityEngine::Vector2(0,+0.1),UnityEngine::Vector2(30,8),
        [colorPicker_below_50]()
            {
                getMainConfig().below_50.SetValue(UnityEngine::Color(1.0, 1.0, 1.0, 0.75));
                colorPicker_below_50->set_currentColor(getMainConfig().below_50.GetValue());
                UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
                preview_acc = 0;
            });


        QuestUI::BeatSaberUI::CreateUIButton(container->get_transform(), "Reset all colors",
        [colorPicker50,colorPicker60,colorPicker70,colorPicker80,colorPicker90,colorPicker95, colorPicker_below_50](){
            // reset colors
            getMainConfig().above_95.SetValue(UnityEngine::Color(0.99609375,0.99609375,0.0,0.75));
            getMainConfig().above_90.SetValue(UnityEngine::Color(0.4453125,0.828125,0.33984375,0.75));
            getMainConfig().above_80.SetValue(UnityEngine::Color(0.4453125,0.828125,0.79296875,0.75));
            getMainConfig().above_70.SetValue(UnityEngine::Color(0.29296875,0.5703125,0.99609375,0.75));
            getMainConfig().above_60.SetValue(UnityEngine::Color(0.47265625,0.32421875,0.99609375,0.75));
            getMainConfig().above_50.SetValue(UnityEngine::Color(0.9296875,0.5078125,0.9296875,0.75));
            getMainConfig().below_50.SetValue(UnityEngine::Color(1.0, 1.0, 1.0, 0.75));
            // reload the color pickers
            colorPicker95->set_currentColor(getMainConfig().above_95.GetValue());
            colorPicker90->set_currentColor(getMainConfig().above_90.GetValue());
            colorPicker80->set_currentColor(getMainConfig().above_80.GetValue());
            colorPicker70->set_currentColor(getMainConfig().above_70.GetValue());
            colorPicker60->set_currentColor(getMainConfig().above_60.GetValue());
            colorPicker50->set_currentColor(getMainConfig().above_50.GetValue());
            colorPicker_below_50->set_currentColor(getMainConfig().below_50.GetValue());
            preview_acc = 0;
            // reload lights
            UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
        });
    }
}       

void DidDeactivate(HMUI::ViewController *self ,bool removedFromHierarchy, bool screenSystemDisabling) {
    (getMainConfig().updatelights_preview.SetValue(false));
    UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
}
// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;
	

    
    getConfig().Load();
    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {            
    il2cpp_functions::Init();
    getMainConfig().Init(modInfo);

    QuestUI::Init();
    QuestUI::Register::RegisterModSettingsViewController(modInfo, DidActivate);
    QuestUI::Register::RegisterMainMenuModSettingsViewController(modInfo, DidActivate);

    getLogger().info("Installing hooks...");
        INSTALL_HOOK(getLogger(),Main_menu)
        INSTALL_HOOK(getLogger(),Song_select)
        INSTALL_HOOK(getLogger(),ResultsScreenUI_init);
        INSTALL_HOOK(getLogger(),LightsUpdater);
        INSTALL_HOOK(getLogger(),Song_select_exit);
        INSTALL_HOOK(getLogger(),ResultsScreenUI_didactivate);
    getLogger().info("Installed all hooks!");
}
