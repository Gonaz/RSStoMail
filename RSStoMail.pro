#-------------------------------------------------
#
# Project created by QtCreator 2011-10-08T10:57:12
#
#-------------------------------------------------

QT       += core
QT		 += network xml

QT       -= gui

TARGET = RSStoMail
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    mail.cpp \
    rss.cpp \
    timer.cpp

HEADERS += \
    mail.h \
    rss.h \
    entry.h \
    timer.h
