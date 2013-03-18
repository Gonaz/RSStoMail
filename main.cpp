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

#include <QtCore/QCoreApplication>
#include <QFile>
#include <iostream>
#include "timer.h"

int main(int argc, char *argv[]) {
	QCoreApplication a(argc, argv);

	QFile file("settings.xml");
	if (!file.open(QIODevice::ReadOnly)){
		std::cerr << "Error opening file " << file.fileName().toStdString() << std::endl;
		return 2;
	}
	QTextStream stream(&file);
	QString data = stream.readAll();
	file.close();

	QXmlStreamReader xml;
	xml.addData(data);

	QString mailServer;
	quint16 mailServerPort;
	QString mailFrom;
	QString mailTo;
	QString auth;
	int updateInterval;
	QString rssUrl;

	QString currentTag;
	bool mail = false;
	bool rss = false;
	while(!xml.atEnd()){
		xml.readNext();
		if(xml.isStartElement()){
			if(xml.name().toString() == "mail"){
				mail = true;
			}  else if(xml.name().toString() == "rss"){
				mail = false;
				rss = true;
			}
			currentTag = xml.name().toString();
		} else if(xml.isCharacters() && !xml.isWhitespace()){
			if(mail){
				if(currentTag == "url"){
					mailServer = xml.text().toString().trimmed();
				} else if(currentTag == "port"){
					mailServerPort = xml.text().toString().trimmed().toUShort();
				} else if(currentTag == "from"){
					mailFrom = xml.text().toString().trimmed();
				} else if(currentTag == "to"){
					mailTo = xml.text().toString().trimmed();
				} else if(currentTag == "auth"){
					auth = xml.text().toString().trimmed();
				}
			}
			if(rss){
				if(currentTag == "url"){
					rssUrl = xml.text().toString().trimmed();
				} else if(currentTag == "updateInterval"){
					updateInterval = xml.text().toString().trimmed().toInt();
				}
			}
		}
	}
	if(xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError){
		std::cerr << "XML ERROR:" << xml.lineNumber() << ": " << xml.errorString().toStdString();
		return 1;
	}

	Timer *t = new Timer();
	t->setMailServer(mailServer);
	t->setMailServerPort(mailServerPort);
	t->setMailFrom(mailFrom);
	t->setMailTo(mailTo);
	t->setAuth(auth);

	t->setRssUrl(rssUrl);
	t->updateInterval = updateInterval;
	t->start();

	return a.exec();
}
