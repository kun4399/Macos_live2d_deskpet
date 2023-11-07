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

class ChatDialog : public QDialog {
Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);

    ~ChatDialog() override;

protected:
//    void focusOutEvent(QFocusEvent *event) override {
//        QDialog::focusOutEvent(event); // 调用基类的事件处理
//
//        // 当窗口失去焦点时，关闭窗口。这个一直实现不了
//        if (textEdit->hasFocus() || inputLine->hasFocus() || sendButton->hasFocus()) {
//            return;
//        } else {
//            close();
//        }
//    }
//    void mousePressEvent(QMouseEvent *event) override ;
//
//    void mouseMoveEvent(QMouseEvent *event) override ;
//
//    void mouseReleaseEvent(QMouseEvent *event) override ;

private slots:

    void sendMessage();

private:
    QLineEdit *inputLine;
    QPushButton *sendButton;
    QTextEdit *textEdit;
    QNetworkAccessManager *networkManager;
    bool mouse_press; // 鼠标按下
};


#endif //QUIETFRIEND_CHATDIALOG_H
