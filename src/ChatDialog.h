//
// Created by 高煜焜 on 2023/11/5.
//

#ifndef QUIETFRIEND_CHATDIALOG_H
#define QUIETFRIEND_CHATDIALOG_H
#include <QCoreApplication>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QObject>

class ChatDialog : public QDialog
{
Q_OBJECT

public:
    ChatDialog(QWidget *parent = nullptr) : QDialog(parent)
    {
        setWindowTitle("Chat Dialog");

        // 创建布局
        QFormLayout *layout = new QFormLayout(this);
        inputLine = new QLineEdit(this);
        sendButton = new QPushButton("Send", this);
        chatText = new QTextEdit(this);
        chatText->setReadOnly(true);

        layout->addRow("Enter your message:", inputLine);
        layout->addRow(chatText);
        layout->addRow(sendButton);

        // 创建网络请求管理器
        networkManager = new QNetworkAccessManager(this);

        // 连接按钮点击事件到发送消息槽函数
        connect(sendButton, &QPushButton::clicked, this, &ChatDialog::sendMessage);

        // 连接网络响应到处理响应槽函数
        connect(networkManager, &QNetworkAccessManager::finished, this, &ChatDialog::handleResponse);
    }

private slots:
    void sendMessage()
    {
        QString message = inputLine->text();
        if (message.isEmpty())
            return;

        // 清空输入框
        inputLine->clear();

        // 发送消息到服务器
        QUrl url("https://example.com"); // 替换为你的服务器地址
        QNetworkRequest request(url);
        QByteArray requestData = message.toUtf8();

        QNetworkReply *reply = networkManager->post(request, requestData);
    }

    void handleResponse(QNetworkReply *reply)
    {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            chatText->append("Server response: " + data);
        } else {
            chatText->append("Network request error: " + reply->errorString());
        }
        reply->deleteLater();
    }

private:
    QLineEdit *inputLine;
    QPushButton *sendButton;
    QTextEdit *chatText;
    QNetworkAccessManager *networkManager;
};


#endif //QUIETFRIEND_CHATDIALOG_H
