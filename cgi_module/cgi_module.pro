TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
DESTDIR = ../../CGI-build

QMAKE_CXXFLAGS += -std=c++11

SOURCES += \
    cgi_module.cpp
