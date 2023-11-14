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

    // 设置 QTextEdit 控件的背景为透明
    textEdit->setStyleSheet("background-color: rgba(0, 0, 0, 20);");

    // 添加垂直滚动条以支持滚动
    textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    // 添加文本内容
    textEdit->setPlainText("输入你的消息：");

    // 将 QTextEdit 添加到布局
    layout->addWidget(textEdit);

    auto *inputLayout = new QHBoxLayout;
    inputLine = new QLineEdit(this);
    inputLayout->addWidget(inputLine);
    sendButton = new QPushButton("Send", this);
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
