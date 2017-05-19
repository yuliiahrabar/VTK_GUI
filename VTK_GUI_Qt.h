#ifndef VTK_GUI_Qt_H
#define VTK_GUI_Qt_H

#include <vtkSmartPointer.h>

#include <QMainWindow>
#include <QFileDialog>
#include "ui_VTK_GUI_Qt.h"

// Let us have just necessary headers
#include <vtkRenderer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredGridReader.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkAlgorithmOutput.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolyDataReader.h>
#include <vtkCamera.h>

class VTK_GUI_Qt : public QMainWindow, private Ui::VTK_GUI_Qt
{
  Q_OBJECT
public:

  // Constructor/Destructor
  VTK_GUI_Qt();
  ~VTK_GUI_Qt() {};

private:
	Ui::VTK_GUI_Qt *ui;
	
public slots:
  virtual void slotExit();

 private slots:
// void fill_data_vector(const QStringList& filenames);
// QStringList getFileNames();
 void loading_files();
 //void show_file();
};

#endif
