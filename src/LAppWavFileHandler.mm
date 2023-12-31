﻿/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppWavFileHandler.hpp"
#include <cmath>
#include <cstdint>
#include "LAppPal.hpp"
#include "Log_util.h"
#include "AudioUtil.h"

LAppWavFileHandler::LAppWavFileHandler()
        : _pcmData(nullptr), _userTimeSeconds(0.0f), _lastRms(0.0f), _sampleOffset(0) {
    _audioPlayer = new AudioPlayer();
}

LAppWavFileHandler::~LAppWavFileHandler() {
    if (_pcmData != nullptr) {
        ReleasePcmData();
    }
    delete _audioPlayer;
}

Csm::csmBool LAppWavFileHandler::Update(Csm::csmFloat32 deltaTimeSeconds) {
    Csm::csmUint32 goalOffset;
    Csm::csmFloat32 rms;
    // 如果在数据加载之前或者到达文件末尾时，不进行更新。
    if ((_pcmData == nullptr)
        || (_sampleOffset >= _wavFileInfo._samplesPerChannel)) {
        _lastRms = 0.0f;
        return false;
    }

    // 保持经过时间后的状态
    _userTimeSeconds += deltaTimeSeconds;
    goalOffset = static_cast<Csm::csmUint32>(_userTimeSeconds * _wavFileInfo._samplingRate);
    if (goalOffset > _wavFileInfo._samplesPerChannel) {
        goalOffset = _wavFileInfo._samplesPerChannel;
    }

    // RMS計測
    rms = 0.0f;
    for (Csm::csmUint32 channelCount = 0; channelCount < _wavFileInfo._numberOfChannels; channelCount++) {
        for (Csm::csmUint32 sampleCount = _sampleOffset; sampleCount < goalOffset; sampleCount++) {
            Csm::csmFloat32 pcm = _pcmData[channelCount][sampleCount];
            rms += pcm * pcm;
        }
    }
    rms = sqrt(rms / (_wavFileInfo._numberOfChannels * (goalOffset - _sampleOffset)));

    _lastRms = rms;
    _sampleOffset = goalOffset;
    CF_LOG_DEBUG("RMS: %f", rms);
    return true;
}

void LAppWavFileHandler::Start(const Csm::csmString &filePath) {

    LoadWavFile(filePath);
    // 初始化样例参照位置
    _sampleOffset = 0;
    _userTimeSeconds = 0.0f;

    // RMS値をリセット
    _lastRms = 0.0f;
    _audioPlayer->playAudio(filePath.GetRawString());
}

Csm::csmFloat32 LAppWavFileHandler::GetRms() const {
    return _lastRms;
}

