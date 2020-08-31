#ifndef AUDIO_H
#define AUDIO_H

#include <stdio.h>
#include <queue>
#include <utility>
#include "soloud\include\soloud.h"
#include "soloud\include\soloud_wav.h"

enum class SOUND_ID { BACKGROUND, DIALOGUE_INTRO, ITEM_PICKUP, TEST, NUM_SOUND_ID};

class Audio {
public:
  void Load();

  /*
  Loops through the samples and check whether they should still be playing.
  */
  void Update(float dt = 0.0f);

  void Delete();
  void PlayLooped(const SOUND_ID soundID);
  void Stop(const SOUND_ID soundID);

  /*
  Play delayed based on other calls to this function. Prevents sounds getting clumped together.
  */
  void PlayClocked(const SOUND_ID soundID, float dt);

  void Play(const SOUND_ID soundID);

private:
  struct Sample {
    SoLoud::Wav* wav;
    bool isPlaying;
    double length;
    float rate;
  };
  Sample samples[(int)SOUND_ID::NUM_SOUND_ID];

  SoLoud::Soloud* soloud;
};
#endif // !AUDIO_H
