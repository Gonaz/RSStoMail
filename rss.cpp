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

#include "rss.h"
#include <QFile>
#include <QTextStream>
#include <iostream>

QVector<Entry*> *RSS::fetch(){
	QXmlStreamReader *xml = new QXmlStreamReader();
	QString wget = "wget \"" + url + "\" -q -O rss.xml";
	system(wget.toStdString().c_str());

	QFile file("rss.xml");
	if (!file.open(QIODevice::ReadOnly)){
		std::cerr << "Error opening file " << file.fileName().toStdString() << std::endl;
		return new QVector<Entry*>();
	}
	QTextStream stream(&file);
	QString data = stream.readAll();
	file.close();

	xml->addData(data);
	return parseXML(xml);
}

QVector<Entry*> *RSS::parseXML(QXmlStreamReader *xml){
	bool course = false;
	bool type = false;
	bool first = true;
	QString currentTag;
	QVector<Entry*> *entries = new QVector<Entry*>();
	Entry *entry;

	while(!xml->atEnd()){
		xml->readNext();
		if(xml->isStartElement()){
			if (xml->name() == "entry"){
				entry = new Entry();
			}
			currentTag = xml->name().toString();
		}else if(xml->isEndElement()){
			if(xml->name() == "entry"){
				entries->append(entry);
			}
		}else if(xml->isCharacters() && !xml->isWhitespace()){
			if(currentTag == "title" && !first){
				entry->title = xml->text().toString();
			}else if(currentTag == "title" && first){
				first = false;
			}else if(currentTag == "name"){
				entry->author = xml->text().toString();
			}else if(currentTag == "id"){
				entry->id = xml->text().toString();
			}else if(xml->text().toString() == "Vak/Community"){
				course = true;
			}else if(xml->text().toString() == "Type"){
				type = true;
			}else if(course){
				QString info = xml->text().toString();
				int index = info.indexOf("|");
				info = info.mid(index);
				info = info.trimmed();

				entry->course = info;
				course = false;
			} else if(type){
				QString info = xml->text().toString();
				info.replace(":", "");
				info = info.trimmed();

				entry->type = info;
				type = false;
			}
		}
	}
	if(xml->error() && xml->error() != QXmlStreamReader::PrematureEndOfDocumentError){
		std::cerr << "XML ERROR:" << xml->lineNumber() << ": " << xml->errorString().toStdString();
	}

	delete xml;
	return entries;
}
