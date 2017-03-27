#ifndef SideBySideRenderWindowsQt_H
#define SideBySideRenderWindowsQt_H

#include <vtkSmartPointer.h>

#include <QMainWindow>
#include <QFileDialog>
#include "ui_SideBySideRenderWindowsQt.h"

class SideBySideRenderWindowsQt : public QMainWindow, private Ui::SideBySideRenderWindowsQt
{
  Q_OBJECT
public:

  // Constructor/Destructor
  SideBySideRenderWindowsQt(); 
  ~SideBySideRenderWindowsQt() {};

private: 
	Ui::SideBySideRenderWindowsQt *ui;


public slots:
  virtual void slotExit();

 private slots:
  void loading_file();
  void loading_files();

};

#endif
