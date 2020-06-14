#include "Audio.h"
#include <string>

  void Audio::Load() {
    int error = 0; // todo print to file

    soloud = new SoLoud::Soloud;
    error = soloud->init(SoLoud::Soloud::FLAGS::CLIP_ROUNDOFF, SoLoud::Soloud::BACKENDS::SDL2,
      SoLoud::Soloud::AUTO, SoLoud::Soloud::AUTO);
    if (error != SoLoud::SO_NO_ERROR)
      printf("SoLoud: %s", soloud->getErrorString(error));

    for (int i = 0; i < static_cast<int>(SOUND_ID::NUM_SOUND_ID); ++i) {
      Sample& background = samples[i];
      background.wav = new SoLoud::Wav;
      std::string path = "../files/asset/audio/" + std::to_string(i) + "_sample.wav";
      error = background.wav->load(path.c_str());
      if (error != SoLoud::SO_NO_ERROR)
        printf("SoLoud: %s", soloud->getErrorString(error));
      background.isPlaying = false;
      background.length = background.wav->getLength();
      background.rate = 0.0f;
    }

    Sample& dialogue = samples[(int)SOUND_ID::DIALOGUE_INTRO];
    dialogue.wav->setVolume(2);
  }

  void Audio::Update(float dt) {
    for (int i = 0; i < static_cast<int>(SOUND_ID::NUM_SOUND_ID); ++i) {
      if (samples[i].isPlaying) {
        samples[i].rate += dt;
        if (samples[i].rate >= samples[i].length) {
          samples[i].rate = 0.0f;
          samples[i].isPlaying = false;
        }
      }
    }
  }

  void Audio::Delete() {
    for (int i = 0; i < (int)SOUND_ID::NUM_SOUND_ID; ++i)
      delete samples[i].wav;

    soloud->deinit();
    delete soloud;
  }

  void Audio::PlayLooped(const SOUND_ID soundID) {
    Sample& sample = samples[(int)soundID];

    if (!sample.isPlaying) {
      sample.isPlaying = true;
      int handle = soloud->play(*sample.wav, 0.5f);
      soloud->setLooping(handle, true);
    }
  }

  void Audio::Stop(const SOUND_ID soundID) {
    Sample& sample = samples[(int)soundID];

    sample.isPlaying = false;
    soloud->stopAudioSource(*sample.wav);
  }

  void Audio::PlayClocked(const SOUND_ID soundID, float dt) {
    Sample& sample = samples[(int)soundID];

    soloud->playClocked(dt, *sample.wav);
  }

  void Audio::Play(const SOUND_ID soundID) {
    Sample& sample = samples[(int)soundID];

    if (!sample.isPlaying) {
      sample.isPlaying = true;
      int handle = soloud->play(*sample.wav, 0.5f);
    }
  }