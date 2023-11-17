//
// Created by 高煜焜 on 2023/11/4.
//
# include "AudioUtil.h"

AudioPlayer::AudioPlayer() : audioPlayer(nullptr) {}

AudioPlayer::~AudioPlayer() {
    if (audioPlayer) {
        [audioPlayer stop];
    }
}

void AudioPlayer::playAudio(const char *audioData, size_t dataSize) {
    NSData *data = [NSData dataWithBytes:audioData length:dataSize];
    NSError *error = nil;

    audioPlayer = [[AVAudioPlayer alloc] initWithData:data error:&error];

    if (audioPlayer) {
        [audioPlayer prepareToPlay];
        [audioPlayer play];
    } else {
        CF_LOG_ERROR("初始化音频播放器时出错: %s", [[error localizedDescription] UTF8String]);
    }
    CF_LOG_DEBUG("play tts audio");
}
void AudioPlayer::playAudio(const char *filePath) {
    NSString *path = [NSString stringWithUTF8String:filePath];
    NSURL *url = [NSURL fileURLWithPath:path];

    NSError *error = nil;
    audioPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:&error];

    if (audioPlayer) {
        [audioPlayer prepareToPlay];
        [audioPlayer play];
    } else {
        CF_LOG_ERROR("初始化音频播放器时出错: %s", [[error localizedDescription] UTF8String]);
    }
    CF_LOG_DEBUG("play local audio");
}
