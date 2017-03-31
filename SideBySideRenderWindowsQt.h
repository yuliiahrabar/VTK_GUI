#ifndef SideBySideRenderWindowsQt_H
#define SideBySideRenderWindowsQt_H

#include <vtkSmartPointer.h>

#include <QMainWindow>
#include <QFileDialog>
#include "ui_SideBySideRenderWindowsQt.h"

#include <vtkDataObjectToTable.h>
#include <vtkElevationFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkQtTableView.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSphereSource.h>
#include <vtkCubeSource.h>
#include <vtkSmartPointer.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkDataObject.h>
#include <vtkAlgorithmOutput.h>


class SideBySideRenderWindowsQt : public QMainWindow, private Ui::SideBySideRenderWindowsQt
{
  Q_OBJECT
public:

  // Constructor/Destructor
  SideBySideRenderWindowsQt(); 
  ~SideBySideRenderWindowsQt() {};

private: 
	Ui::SideBySideRenderWindowsQt *ui;

	std::vector<vtkAlgorithmOutput*> data;
public slots:
  virtual void slotExit();

 private slots:
 void fill_data_vector();
 void loading_files();
 void show_file();
};

#endif
