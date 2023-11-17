//
// Created by 高煜焜 on 2023/11/4.
//

#ifndef QUIETFRIEND_AUDIOUTIL_H
#define QUIETFRIEND_AUDIOUTIL_H

#include "Log_util.h"
#import <AVFoundation/AVFoundation.h>
class AudioPlayer {

public:
    AudioPlayer();

    ~AudioPlayer();
    void playAudio(const char *filePath);

    void playAudio(const char *audioData, size_t dataSize);
private:
    AVAudioPlayer *audioPlayer;

};

#endif //QUIETFRIEND_AUDIOUTIL_H
