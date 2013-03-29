TEMPLATE      = app
QT           += network
HEADERS       = tripplanner.h \
    pn532.h \
    pn532_cmd.h
SOURCES       = main.cpp \
                tripplanner.cpp \
    pn532.cpp
FORMS         = tripplanner.ui
