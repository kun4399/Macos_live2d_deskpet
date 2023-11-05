//
// Created by 高煜焜 on 2023/11/4.
//
# include "AudioVolum.h"
AudioPlayer::AudioPlayer(): player_(new QMediaPlayer) {
}

auto AudioPlayer::PlayAudio(const QString &source) -> void {
    soundEffect_.setSource(QUrl::fromLocalFile(source));
    soundEffect_.setVolume(1);
    soundEffect_.play();

}