Csm::csmBool LAppWavFileHandler::LoadWavFile(const Csm::csmString &filePath, std::shared_ptr<QByteArray> sound) {
    Csm::csmBool ret;

    // 如果已经加载了wav文件，则释放区域。
    if (_pcmData != nullptr) {
        ReleasePcmData();
    }

    // 加载文件
    if (sound != nullptr) {
        _byteReader._fileSize = sound->size();
        _byteReader._fileByte = reinterpret_cast<Csm::csmByte *>(sound->data());
    } else {
        _byteReader._fileByte = LAppPal::LoadFileAsBytes(filePath.GetRawString(), &(_byteReader._fileSize));
    }
    _byteReader._readOffset = 0;

    // 如果文件加载失败或者没有给定以"RIFF"开头的大小，则失败。
    if ((_byteReader._fileByte == nullptr) || (_byteReader._fileSize < 4)) {
        return false;
    }

    // ファイル名
    _wavFileInfo._fileName = filePath;

    do {
        // 签名 "RIFF"
        if (!_byteReader.GetCheckSignature("RIFF")) {
            ret = false;
            break;
        }
        // 文件大小-8（跳过阅读）
        _byteReader.Get32LittleEndian();
        // "WAVE" 签名
        if (!_byteReader.GetCheckSignature("WAVE")) {
            ret = false;
            break;
        }
        // シグネチャ "fmt "
        if (!_byteReader.GetCheckSignature("fmt ")) {
            ret = false;
            break;
        }
        // fmt块大小
        const Csm::csmUint32 fmtChunkSize = _byteReader.Get32LittleEndian();
//         只接受格式ID为1（线性PCM）和3（IEEE浮点数）的文件。
        _typeID = _byteReader.Get16LittleEndian();
        if (_typeID != 1 && _typeID != 3) {
            ret = false;
            CF_LOG_ERROR("Unsupported wav file type ID : %d", _typeID);
            break;
        }
        // 频道数量
        _wavFileInfo._numberOfChannels = _byteReader.Get16LittleEndian();
        // 采样率
        _wavFileInfo._samplingRate = _byteReader.Get32LittleEndian();
        // 数据速度[字节/秒]（跳过读取）
        _byteReader.Get32LittleEndian();
        // 块大小（跳过）
        _byteReader.Get16LittleEndian();
        // 量子化位数
        _wavFileInfo._bitsPerSample = _byteReader.Get16LittleEndian();
        // 忽略fmt块的扩展部分。
        if (fmtChunkSize > 16) {
            _byteReader._readOffset += (fmtChunkSize - 16);
        }
        // 跳过读取直到出现"data"块。
        while (!(_byteReader.GetCheckSignature("data"))
               && (_byteReader._readOffset < _byteReader._fileSize)) {
            _byteReader._readOffset += _byteReader.Get32LittleEndian();
        }
        // 在文件中没有出现"data"块。
        if (_byteReader._readOffset >= _byteReader._fileSize) {
            ret = false;
            break;
        }
        _wavFileInfo._dataOffset = _byteReader._readOffset;
        // 样本数量
        {
            const Csm::csmUint32 dataChunkSize = _byteReader.Get32LittleEndian();
            _wavFileInfo._samplesPerChannel =
                    (dataChunkSize * 8) / (_wavFileInfo._bitsPerSample * _wavFileInfo._numberOfChannels);
        }
        // 领域保障
        _pcmData = static_cast<Csm::csmFloat32 **>(CSM_MALLOC(
                sizeof(Csm::csmFloat32 *) * _wavFileInfo._numberOfChannels));
        for (Csm::csmUint32 channelCount = 0; channelCount < _wavFileInfo._numberOfChannels; channelCount++) {
            _pcmData[channelCount] = static_cast<Csm::csmFloat32 *>(CSM_MALLOC(
                    sizeof(Csm::csmFloat32) * _wavFileInfo._samplesPerChannel));
        }
        // 波形数据获取
        for (Csm::csmUint32 sampleCount = 0; sampleCount < _wavFileInfo._samplesPerChannel; sampleCount++) {
            for (Csm::csmUint32 channelCount = 0; channelCount < _wavFileInfo._numberOfChannels; channelCount++) {
                _pcmData[channelCount][sampleCount] = GetPcmSample();
            }
        }

        ret = true;

    } while (false);

    // 文件开放
    if (sound == nullptr) {
        LAppPal::ReleaseBytes(_byteReader._fileByte);
    }
    _byteReader._fileByte = nullptr;
    _byteReader._fileSize = 0;
    CF_LOG_DEBUG("波形数据获取成功");
    return ret;
}

Csm::csmFloat32 LAppWavFileHandler::GetPcmSample() {
    Csm::csmInt32 pcm32;
    float num = 0;
    // 32ビット幅に拡張してから-1～1の範囲に丸める
    switch (_wavFileInfo._bitsPerSample) {
        case 8:
            pcm32 = static_cast<Csm::csmInt32>(_byteReader.Get8()) - 128;
            pcm32 <<= 24;
            break;
        case 16:
            pcm32 = _byteReader.Get16LittleEndian() << 16;
            break;
        case 24:
            pcm32 = _byteReader.Get24LittleEndian() << 8;
            break;
        default:
            // unsupported bit width
            pcm32 = 0;
            break;
    }
    // 这里我企图修改代码使得格式ID为3（IEEE浮点数）的文件也能被计算rms从而同步嘴形，成功了。
    if (_typeID == 3 && _wavFileInfo._bitsPerSample == 32) {
        pcm32 = _byteReader.Get32LittleEndian();
        num = *reinterpret_cast<float *>(&pcm32); // 要点在于强制转化为32位浮点数而不是静态转化
        return static_cast<Csm::csmFloat32>(num);

    }
    return static_cast<Csm::csmFloat32>(pcm32) / INT32_MAX;
}

void LAppWavFileHandler::ReleasePcmData() {
    for (Csm::csmUint32 channelCount = 0; channelCount < _wavFileInfo._numberOfChannels; channelCount++) {
        CSM_FREE(_pcmData[channelCount]);
    }
    CSM_FREE(_pcmData);
    _pcmData = nullptr;
}

void LAppWavFileHandler::Start(std::shared_ptr<QByteArray> &sound) {
    if (!LoadWavFile("", sound)) {
        CF_LOG_ERROR("LoadWavFile failed, no sound data");
        return;
    }
    _audioPlayer->playAudio((char *)sound->data(),sound->size());

}
