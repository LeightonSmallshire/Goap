#-------------------------------------------------
#
# Project created by QtCreator 2019-08-20T16:29:55
# V0.2
#
#-------------------------------------------------

QT       -= gui
CONFIG += c++11

#TARGET = Goap
#TEMPLATE = lib
#CONFIG += staticlib

DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
        action.hpp \
        planner.hpp \
        reality.hpp \
        types.hpp
SOURCES += \
    action.cpp \
    main.cpp \
    planner.cpp \
    reality.cpp \
    types.cpp

#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}
