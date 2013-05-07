TEMPLATE      = app
QT           += network
HEADERS       = tripplanner.h \
    pn532.h \
    pn532_cmd.h \
    netnfc.h
SOURCES       = main.cpp \
                tripplanner.cpp \
    pn532.cpp \
    netnfc.cpp
FORMS         = tripplanner.ui \
    netnfc.ui
