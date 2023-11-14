//
// Created by 高煜焜 on 2023/11/4.
//

#ifndef QUIETFRIEND_AUDIOUTIL_H
#define QUIETFRIEND_AUDIOUTIL_H

#include <QCoreApplication>
#include <QBuffer>
#include <QMediaPlayer>
#include <QAudioFormat>
#include <QAudioDevice>
#include <QSoundEffect>
#include "QMediaDevices"
#include "QAudioSink"
class AudioPlayer : public QObject {
Q_OBJECT

public:
    AudioPlayer()= default;

    ~AudioPlayer() override ;

    auto PlayAudio(const QByteArray &source,int sample_rate,int channel_count,QAudioFormat::SampleFormat sample_format) -> void;

};

#endif //QUIETFRIEND_AUDIOUTIL_H
