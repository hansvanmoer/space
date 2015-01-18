#include "Settings.h"
#include "JSONReader.h"
#include "JSONWriter.h"
#include <fstream>
using namespace Game;

using Document = JSON::Document<JSON::BufferedInput<> >;
using Object = typename Document::Object;

using Writer = JSON::MinifiedWriter<>;

SettingsException::SettingsException(std::string message) : std::runtime_error(message){}


void validateVideoSettings(const VideoSettings &settings){
    if(settings.antialiasingLevel < 0 || settings.antialiasingLevel > 4){
        throw SettingsException("antialiasing level should be set in the range of 0 - 4");
    }
}

void readVideoSettings(Object node, VideoSettings &settings){
    settings.antialiasingLevel=static_cast<int>(node.getNumber("antialisingLevel"));
};

void writeVideoSettings(Writer &writer, const VideoSettings &settings){
    writer.beginField("antialisingLevel").writeNumber(settings.antialiasingLevel).endField();
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
    writer.beginField("ambientVolume").writeNumber(settings.ambientVolume).endField();
    writer.beginField("effectVolume").writeNumber(settings.effectVolume).endField();
    writer.beginField("masterVolume").writeNumber(settings.masterVolume).endField();
    writer.beginField("uiVolume").writeNumber(settings.uiVolume).endField();
};