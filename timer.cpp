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

#include "timer.h"
#include "rss.h"
#include <QFile>
#include <iostream>
#include <QTextStream>
#include <QTimer>
#include <QtAlgorithms>
#include <QDate>

QVector<Entry*> *Timer::readOldEntries(){
	QVector<Entry*> *entries = new QVector<Entry*>();

	QFile file("entries");
	if (!file.open(QIODevice::ReadOnly)){
		std::cerr << "Error opening file " << file.fileName().toStdString() << std::endl;
		return new QVector<Entry*>();
	}
	QTextStream stream (&file);
	while(!stream.atEnd()){
		Entry *entry = new Entry();
		entry->id = stream.readLine();
		entries->append(entry);
	}
	file.close();

	return entries;
}

QVector<Entry*> *Timer::findNewEntries(QVector<Entry*> *entries, QVector<Entry*> *oldEntries){
	QVector<Entry*> *newEntries = new QVector<Entry*>();

	foreach(Entry *entry, *entries){
		bool contains = false;
		foreach(Entry *oldEntry, *oldEntries){
			if(entry->id == oldEntry->id){
				contains = true;
			}
		}

		if(!contains){
			newEntries->append(entry);
		}
	}

	return newEntries;
}

void Timer::sendMail(QVector<Entry*> *entries){
	std::cout << entries->count() << " new feed(s)" << std::endl;
	QString subject = "";
	QString body = "";

	if(entries->count() == 0){
		return;
	} else if(entries->count() == 1) {
		Entry *entry = entries->at(0);
		subject = entry->title;
		body += "Title: " + entry->title;
		body += "\n\tType: " + entry->type;
		body += "\n\tCourse: " + entry->course;
		body += "\n\tAuthor: " + entry->author;
	} else {
		subject = QString::number(entries->count()) + " new feeds";
		foreach(Entry *entry, *entries){
			body += "Title: " + entry->title;
			body += "\n\tType: " + entry->type;
			body += "\n\tCourse: " + entry->course;
			body += "\n\tAuthor: " + entry->author;
			body += "\n\n";
		}
	}

	mail->subject = "RSS: " + subject;
	mail->body = body;
	mail->send();
}

void Timer::write(QVector<Entry*> *entries){
	QByteArray data;
	foreach(Entry *entry, *entries){
		data.append(entry->id);
		data.append("\n");
	}

	QFile file("entries");
	if (!file.open(QIODevice::Append)){
		std::cerr << "Error opening file " << file.fileName().toStdString() << std::endl;
	}
	file.write(data);
	file.close();
}

void Timer::exec(){
	QVector<Entry*> *oldEntries = readOldEntries();
	QVector<Entry*> *entries = rss->fetch();
	std::cout << QDate::currentDate().toString().toStdString() << " " << QTime::currentTime().toString().toStdString() << " Fetched " << entries->count() << " feed(s)" << std::endl;
	QVector<Entry*> *newEntries = findNewEntries(entries, oldEntries);

	write(newEntries);
	sendMail(newEntries);

	qDeleteAll(*oldEntries);
	qDeleteAll(*entries);
	delete oldEntries;
	delete entries;
	delete newEntries;
}

void Timer::start(){
	std::cout << "Application started" << std::endl;

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(exec()));
	exec();
	timer->start(updateInterval);
}
