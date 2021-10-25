#ifndef GUILOGGER_H
#define GUILOGGER_H
#include "spdlog/sinks/base_sink.h"
#include <QTextBrowser>
#include <QScrollBar>
#include "DigitizerConstants.h"
#include <QObject>
class QGUILogHelper : public QObject{
    Q_OBJECT

private:
    QTextBrowser * textBrowser;
public:
    QGUILogHelper(QTextBrowser * textBrowser) :textBrowser(textBrowser) {
        connect(this, &QGUILogHelper::doAppendMessage, this->textBrowser, [=](QString msg){this->textBrowser->append(msg);}, Qt::ConnectionType::QueuedConnection);
        this->textBrowser->verticalScrollBar()->connect(this->textBrowser->verticalScrollBar(), &QScrollBar::rangeChanged, [=](int min, int max) {
            this->textBrowser->verticalScrollBar()->setValue(max);
        });
    }
    void appendMessage(QString qmsg)
    {
        emit this->doAppendMessage(qmsg);
    }
signals:
    void doAppendMessage(QString msg);
};

template<typename Mutex>
class QGUILogSink : public spdlog::sinks::base_sink <Mutex>
{
public:
    QGUILogSink(QTextBrowser *txt) : textBrowser(txt), helper(txt)
    {

    }

protected:
    QTextBrowser *textBrowser;
    QGUILogHelper helper;
    void sink_it_(const spdlog::details::log_msg& msg) override
    {

        // log_msg is a struct containing the log entry info like level, timestamp, thread id etc.
        // msg.raw contains pre formatted log

        // If needed (very likely but not mandatory), the sink formats the message before sending it to its final destination:
        spdlog::memory_buf_t formatted;
        spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
        std::string color = LOG_COLORS[msg.level];
        QString qmsg = QString::fromStdString(fmt::format("<span style=\"color:{}\">{}</span>", color, fmt::to_string(formatted)));
        helper.appendMessage(qmsg);
    }

    void flush_() override
    {
       //std::cout << std::flush;
    }

};

#include "spdlog/details/null_mutex.h"
#include <mutex>
using QGUILogSink_mt = QGUILogSink<std::mutex>;
using QGUILogSink_st = QGUILogSink<spdlog::details::null_mutex>;
#endif // GUILOGGER_H
