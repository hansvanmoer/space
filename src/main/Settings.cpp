#include "Settings.h"
#include "JSONReader.h"
#include "JSONWriter.h"
#include "Path.h"
#include "Data.h"
#include <fstream>

using namespace Game;
using Core::Path;
using Core::PathException;


using Document = JSON::Document<JSON::BufferedInput<> >;
using Object = typename Document::Object;

using Writer = JSON::PrettyWriter<>;

SettingsException::SettingsException(std::string message) : std::runtime_error(message){
}

VideoSettings::VideoSettings() : antialiasingLevel(4),framesPerSecond(60) {
}

ControlSettings::ControlSettings() : zoomSpeed(1.0){}


AudioSettings::AudioSettings() : masterVolume(1.f), ambientVolume(1.f),effectVolume(1.f), uiVolume(1.f){};

WindowSettings::WindowSettings() : windowSize{800,600}, fullScreen(false){};

ApplicationSettings::ApplicationSettings() : videoSettings(), audioSettings(), windowSettings(){};

void validateVideoSettings(const VideoSettings &settings){
    if(settings.antialiasingLevel < 0 || settings.antialiasingLevel > 4){
        throw SettingsException("antialiasing level should be set in the range of 0 - 4");
    }
    if(settings.framesPerSecond < 10 || settings.framesPerSecond > 100){
        throw SettingsException("frames per second should be set in the range of 10 - 100");
    }
}

void validateWindowSettings(const WindowSettings &settings){
    if(settings.windowSize.width < 0){
        throw SettingsException("window width should be > 0");
    }
    if(settings.windowSize.height < 0){
        throw SettingsException("window height should be > 0");
    }
}

void readVideoSettings(Object node, VideoSettings &settings){
    settings.antialiasingLevel=static_cast<int>(node.getNumber("antialisingLevel"));
    settings.framesPerSecond = static_cast<int>(node.getNumber("framesPerSecond"));
};

void writeVideoSettings(Writer &writer, const VideoSettings &settings){
    writer.beginObject();
    writer.beginField("antialisingLevel").writeNumber(settings.antialiasingLevel).endField();
    writer.beginField("framesPerSecond").writeNumber(settings.framesPerSecond).endField();
    writer.endObject();
};

void validateControlSettings(const ControlSettings &settings){
    if(settings.zoomSpeed <= 0){
        throw SettingsException("window width should be > 0");
    }
}

void readControlSettings(Object node, ControlSettings &settings){
    settings.zoomSpeed=static_cast<double>(node.getNumber("zoomSpeed"));
};

void writeVideoSettings(Writer &writer, const ControlSettings &settings){
    writer.beginObject();
    writer.beginField("zoomSpeed").writeNumber(settings.zoomSpeed).endField();
    writer.endObject();
};

void validateAudioSettings(const AudioSettings &settings){
    if(settings.ambientVolume < 0.f || settings.ambientVolume > 1.f){
        throw SettingsException("ambient volume should be set in the range of 0 - 1");
    }
    if(settings.effectVolume < 0.f || settings.effectVolume > 1.f){
        throw SettingsException("effect volume should be set in the range of 0 - 1");
    }
    if(settings.masterVolume < 0.f || settings.masterVolume > 1.f){
        throw SettingsException("master volume should be set in the range of 0 - 1");
    }
    if(settings.uiVolume < 0.f || settings.uiVolume > 1.f){
        throw SettingsException("ui volume should be set in the range of 0 - 1");
    }
};

void readAudioSettings(Object node, AudioSettings &settings){
    settings.ambientVolume=static_cast<float>(node.getNumber("ambientVolume"));
    settings.effectVolume=static_cast<float>(node.getNumber("effectVolume"));
    settings.masterVolume=static_cast<float>(node.getNumber("masterVolume"));
    settings.uiVolume=static_cast<float>(node.getNumber("uiVolume"));
};

void writeAudioSettings(Writer &writer, const AudioSettings &settings){
    writer.beginObject();
    writer.beginField("ambientVolume").writeNumber(settings.ambientVolume).endField();
    writer.beginField("effectVolume").writeNumber(settings.effectVolume).endField();
    writer.beginField("masterVolume").writeNumber(settings.masterVolume).endField();
    writer.beginField("uiVolume").writeNumber(settings.uiVolume).endField();
    writer.endObject();
};

