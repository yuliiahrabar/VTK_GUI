#include "VTK_GUI_Qt.h"

#include <vtkGenericDataObjectReader.h>
#include <vtkCamera.h>



// Constructor
VTK_GUI_Qt::VTK_GUI_Qt()
{
	this->setupUi(this);

	// Set up action signals and slots
	connect(this->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
	connect(this->OpenButton, SIGNAL(clicked()), this, SLOT(loading_files()));
	//connect(this->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(show_file()));

	// Let the horizontal slider be disabled before loading file.
	// otherwise changing value will cause segfault
	this->horizontalSlider->setEnabled(false);
}


void VTK_GUI_Qt::slotExit()
{
	qApp->exit();
}


void  VTK_GUI_Qt::loading_files()
{
	this->setupUi(this);

	std::string filename = "C:/Users/YULIA/Desktop/CFD_199500.vtk";
	//std::string filename = "C:/Users/YULIA/Desktop/frac_frontBack_199500.vtk";

	vtkSmartPointer<vtkGenericDataObjectReader> reader =
		vtkSmartPointer<vtkGenericDataObjectReader>::New();

	reader->SetFileName(filename.c_str());
	reader->Update();

	vtkSmartPointer<vtkPolyData> polydata =
		vtkSmartPointer<vtkPolyData>::New();
	std::vector<vtkSmartPointer<vtkPolyData>> polydataVector;
	polydata->ShallowCopy(reader->GetOutput());
	polydataVector.push_back(polydata);

	std::cout << "output has " << polydataVector[0]->GetNumberOfPoints() << " points." << std::endl;

	std::vector<vtkAlgorithmOutput *> mapperDataVector;
	vtkAlgorithmOutput * pd;

	pd = reader->GetOutputPort();

	mapperDataVector.push_back(pd);

	//Create a mapper and actor
	vtkSmartPointer<vtkDataSetMapper> mapper =
		vtkSmartPointer<vtkDataSetMapper>::New();

	mapper->SetInputConnection(mapperDataVector[0]);

	vtkSmartPointer<vtkActor> actor =
		vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

	// VTK Renderer
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor(actor);

	vtkSmartPointer<vtkRenderer> rightRenderer =
		vtkSmartPointer<vtkRenderer>::New();
	// VTK/Qt wedded
	this->qvtkWidgetLeft->GetRenderWindow()->AddRenderer(renderer);

	// Set up action signals and slots
	connect(this->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
}


