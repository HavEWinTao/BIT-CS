greaterThan(QT_MAJOR_VERSION, 4):

QT += widgets
QT += core gui
QT += network
QT += sql

CONFIG += c++11

RC_ICONS = bitbug_favicon.ico

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    already.cpp \
    draft.cpp \
    forget.cpp \
    main.cpp \
    login.cpp \
    mainwindow.cpp \
    read.cpp \
    readwithfile.cpp \
    recieve.cpp \
    regist.cpp \
    rubbish.cpp \
    write.cpp

HEADERS += \
    already.h \
    draft.h \
    forget.h \
    login.h \
    mainwindow.h \
    read.h \
    readwithfile.h \
    recieve.h \
    regist.h \
    rubbish.h \
    write.h \
    all.h

FORMS += \
    already.ui \
    draft.ui \
    forget.ui \
    login.ui \
    mainwindow.ui \
    read.ui \
    readwithfile.ui \
    recieve.ui \
    regist.ui \
    rubbish.ui \
    write.ui

TRANSLATIONS += \
    mailbox_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