void readWindowSettings(Object node, WindowSettings &settings){
    settings.windowSize.width=static_cast<int>(node.getNumber("windowWidth"));
    settings.windowSize.height=static_cast<int>(node.getNumber("windowHeight"));
    settings.fullScreen=node.getBoolean("fullScreen");
};

void writeWindowSettings(Writer &writer, const WindowSettings &settings){
    writer.beginObject();
    writer.beginField("windowWidth").writeNumber(settings.windowSize.width).endField();
    writer.beginField("windowHeight").writeNumber(settings.windowSize.height).endField();
    writer.beginField("fullScreen").writeBoolean(settings.fullScreen).endField();
    writer.endObject();
};

void validateApplicationSettings(const ApplicationSettings &settings){
    validateAudioSettings(settings.audioSettings);
    validateVideoSettings(settings.videoSettings);
    validateWindowSettings(settings.windowSettings);
    validateControlSettings(settings.controlSettings);
}

void readApplicationSettings(Object node, ApplicationSettings &settings){
    readWindowSettings(node.getObject("windowSettings"), settings.windowSettings);
    readAudioSettings(node.getObject("audioSettings"), settings.audioSettings);
    readVideoSettings(node.getObject("videoSettings"), settings.videoSettings);
    readControlSettings(node.getObject("controlSettings"), settings.controlSettings);
};

void writeApplicationSettings(Writer &writer, const ApplicationSettings &settings){
    writer.beginField("windowSettings");
    writeWindowSettings(writer, settings.windowSettings);
    writer.endField();
    writer.beginField("audioSettings");
    writeAudioSettings(writer, settings.audioSettings);
    writer.endField();
    writer.beginField("videoSettings");
    writeVideoSettings(writer, settings.videoSettings);
    writer.endField();
    writer.beginField("controlSettings");
    writeVideoSettings(writer, settings.controlSettings);
    writer.endField();
}

Path createSettingsPath(){
    return Path{ApplicationSystem<DataSystem>::instance().runtimeDataPath(),"applicationSettings"};
}

const std::string SettingsSystem::NAME{"settings"};

SettingsSystem::SettingsSystem() : applicationSettings_(), dataMutex_(), fileMutex_(){
    
}

void SettingsSystem::load(){
    std::lock_guard<std::mutex> lock_{fileMutex_};
    try{
        Path settingsPath{createSettingsPath()};
        if(settingsPath.fileExists()){
            std::ifstream input;
            settingsPath.openFile(input);
            Document document{JSON::BufferedInput<>{input}};
            ApplicationSettings settings;
            readApplicationSettings(document.rootNode().object(), settings);
            if(input.bad()){
                throw SettingsException("file input error");
            }
            validateApplicationSettings(settings);
            input.close();
            applicationSettings(settings);
        }else{
            throw SettingsException("no settings file found");
        }
        
    }catch(JSON::JSONException &e){
        throw SettingsException(e.what());
    }catch(PathException &e){
        throw SettingsException(e.what());
    }
}

void SettingsSystem::save() const{
    std::lock_guard<std::mutex> lock_{fileMutex_};
    applicationSettings();
    ApplicationSettings settings;//{applicationSettings()};
    try{
        Path settingsPath{createSettingsPath()};
        settingsPath.createFile();
        std::ofstream output;
        settingsPath.openFile(output);
        Writer writer{output};
        writer.beginObject();
        writeApplicationSettings(writer, settings);
        writer.endObject();
        output.flush();
        if(!output.good()){
            throw SettingsException("file output error");
        }
    }catch(JSON::JSONException &e){
        throw SettingsException(e.what());
    }catch(PathException &e){
        throw SettingsException(e.what());
    }
}

ApplicationSettings SettingsSystem::defaultApplicationSettings(){
    return ApplicationSettings{};
}

void SettingsSystem::reset(){
    applicationSettings(defaultApplicationSettings());
}

void SettingsSystem::applicationSettings(const ApplicationSettings& settings){
    validateApplicationSettings(settings);
    {
        std::lock_guard<std::mutex> lock_{dataMutex_};
        applicationSettings_ = settings;
    }
}

ApplicationSettings SettingsSystem::applicationSettings() const{
    std::lock_guard<std::mutex> lock_{dataMutex_};
    return applicationSettings_;
}