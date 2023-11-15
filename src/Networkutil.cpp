//
// Created by 高煜焜 on 2023/11/5.
//

#include "Networkutil.h"
#include "Log_util.h"

NetworkManager::NetworkManager(QObject *parent) : QObject(parent) {

    chat_dialog_ = (ChatDialog *) parent;
    network_manager_ = new QNetworkAccessManager(this);
    gpt_request_ = new QNetworkRequest;
    voice_request_ = new QNetworkRequest;
    gpt_request_->setUrl(QUrl(resource_loader::get_instance().get_gpt_url()));
    gpt_request_->setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    gpt_request_->setRawHeader("api-key", resource_loader::get_instance().get_gpt_key().toUtf8());
    voice_request_->setHeader(QNetworkRequest::ContentTypeHeader, "audio/wav");
    request_messages_ = new QJsonArray;
    QJsonObject request_message;
    request_message.insert("role", "system");
    request_message.insert("content", resource_loader::get_instance().get_gpt_system_prompt());
    request_messages_->append(request_message);
    connect(network_manager_, &QNetworkAccessManager::finished, this, &NetworkManager::handleNetworkReply);


}

void NetworkManager::SendRequest(const QString &user_message) {
    if (user_message.isEmpty()) {
        return;
    }
    if (total_tokens_ +  user_message.length() > MAX_TOKENS) {
        ClearMessages();
    }
    QJsonObject request_message;
    request_message.insert("role", "user");
    request_message.insert("content", user_message);
    request_messages_->append(request_message);
    QJsonObject request;
    request.insert("messages", *request_messages_);
    CF_LOG_DEBUG("request: %s", QJsonDocument(request).toJson().data());
    network_manager_->post(*gpt_request_, QJsonDocument(request).toJson());
}

bool NetworkManager::handleNetworkReply(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
//        qDebug() << "请求失败 Url:" << reply->url() << reply->errorString();
        CF_LOG_ERROR("请求失败 Url: %s, %s", reply->url().toString().toUtf8().data(), reply->errorString().toUtf8().data());
        chat_dialog_->BotReply("请求失败");
        return false;
    }
    CF_LOG_INFO("请求成功 Url: %s", reply->url().toString().toUtf8().data());
    if (reply->url().toString() == resource_loader::get_instance().get_gpt_url()) {
        QByteArray reply_data = std::move(reply->readAll());
        if (!ReplyMessageProcess(reply_data)) {
            return false;
        }
        if (resource_loader::get_instance().tts_enable_) {
            SendVoiceRequest();
            return true;
        }
        chat_dialog_->BotReply(robot_message_);
        LAppLive2DManager::GetInstance()->RobotControl((Csm::csmChar*)motion_.GetRawString(),(Csm::csmChar*)expression_.GetRawString(), nullptr);
    } else {
        CF_LOG_DEBUG("voice request handle");
        std::shared_ptr<QByteArray> voice = std::make_shared<QByteArray>(std::move(reply->readAll()));
        chat_dialog_->BotReply(robot_message_);
        LAppLive2DManager::GetInstance()->RobotControl((Csm::csmChar*)motion_.GetRawString(),(Csm::csmChar*)expression_.GetRawString(), voice);
    }
    return true;
}

bool NetworkManager::ReplyMessageProcess(const QByteArray &reply_data) {

    QJsonDocument reply_document = QJsonDocument::fromJson(reply_data);
    QJsonObject reply_object = reply_document.object();
    QJsonObject usage = reply_object["usage"].toObject();
    total_tokens_ = usage["total"].toInt();
    QJsonObject choices = reply_object["choices"].toArray()[0].toObject();
    QJsonObject reply_message = choices["message"].toObject();
    QString content = reply_message["content"].toString();
    request_messages_->append(reply_message);
    CF_LOG_DEBUG("reply: %s", content.toUtf8().data());
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(content.toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        CF_LOG_ERROR("json error! %s", jsonError.errorString().toUtf8().data());
        return false;
    }
    QJsonObject jsonObject = jsonDoc.object();
    robot_message_ = jsonObject[GPT_MESSAGE].toString();
    motion_ = jsonObject[GPT_MOTION].toString().toUtf8();
    expression_ = jsonObject[GPT_EXPRESSION].toString().toUtf8();
    CF_LOG_INFO("motion: %s", motion_.GetRawString());
    return true;
}

void NetworkManager::ClearMessages() {
    total_tokens_ = 0;
    delete request_messages_;
    request_messages_ = new QJsonArray;
    QJsonObject request_message;
    request_message.insert("role", "system");
    request_message.insert("content", resource_loader::get_instance().get_gpt_system_prompt());
    request_messages_->append(request_message);
}

NetworkManager::~NetworkManager() {
    delete network_manager_;
    delete gpt_request_;
    delete voice_request_;
    delete request_messages_;
}

void NetworkManager::SendVoiceRequest() {
    QUrl url(VoiceAPI);
    QUrlQuery query;
    query.addQueryItem("speaker", "纳西妲_ZH");
    query.addQueryItem("text", robot_message_);
    query.addQueryItem("format", "wav");
    query.addQueryItem("language", "ZH");
    query.addQueryItem("length", "1");
    query.addQueryItem("sdp", "0.4");
    query.addQueryItem("noise", "0.6");
    query.addQueryItem("noisew", "0.8");
    url.setQuery(query);
    voice_request_->setUrl(url);
    network_manager_->get(*voice_request_);
}


