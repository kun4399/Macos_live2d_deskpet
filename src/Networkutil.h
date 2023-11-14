//
// Created by 高煜焜 on 2023/11/5.
//

#ifndef QUIETFRIEND_NETWORKUTIL_H
#define QUIETFRIEND_NETWORKUTIL_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtCore/qurlquery.h>
#include <QUrl>
#include <QObject>
#include <QByteArray>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "ChatDialog.h"
#include <CubismFramework.hpp>
#include "resource_loader.hpp"
#include "LAppLive2DManager.hpp"

#define SystemPrompt "你的回复要包括三个参数：expression（无可选值），motion（可选 Idle），message,需要json格式回复,无有特殊符号。简洁的回答"
#define VoiceAPI "https://genshinvoice.top/api"
#define GPT_MESSAGE "message"
#define GPT_MOTION "motion"
#define GPT_EXPRESSION "expression"
#define MAX_TOKENS 4096
class ChatDialog;

class NetworkManager : public QObject {
public:
    explicit NetworkManager(QObject *parent = nullptr);

    ~NetworkManager() override;

    void SendRequest(const QString &user_message);

    void SendVoiceRequest();

    void ClearMessages();

    bool ReplyMessageProcess(const QByteArray &reply_data);


private slots:

    bool handleNetworkReply(QNetworkReply *reply);

private:
    ChatDialog *chat_dialog_;
    QNetworkAccessManager *network_manager_;
    QJsonArray *request_messages_;
    QNetworkRequest *gpt_request_;
    QNetworkRequest *voice_request_;
    int total_tokens_{0};
    Csm::csmString motion_;
    Csm::csmString expression_;
    QByteArray voice_;
    QString robot_message_;
};


#endif //QUIETFRIEND_NETWORKUTIL_H
