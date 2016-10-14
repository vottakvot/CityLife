QT += core
QT -= gui
QT += network

TARGET = Citylife
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

TEMPLATE = app

SOURCES += main.cpp \
    CityLifeLogic.cpp

HEADERS += \
    CityLifeLogic.h
