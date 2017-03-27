#include "SideBySideRenderWindowsQt.h"

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

// Constructor
SideBySideRenderWindowsQt::SideBySideRenderWindowsQt() 
{
  this->setupUi(this);
  /*
  // Sphere
  vtkSmartPointer<vtkSphereSource> sphereSource = 
      vtkSmartPointer<vtkSphereSource>::New();
  sphereSource->Update();
  vtkSmartPointer<vtkPolyDataMapper> sphereMapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkSmartPointer<vtkActor> sphereActor = 
      vtkSmartPointer<vtkActor>::New();
  sphereActor->SetMapper(sphereMapper);
  
  // Cube
  vtkSmartPointer<vtkCubeSource> cubeSource = 
      vtkSmartPointer<vtkCubeSource>::New();
  cubeSource->Update();
  vtkSmartPointer<vtkPolyDataMapper> cubeMapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  cubeMapper->SetInputConnection(cubeSource->GetOutputPort());
  vtkSmartPointer<vtkActor> cubeActor = 
      vtkSmartPointer<vtkActor>::New();
  cubeActor->SetMapper(cubeMapper);
  
  // VTK Renderer
  vtkSmartPointer<vtkRenderer> leftRenderer = 
      vtkSmartPointer<vtkRenderer>::New();
  leftRenderer->AddActor(sphereActor);
  
  vtkSmartPointer<vtkRenderer> rightRenderer = 
      vtkSmartPointer<vtkRenderer>::New();

  // Add Actor to renderer
  rightRenderer->AddActor(cubeActor);

  // VTK/Qt wedded
  this->qvtkWidgetLeft->GetRenderWindow()->AddRenderer(leftRenderer);
  */

  // Set up action signals and slots
  
  connect(this->OpenButton, SIGNAL(clicked()), this, SLOT(loading_file()));
 // connect(this->SaveButton, SIGNAL(clicked()), this, SLOT(saving_file()));
  connect(this->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
}


void SideBySideRenderWindowsQt::slotExit() 
{
  qApp->exit();
}

vtkSmartPointer<vtkPolyData> getObject(vtkSmartPointer<vtkPolyData> myObject)
{
	return myObject;
}

void SideBySideRenderWindowsQt::loading_file()
{

	// simply set filename
	//std::string inputFilename = "vtk.vtk";

	QString filename = QFileDialog::getOpenFileName(this, tr("Choose"), "", tr("Vtk files (*.vtk)"));
	std::string inputFilename = filename.toLocal8Bit().constData();
	
	// Get all data from the file
	vtkSmartPointer<vtkGenericDataObjectReader> reader =
		vtkSmartPointer<vtkGenericDataObjectReader>::New();
	reader->SetFileName(inputFilename.c_str());
	reader->Update();

	// All of the standard data types can be checked and obtained like this:
	if(reader->IsFilePolyData())
	{
		std::cout << "output is a polydata" << std::endl;
		vtkPolyData* output = reader->GetPolyDataOutput();
		std::cout << "output has " << output->GetNumberOfPoints() << " points." << std::endl;
	}
	
	vtkSmartPointer<vtkPolyDataMapper> objectMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();

	objectMapper->SetInputConnection(reader ->GetOutputPort());
	
	vtkSmartPointer<vtkActor> objectActor =
		vtkSmartPointer<vtkActor>::New();
	objectActor->SetMapper(objectMapper);

	// VTK Renderer
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor(objectActor);

	// VTK/Qt wedded
	this->qvtkWidgetLeft->GetRenderWindow()->AddRenderer(renderer);

}


template<class TReader> vtkDataSet *readVTKfile(std::string fileName)
{
	vtkSmartPointer<TReader> reader =
		vtkSmartPointer<TReader>::New();
	reader->SetFileName(fileName.c_str());
	reader->Update();
	reader->GetOutput()->Register(reader);
	return vtkDataSet::SafeDownCast(reader->GetOutput());
}

void SideBySideRenderWindowsQt::loading_files()
{

	QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Choose"), "", tr("Vtk files (*.vtk)"));
	std::vector<std::string> inputFilenames(filenames.count());
	if (!filenames.isEmpty())
	{
		for (int i = 0; i < filenames.count(); i++)
			inputFilenames.push_back(filenames.at(i).toLocal8Bit().constData());
	}
	
	std::vector<vtkDataSet*> data(inputFilenames.size());
	
	//vtkDataSet *dataSet;

	vtkSmartPointer<vtkGenericDataObjectReader> reader =
		vtkSmartPointer<vtkGenericDataObjectReader>::New();

	for (int i = 0; i < inputFilenames.size(); i++)
	{
		data[i] = readVTKfile<vtkGenericDataObjectReader>(inputFilenames[i]);
	}

	// All of the standard data types can be checked and obtained like this:
	if (data[0]->IsFilePolyData())
	{
		std::cout << "output is a polydata" << std::endl;
		vtkPolyData* output = reader->GetPolyDataOutput();
		std::cout << "output has " << output->GetNumberOfPoints() << " points." << std::endl;
	}

	vtkSmartPointer<vtkPolyDataMapper> objectMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();

	//objectMapper->SetInputConnection(reader->GetOutputPort());
	objectMapper->SetInputConnection(data[0]);


	vtkSmartPointer<vtkActor> objectActor =
		vtkSmartPointer<vtkActor>::New();
	objectActor->SetMapper(objectMapper);

	// VTK Renderer
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor(objectActor);

	// VTK/Qt wedded
	this->qvtkWidgetLeft->GetRenderWindow()->AddRenderer(renderer);

}


