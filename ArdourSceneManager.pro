TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += \
    src/ASMView.cpp \
    src/ArdourSceneManager.cpp \
    src/SceneParser/SceneParser.cpp \
    src/Scene/Scene.cpp \
    src/JackMIDI/jackMIDI.cpp \
    src/ArdourSessionParser/ArdourSession.cpp \
    src/Scene/Send.cpp \
    src/Scene/ArdourControllable.cpp \
    src/Scene/Track.cpp \
    src/Scene/Master.cpp \
    src/OSCServer.cpp

HEADERS += \
    src/ASMView.h \
    src/SceneParser/SceneParser.h \
    src/Scene/Scene.h \
    src/JackMIDI/midi.h \
    src/JackMIDI/jackMIDI.h \
    src/ArdourSessionParser/ArdourSession.h \
    src/Scene/Send.h \
    src/Scene/ArdourControllable.h \
    src/Scene/Track.h \
    src/Scene/Master.h \
    OSCServer.h \
    src/OSCServer.h \
    src/ControllerEvent.h

OTHER_FILES += \
    extras/knob.png \
    extras/ArdourSceneManager.map \
    extras/ArdourSceneManager.desktop \
    wscript \
    waf \
    README.md \
    LICENSE \
    band1.scene

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += jack
unix: PKGCONFIG += gtkmm-3.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../usr/lib/x86_64-linux-gnu/release/ -lxml2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../usr/lib/x86_64-linux-gnu/debug/ -lxml2
else:symbian: LIBS += -lxml2
else:unix: LIBS += -L$$PWD/../../../usr/lib/x86_64-linux-gnu/ -lxml2

INCLUDEPATH += $$PWD/../../../usr/include/libxml2
DEPENDPATH += $$PWD/../../../usr/include/libxml2

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../usr/lib/x86_64-linux-gnu/release/xml2.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../usr/lib/x86_64-linux-gnu/debug/xml2.lib
else:unix:!symbian: PRE_TARGETDEPS += $$PWD/../../../usr/lib/x86_64-linux-gnu/libxml2.a
