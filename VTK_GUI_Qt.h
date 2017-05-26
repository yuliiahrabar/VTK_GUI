#ifndef VTK_GUI_Qt_H
#define VTK_GUI_Qt_H

#include <vtkSmartPointer.h>

#include <QMainWindow>
#include <QFileDialog>
#include "ui_VTK_GUI_Qt.h"

// Let us have just necessary headers
#include <vtkRenderer.h>
#include <vtkPolyData.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkCamera.h>
#include <vtkCamera.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkDataSetMapper.h>
#include <vtkNamedColors.h>
#include <vtkColorTransferFunction.h>
#include <vtkLookupTable.h>
#include <vtkFloatArray.h>
#include <vtkUnstructuredGrid.h>

class VTK_GUI_Qt : public QMainWindow, private Ui::VTK_GUI_Qt
{
  Q_OBJECT
public:

  // Constructor/Destructor
  VTK_GUI_Qt();
  ~VTK_GUI_Qt() {};

private:
	Ui::VTK_GUI_Qt *ui;

	int sliderId;

	// vector filled with polydata for use in filters
	std::vector<vtkSmartPointer<vtkPolyData>> polydataVector;

	// vector for correct mapping both Unstructured Grid and Polydata
	std::vector<vtkSmartPointer<vtkGenericDataObjectReader>> mapperDataVector;
	

public slots:
	virtual void slotExit();

private slots:
	void fill_data_vector(const QStringList& filenames);
	QStringList getFileNames();
	void loading_files();
	void show_file();
	void color_update();
	void color_polydata(int arrNum);
	void color_unstructuredGrid(int arrNum);
	void pointData_comboBox();
	void cellData_comboBox();
};

#endif
