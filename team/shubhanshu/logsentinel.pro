QT += core gui widgets
CONFIG += c++17
TARGET = logsentinel_pro
TEMPLATE = app

INCLUDEPATH += ../cpp/include

HEADERS += mainwindow.h
SOURCES += mainwindow.cpp

# For statvfs
LIBS += -lstdc++
