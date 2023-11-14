//
// Created by 高煜焜 on 2023/11/4.
//
# include "AudioUtil.h"



auto AudioPlayer::PlayAudio(const QByteArray &source, int sample_rate, int channel_count,
                            QAudioFormat::SampleFormat sample_format) -> void {
    qDebug() << "AudioPlayer::PlayAudio : start";
    QAudioFormat format;
    format.setSampleRate(sample_rate);
    format.setChannelCount(channel_count);
    format.setSampleFormat(sample_format);
    auto audio = new QAudioSink(format, this);
    auto *buffer = new QBuffer(audio);
    buffer->setData(source);
    buffer->open(QIODevice::ReadOnly);
    audio->start(buffer);
}

AudioPlayer::~AudioPlayer() {

}
