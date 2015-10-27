TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

SOURCES += \
    src/main.cpp \
    src/server.cpp \
    src/logger.cpp \
    src/helpers.cpp \
    src/request_strategy.cpp

HEADERS += \
    include/server.h \
    include/logger.h \
    include/headers.h \
    include/helpers.h \
    include/request_strategy.h

