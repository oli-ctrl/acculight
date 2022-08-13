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





static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup
using namespace GlobalNamespace;
using namespace UnityEngine;



float percentage, userScore;





// calculate percentage things
MAKE_HOOK_MATCH(ResultsScreenUI, &ResultsViewController::Init, void, ResultsViewController* self, LevelCompletionResults* levelCompletionResults, IReadonlyBeatmapData* transformedBeatmapData, IDifficultyBeatmap* difficultyBeatmap, bool practice, bool newHighScore){
    ResultsScreenUI(self,levelCompletionResults,transformedBeatmapData,difficultyBeatmap,practice,newHighScore);
    getLogger().info("RESULT SCREEN OPENED :D");

    userScore = levelCompletionResults->modifiedScore;
    auto maxScore = ScoreModel::ComputeMaxMultipliedScoreForBeatmap(self->transformedBeatmapData);
        getLogger().info("3");


    percentage = (userScore /maxScore) * 100;
    getLogger().info("userScorefor this song is %f", userScore);
    getLogger().info("The percentage is %f", percentage);

    UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
    };
    
    
    

MAKE_HOOK_MATCH(LightsUpdater, &LightWithIdManager::SetColorForId, void, LightWithIdManager *self, int lightId, UnityEngine::Color color){
    getLogger().info("lights");
    getLogger().info("The current light env is: %d", lightId);
if (lightId == 7){
        getLogger().info("Correct env");
    if(percentage > 90) {
        //green
        color = UnityEngine::Color(0, 1, 0, 1);
        
    }
    else if(percentage > 80) {
        //cyan
        color = UnityEngine::Color(0, 1, 1, 1);
        
    }
    else if(percentage > 70) {
        //blue
        color = UnityEngine::Color(0, 0, 1, 1);
    }
    else if(percentage > 60) {
        //magenta
        color = UnityEngine::Color(1, 0, 1, 1);
    }
    else if(percentage < 50) {
        //white
        color = UnityEngine::Color(1, 1, 1, 1);
    }
    else{
        color = UnityEngine::Color(1, 1, 1., 1);
    }
    getLogger().info("lights update");
    LightsUpdater(self, lightId, color);
    }
}

// Loads the config from disk using our modInfo, then returns it for use
// other config tools such as config-utils don't use this config, so it can be removed if those are in use
Configuration& getConfig() {
    static Configuration config(modInfo);
    return config;
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

    getLogger().info("Installing hooks...");
        INSTALL_HOOK(getLogger(),ResultsScreenUI);
        INSTALL_HOOK(getLogger(),LightsUpdater)
    getLogger().info("Installed all hooks!");
}