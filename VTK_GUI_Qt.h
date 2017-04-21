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


class VTK_GUI_Qt : public QMainWindow, private Ui::VTK_GUI_Qt
{
  Q_OBJECT
public:

  // Constructor/Destructor
  VTK_GUI_Qt();
  ~VTK_GUI_Qt() {};

private:
	Ui::VTK_GUI_Qt *ui;

	// With vtkSmartPointer we don't have to worry about
	// proper destruction of this object. vtkSmartPointer will do
	// the job for us (reference counting)
	std::vector<vtkSmartPointer<vtkPolyData>> data;

	// To avoid gitches in display window let's have
	// renderer available from very beginning
	vtkSmartPointer<vtkRenderer> renderer;

	// Don't create object mapper and rest of the
	// rendering pipeline every time when show_file is called
	vtkSmartPointer<vtkPolyDataMapper> objectMapper;

	// Same as above
	vtkSmartPointer<vtkActor> objectActor;
	
public slots:
  virtual void slotExit();

 private slots:
 void fill_data_vector(const QStringList& filenames);
 QStringList getFileNames();
 void loading_files();
 void show_file();
};

#endif
