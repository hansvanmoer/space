/* 
 * File:   ApplicationSettings.h
 * Author: hans
 *
 * Created on 17 January 2015, 17:11
 */

#ifndef CORE_SETTINGS_H
#define	CORE_SETTINGS_H

#include "Metrics.h"

#include <string>
#include <stdexcept>

namespace Game{
    
    struct VideoSettings{
        int antialiasingLevel;
    };
    
    struct WindowSettings{
        Core::Size<int> windowSize;
        bool fullScreen;
    };
    
    struct AudioSettings{
        float masterVolume;
        float ambientVolume;
        float effectVolume;
        float uiVolume;
    };
    
    struct ApplicationSettings{
        VideoSettings videoSettings;
        WindowSettings windowSettings;
        AudioSettings audioSettings;
    };
    
    class SettingsException : public std::runtime_error{
    public:
        SettingsException(std::string message);
    };
    
    ApplicationSettings loadSettings();
    
    void loadSettings(ApplicationSettings &setting);
    
    void saveSettings(const ApplicationSettings &settings);
    
}

#endif	/* APPLICATIONSETTINGS_H */

