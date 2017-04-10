#ifndef SideBySideRenderWindowsQt_H
#define SideBySideRenderWindowsQt_H

#include <vtkSmartPointer.h>

#include <QMainWindow>
#include <QFileDialog>
#include "ui_SideBySideRenderWindowsQt.h"

// Let us have just necessary headers
#include <vtkRenderer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>


class SideBySideRenderWindowsQt : public QMainWindow, private Ui::SideBySideRenderWindowsQt
{
  Q_OBJECT
public:

  // Constructor/Destructor
  SideBySideRenderWindowsQt(); 
  ~SideBySideRenderWindowsQt() {};

private: 
	Ui::SideBySideRenderWindowsQt *ui;

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
