#if 1
#include <QApplication>
#include "netnfc.h"
#include <stdlib.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    NetNfc netnfc;
    netnfc.show();
    return app.exec();
}

#else

#include <QApplication>
#include "tripplanner.h"
#include <stdlib.h>



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TripPlanner tripPlanner;
    tripPlanner.show();
    return app.exec();
}

#endif
