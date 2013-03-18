// Copyright (c) 2011, Jonas Vanthornhout
// All rights reserved.
 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef TIMER_H
#define TIMER_H

#include <QObject>
#include "mail.h"
#include "rss.h"
#include "entry.h"

class Timer : public QObject {
    Q_OBJECT
public:
	explicit Timer(QObject *parent = 0):QObject(parent){mail = new Mail(); rss = new RSS();}
	void start();

	void setMailServer(QString mailServer){mail->server = mailServer;}
	void setMailServerPort(quint16 mailServerPort){mail->serverPort = mailServerPort;}
	void setMailFrom(QString mailFrom){mail->from = mailFrom;}
	void setMailTo(QString mailTo){mail->to = mailTo;}
	void setAuth(QString auth){mail->auth = auth;}
	void setRssUrl(QString rssUrl){rss->url = rssUrl;}

	int updateInterval;

private slots:
	void exec();

private:
	QVector<Entry*> *readOldEntries();
	QVector<Entry*> *findNewEntries(QVector<Entry*> *entries, QVector<Entry*> *oldEntries);
	void sendMail(QVector<Entry*> *entries);
	void write(QVector<Entry*> *entries);

	Mail *mail;
	RSS *rss;
};

#endif // TIMER_H
