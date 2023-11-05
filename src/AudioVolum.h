//
// Created by 高煜焜 on 2023/11/4.
//

#ifndef QUIETFRIEND_AUDIOVOLUM_H
#define QUIETFRIEND_AUDIOVOLUM_H

#include <QMediaPlayer>
#include <QAudioFormat>
#include <QAudioInput>
#include <QIODevice>
#include <QSoundEffect>

class AudioPlayer : public QObject {
Q_OBJECT

public:
    AudioPlayer();

    ~AudioPlayer() override = default;

    auto PlayAudio(const QString &source) -> void;

    auto GetVolume() -> float;

private:
    QMediaPlayer * player_;
    QAudioInput *input_;
    QAudioFormat format_;
    QIODevice *m_device;
    QSoundEffect soundEffect_;
};

#endif //QUIETFRIEND_AUDIOVOLUM_H
