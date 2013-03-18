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

#include "mail.h"
#include <iostream>

Mail::Mail(){
	socket = new QSslSocket();
	connect(socket, SIGNAL(readyRead()), this, SLOT(advanceState()));
	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(errorReceived(QAbstractSocket::SocketError)));
	connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

	textStream = new QTextStream(socket);
	state = Helo;
}

void Mail::send(){
	socket->connectToHostEncrypted(server, serverPort);
}

void Mail::errorReceived(QAbstractSocket::SocketError socketError) {
	std::cerr << "error " << socketError << std::endl;
}

void Mail::disconnected() {
	if(state == Close){
		state = Helo;
	} else {
		std::cerr <<"disconnected" << std::endl;
		std::cerr << "error " << socket->errorString().toStdString() << std::endl;
	}
}

void Mail::advanceState() {
	QString responseLine;
	do{
		responseLine = socket->readLine();
	}while(socket->canReadLine());

	responseLine.truncate(3);

	if(state == Helo && responseLine == "220"){
		*textStream << "HELO localhost\r\n";
		textStream->flush();
		state = Auth;
	}else if(state == Auth && responseLine == "250"){
		*textStream << "AUTH PLAIN " << auth << "\r\n";
		textStream->flush();
		state = MailFrom;
	}else if(state == MailFrom && responseLine == "235"){
		*textStream << "MAIL FROM:<" << from << ">\r\n";
		textStream->flush();
		state = RcptTo;
	}else if(state == RcptTo && responseLine == "250"){
		*textStream << "RCPT TO:<" << to << ">\r\n";
		textStream->flush();
		state = Data;
	}else if(state == Data && responseLine == "250"){
		*textStream << "DATA\r\n";
		textStream->flush();
		state = Body;
	}else if(state == Body && responseLine == "354"){
		*textStream << "from: " << from << "\r\n";
		*textStream << "to: " << to << "\r\n";
		*textStream << "subject: " << subject << "\r\n\r\n";
		*textStream << body << "\r\n.\r\n";
		textStream->flush();
		state = Quit;
	}else if(state == Quit && responseLine == "250"){
		*textStream << "QUIT\r\n";
		textStream->flush();
		state = Close;
		std::cout << "Mail sent" << std::endl;
	}else if(state == Close) {
		socket->disconnectFromHost();
		return;
	}else{
		std::cerr << "Unexpected reply from Mail server:\n\n" << responseLine.toStdString() << std::endl;
		state = Close;
	}
}
