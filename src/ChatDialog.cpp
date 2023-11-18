//
// Created by 高煜焜 on 2023/11/5.
//

#include "ChatDialog.h"
#include <QMouseEvent>

//namespace {
//    int pos_x;
//    int pos_y;
//

ChatDialog::ChatDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("聊天");
    this->setAttribute(Qt::WA_TranslucentBackground);
//        this->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setWindowFlag(Qt::NoDropShadowWindowHint);
//    this->setWindowFlag(Qt::WindowStaysOnTopHint);
    auto &model = resource_loader::get_instance();
    this->resize(model.dialog_width, model.dialog_height);
    this->move(model.dialog_x, model.dialog_y);
    network_manager_ = new NetworkManager(this);
    // 创建 QVBoxLayout 用于放置 QTextEdit 控件
    auto *layout = new QVBoxLayout(this);

    // 创建 QTextEdit 控件
    textEdit = new QTextEdit(this);

    // 设置 QTextEdit 控件为只读
    textEdit->setReadOnly(true);

    textEdit->setStyleSheet(
            "QTextEdit {"
            "background-color: rgba(220, 240, 255, 50%); "
            "border: 1px solid #94B8FF; "
            "border-radius: 10px; "
            "padding: 1px 1px; "
            "font-family: 'Fangzheng Lanting Black', sans-serif; font-size: 14px; color: #333333;"
            "}"
            "QScrollBar:vertical {"
            "    width: 0px; /* 隐藏垂直滚动条滑块宽度 */"
            "}"
            "QScrollBar:horizontal {"
            "    height: 0px; /* 隐藏水平滚动条滑块高度 */"
            "}"
    );

    // 添加文本内容
    textEdit->setPlainText("输入你的消息：");
    layout->addWidget(textEdit);

    auto *inputLayout = new QHBoxLayout;
    inputLine = new QLineEdit(this);
    // 美化输入文本框
    inputLine->setStyleSheet(
            "QLineEdit {"
            "    border: 1px solid #94B8FF;"
            "    background: rgba(220, 240, 255, 30%);"
            "    border-radius: 5px;"
            "    padding: 5px;"
            "}"
    );

    inputLayout->addWidget(inputLine);
    sendButton = new QPushButton("Send", this);
    // 美化按钮
    sendButton->setStyleSheet(
            "QPushButton {"
            "    background-color: #5082E8;"
            "    color: white;"
            "    border: 1px solid #5082E8;"
            "    border-radius: 5px;"
            "    padding: 5px 10px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #405F9E;"
            "}"
            "QPushButton:pressed {"
            "    background-color: #304974;"
            "}"
    );

    inputLayout->addWidget(sendButton);
    layout->addLayout(inputLayout);
    setLayout(layout);
    connect(sendButton, &QPushButton::clicked, this, &ChatDialog::sendMessage);
}

ChatDialog::~ChatDialog() {
    delete textEdit;
    delete inputLine;
    delete sendButton;
}

void ChatDialog::sendMessage() {
    QString message = inputLine->text();
    if (!message.isEmpty()) {
        sendButton->setEnabled(false);
        textEdit->append("You:\n " + message);
        inputLine->clear();
        // 滚动到底部
        QTextCursor cursor = textEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        textEdit->setTextCursor(cursor);
        if (resource_loader::get_instance().gpt_enable_) { network_manager_->SendRequest(message); }
        else {
            textEdit->append("GPT serves is not enabled");
            sendButton->setEnabled(true);
            // 滚动到底部
            QTextCursor cursor = textEdit->textCursor();
            cursor.movePosition(QTextCursor::End);
            textEdit->setTextCursor(cursor);
            sendButton->setEnabled(true);
        }
    }
}

void ChatDialog::BotReply(const QString &content) {
    textEdit->append("Bot:\n " + content);
    sendButton->setEnabled(true);
    // 滚动到底部
    QTextCursor cursor = textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    textEdit->setTextCursor(cursor);
    sendButton->setEnabled(true);
}

//void ChatDialog::mousePressEvent(QMouseEvent *event) {
//    if (event->button() == Qt::LeftButton) {
//        pos_x = event->globalPosition().x();
//        pos_y = event->globalPosition().y();
//        this->mouse_press = true;
//    }
//}
//
//void ChatDialog::mouseMoveEvent(QMouseEvent *event) {
//    if (this->mouse_press) {
//        this->setCursor(Qt::ClosedHandCursor);
//        this->move(this->pos().x() + event->globalPosition().x() - pos_x,
//                   this->pos().y() + event->globalPosition().y() - pos_y);
//        pos_x = event->globalPosition().x();
//        pos_y = event->globalPosition().y();
//    }
//}
//
//void ChatDialog::mouseReleaseEvent(QMouseEvent *event) {
//    if (event->button() == Qt::LeftButton) {
//        this->mouse_press = false;
//        this->setCursor(Qt::ArrowCursor);
//        resource_loader::get_instance().update_dialog_position(this->pos().x(), this->pos().y());
//    }
//}
