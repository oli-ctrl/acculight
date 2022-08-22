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

DEFINE_CONFIG(MainConfig)

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup
using namespace GlobalNamespace;
using namespace UnityEngine;


float percentage, userScore;
bool updatelights;
GlobalNamespace::MenuLightsManager *menuLightsmanager;


// calculate percentage things
MAKE_HOOK_MATCH(ResultsScreenUI, &ResultsViewController::Init, void, ResultsViewController* self, LevelCompletionResults* levelCompletionResults, IReadonlyBeatmapData* transformedBeatmapData, IDifficultyBeatmap* difficultyBeatmap, bool practice, bool newHighScore){
    ResultsScreenUI(self,levelCompletionResults,transformedBeatmapData,difficultyBeatmap,practice,newHighScore);
    getLogger().info("RESULT SCREEN OPENED :D");
    updatelights = true;

    
    userScore = levelCompletionResults->modifiedScore;
    auto maxScore = ScoreModel::ComputeMaxMultipliedScoreForBeatmap(self->transformedBeatmapData);
    percentage = (userScore /maxScore) * 100;
    getLogger().info("userScorefor this song is %f", userScore);
    getLogger().info("The percentage is %f", percentage);
    menuLightsmanager = UnityEngine::Object::FindObjectOfType<GlobalNamespace::MenuLightsManager*>();

    if (getMainConfig().Mod_active.GetValue()){
    getLogger().info("lights update hook: %d", updatelights);
            getLogger().info("updating lights");



        if(percentage > 95) {
            //purple
            menuLightsmanager-> SetColor(1, UnityEngine::Color(0.64453125,0.06640625,0.99609375,0.75));
            
        }
        else if(percentage > 90) {
            //green
            menuLightsmanager-> SetColor(1, UnityEngine::Color(0.4453125,0.828125,0.33984375,0.75));
            
        }
        else if(percentage > 80) {
            //cyan
            menuLightsmanager-> SetColor(1, UnityEngine::Color(0.4453125,0.828125,0.79296875,0.75));
            
        }
        else if(percentage > 70) {
            //blue
            menuLightsmanager-> SetColor(1, UnityEngine::Color(0.29296875,0.5703125,0.99609375,0.75));
        }
        else if(percentage > 60) {
            //magenta
            menuLightsmanager-> SetColor(1, UnityEngine::Color(0.8984375,0.4140625,0.8203125,0.75));
        }
        else if(percentage < 50) {
            //white
            menuLightsmanager-> SetColor(1, UnityEngine::Color(1, 1, 1, 0.75));
        }
        }

    getLogger().info("result screen song complete %d", updatelights);

    };



// song select open, toggle change light false unless 
MAKE_HOOK_MATCH(Song_select, &LevelCollectionViewController::DidActivate, void , LevelCollectionViewController *self , bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling){
    Song_select( self, firstActivation, addedToHierarchy, screenSystemEnabling);
    
getLogger().info("song select open");
 if (getMainConfig().In_Menu.GetValue() == false){
        updatelights = false;
        getLogger().info("false - song select song complete%d", updatelights);
        }
 else if (getMainConfig().In_Menu.GetValue()){
        updatelights = true;
        getLogger().info("true - song select song complete%d", updatelights);
        }    
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
     getLogger().info("Main menu %d", updatelights);
}



//manage the lights and change depending on accuracy 
MAKE_HOOK_MATCH(LightsUpdater, &LightWithIdManager::SetColorForId, void, LightWithIdManager *self, int lightId, UnityEngine::Color color){
if (getMainConfig().Mod_active.GetValue()){
     getLogger().info("lights update hook: %d", updatelights);
    if (updatelights == true){
            getLogger().info("updating lights");



        if(percentage > 95) {
            //purple
            color = UnityEngine::Color(0.64453125,0.06640625,0.99609375,0.75);
            
        }
        else if(percentage > 90) {
            //green
            color = UnityEngine::Color(0.4453125,0.828125,0.33984375,0.75);
            
        }
        else if(percentage > 80) {
            //cyan
            color = UnityEngine::Color(0.4453125,0.828125,0.79296875,0.75);
            
        }
        else if(percentage > 70) {
            //blue
            color = UnityEngine::Color(0.29296875,0.5703125,0.99609375,0.75);
        }
        else if(percentage > 60) {
            //magenta
            color = UnityEngine::Color(0.8984375,0.4140625,0.8203125,0.75);
        }
        else if(percentage < 50) {
            //white
            color = UnityEngine::Color(1, 1, 1, 0.75);
        }
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
       
        QuestUI::BeatSaberUI::CreateText(container->get_transform(), "The mod config, change these settings or completely turn off the mod!");

        // the mod active toggle
        QuestUI::BeatSaberUI::CreateToggle(container->get_transform(), "Mod Enabled", getMainConfig().Mod_active.GetValue(), [](bool value) {
		getMainConfig().Mod_active.SetValue(value, true);
        });
        // the enable in song select toggle
        QuestUI::BeatSaberUI::CreateToggle(container->get_transform(), "Enable in song select", getMainConfig().In_Menu.GetValue(), [](bool value) {
		getMainConfig().In_Menu.SetValue(value, true);
        });

    }
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
        INSTALL_HOOK(getLogger(),ResultsScreenUI);
        INSTALL_HOOK(getLogger(),LightsUpdater);
        INSTALL_HOOK(getLogger(),Song_select_exit);
    getLogger().info("Installed all hooks!");
}