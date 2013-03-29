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
