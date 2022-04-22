#ifndef MUSEC_AUDIO_ENGINE_PROJECT
#define MUSEC_AUDIO_ENGINE_PROJECT

#include "audio/arrangement/TrackSequence.hpp"
#include "audio/engine/Graph.hpp"
#include "entities/CompleteTrack.hpp"
#include "audio/plugin/IPlugin.hpp"
#include "audio/track/ITrack.hpp"

#include <memory>
#include <variant>
#include <vector>

namespace Musec
{
namespace Audio
{
namespace Engine
{
class Project
{
public:
    struct CompleteTrackRef
    {
        std::shared_ptr<Musec::Audio::Track::ITrack>& track;
        double& gain;
        double& panning;
        std::vector<bool>::reference trackMute;
        std::vector<bool>::reference trackSolo;
        std::vector<bool>::reference trackInvertPhase;
        std::vector<bool>::reference trackArmRecording;
    };
public:
    Project();
    Project(const Project&) = delete;
    Project(Project&&) = default;
    ~Project() noexcept;
public:
    std::size_t trackCount() const noexcept;
    CompleteTrackRef at(std::size_t index);
    CompleteTrackRef operator[](std::size_t index);
public:
    void insertTrack(std::size_t index, const Musec::Entities::CompleteTrack& track);
    void eraseTrack(std::size_t index);
private:
    Musec::Audio::Engine::Graph<std::shared_ptr<Musec::Audio::Plugin::IPlugin<double>>> pluginGraph_;
    std::vector<std::shared_ptr<Musec::Audio::Track::ITrack>> tracks_;
    std::vector<double> gain_;
    std::vector<double> panning_;
    std::vector<bool> trackMute_;
    std::vector<bool> trackSolo_;
    std::vector<bool> trackInvertPhase_;
    std::vector<bool> trackArmRecording_;
};
}
}
}

#endif //MUSEC_AUDIO_ENGINE_PROJECT