//
// Created by 高煜焜 on 2023/11/5.
//

#ifndef QUIETFRIEND_NETWORKUTIL_H
#define QUIETFRIEND_NETWORKUTIL_H
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QObject>
#include <QByteArray>
#include <QDebug>

class NetworkRequestManager : public QObject{
public:
    NetworkRequestManager(QObject *parent = nullptr) : QObject(parent) {
            manager = new QNetworkAccessManager(this);
            connect(manager, &QNetworkAccessManager::finished, this, &NetworkRequestManager::onRequestFinished);
    }

    void sendGetRequest(const QUrl &url) {
        QNetworkRequest request(url);
        currentReply = manager->get(request);
    }

signals:
    void requestFinished(const QByteArray &data, bool success);

private slots:
    void onRequestFinished(QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            emit requestFinished(data, true);
        } else {
            emit requestFinished(QByteArray(), false);
            qDebug() << "Network request error: " << reply->errorString();
        }
        reply->deleteLater();
    }
private:
    QNetworkAccessManager *manager;
    QNetworkReply *currentReply;
};


#endif //QUIETFRIEND_NETWORKUTIL_H
