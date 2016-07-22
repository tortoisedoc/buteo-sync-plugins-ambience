TEMPLATE = lib
TARGET = storage-storage
DEPENDPATH += .
INCLUDEPATH += . 

CONFIG += link_pkgconfig plugin

PKGCONFIG = buteosyncfw5
target.path = /usr/lib/buteo-plugins-qt5

QT += dbus sql network
QT -= gui

VER_MAJ = 1
VER_MIN = 0
VER_PAT = 0

#input
HEADERS += MyStoragePlugin.h

SOURCES += MyStoragePlugin.cpp

QMAKE_CXXFLAGS = -Wall \
    -g \
    -Wno-cast-align \
    -O2 -finline-functions

#clean
QMAKE_CLEAN += $(TARGET) $(TARGET0) $(TARGET1) $(TARGET2)
QMAKE_CLEAN += $(OBJECTS_DIR)/*.gcda $(OBJECTS_DIR)/*.gcno $(OBJECTS_DIR)/*.gcov $(OBJECTS_DIR)/moc_*

#install

storage.path = /etc/buteo/profiles/storage
storage.files = xml/storage/*


storage.path = /etc/buteo/profiles/storage
storage.files = xml/storage/*

INSTALLS += target storage


INSTALLS += storage

