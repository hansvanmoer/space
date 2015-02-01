/* 
 * File:   ApplicationSettings.h
 * Author: hans
 *
 * Created on 17 January 2015, 17:11
 */

#ifndef CORE_SETTINGS_H
#define	CORE_SETTINGS_H

#include "Metrics.h"
#include "Application.h"

#include <string>
#include <stdexcept>
#include <mutex>

namespace Game{
    
    struct VideoSettings{
        int antialiasingLevel;
        int framesPerSecond;
        
        VideoSettings();
    };
    
    struct WindowSettings{
        Core::Size<int> windowSize;
        bool fullScreen;
        
        WindowSettings();
    };
    
    struct ControlSettings{
        double zoomSpeed;
        double mouseScrollSpeed;
        double keyScrollSpeed;
        
        ControlSettings();
    };
    
    struct AudioSettings{
        float masterVolume;
        float ambientVolume;
        float effectVolume;
        float uiVolume;
        
        AudioSettings();
    };
    
    struct ApplicationSettings{
        VideoSettings videoSettings;
        WindowSettings windowSettings;
        AudioSettings audioSettings;
        ControlSettings controlSettings;
        
        ApplicationSettings();
    };
    
    class SettingsException : public std::runtime_error{
    public:
        SettingsException(std::string message);
    };
        
    class SettingsSystem{
    public:
        
        static const std::string NAME;
        
        static ApplicationSettings defaultApplicationSettings();
        
        ApplicationSettings applicationSettings() const;
        
        void applicationSettings(const ApplicationSettings &settings);
        
        void load();
        
        void save() const;
        
        void reset();
    private:
        ApplicationSettings applicationSettings_;
        mutable std::mutex dataMutex_;
        mutable std::mutex fileMutex_;
        
        friend class ApplicationSystem<SettingsSystem>;
        
        SettingsSystem();
    };
    
}

#endif	/* APPLICATIONSETTINGS_H */

