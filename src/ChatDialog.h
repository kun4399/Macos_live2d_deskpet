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
#include <QScrollArea>
#include "resource_loader.hpp"
#include "Networkutil.h"
class NetworkManager;
class ChatDialog : public QDialog {
Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);

    ~ChatDialog() override;

    void BotReply(const QString &content);

private slots:
    void sendMessage();
private:
    QLineEdit *inputLine;
    QPushButton *sendButton;
    QTextEdit *textEdit;
    NetworkManager *network_manager_;
};


#endif //QUIETFRIEND_CHATDIALOG_H
