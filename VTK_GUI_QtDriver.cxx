#include <QApplication>
#include "VTK_GUI_Qt.h"

int main( int argc, char** argv )
{
  // QT Stuff
  QApplication app( argc, argv );
  
  VTK_GUI_Qt vtk_GUI_Qt;
  vtk_GUI_Qt.show();
  
  return app.exec();
}
