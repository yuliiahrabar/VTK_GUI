#include "SideBySideRenderWindowsQt.h"

//#include <vtkDataObjectToTable.h>
//#include <vtkElevationFilter.h>
//#include <vtkPolyDataMapper.h>
//#include <vtkQtTableView.h>
//#include <vtkRenderer.h>
//#include <vtkRenderWindow.h>
//#include <vtkSphereSource.h>
//#include <vtkCubeSource.h>
//#include <vtkSmartPointer.h>
//#include <vtkGenericDataObjectReader.h>
//#include <vtkDataObject.h>
//#include <vtkAlgorithmOutput.h>

// Constructor
SideBySideRenderWindowsQt::SideBySideRenderWindowsQt() 
{
  this->setupUi(this);
 
  // Set up action signals and slots
  
  connect(this->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));

  connect(this->OpenButton, SIGNAL(clicked()), this, SLOT(loading_files()));
  
  connect(this->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(show_file()));
}


void SideBySideRenderWindowsQt::slotExit() 
{
  qApp->exit();
}

template<class TReader> vtkAlgorithmOutput *readVTKfile(std::string fileName)
{
	vtkSmartPointer<TReader> reader =
		vtkSmartPointer<TReader>::New();
	reader->SetFileName(fileName.c_str());
	reader->Update();
	reader->GetOutput()->Register(reader);
	// Cheking reader:
	if (reader->IsFilePolyData())
	{
		std::cout << "output is a polydata" << std::endl;
		vtkPolyData* output = reader->GetPolyDataOutput();
		std::cout << "output has " << output->GetNumberOfPoints() << " points." << std::endl;
	}
	vtkAlgorithmOutput *pd = dynamic_cast<vtkAlgorithmOutput *>(reader->GetOutput());
	return pd;
}

void SideBySideRenderWindowsQt::fill_data_vector()
{
	QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Choose"), "", tr("Vtk files (*.vtk)"));
	//std::vector<std::string> inputFilenames(filenames.count());
	std::vector<std::string> inputFilenames;
	inputFilenames.reserve(filenames.count());
	foreach(QString str, filenames) {
		inputFilenames.push_back(str.toStdString());
	}
	data.resize(inputFilenames.size());

	vtkSmartPointer<vtkGenericDataObjectReader> reader =
		vtkSmartPointer<vtkGenericDataObjectReader>::New();

	cout << "Input files: " << endl;
	for (int i = 0; i < inputFilenames.size(); i++)
	{
		cout << inputFilenames[i] << endl;
		data[i] = readVTKfile<vtkGenericDataObjectReader>(inputFilenames[i]);
	}

}

void  SideBySideRenderWindowsQt::loading_files()
{
	fill_data_vector();
	cout << "Data size: " << data.size() << endl;
	this->horizontalSlider->setMinimum(0);
	this->horizontalSlider->setMaximum(data.size()-1);
}

void SideBySideRenderWindowsQt::show_file()
{
	int id = this -> horizontalSlider->value();
	cout << "Slider value: " << id << endl;
	
	// VTK Mapper
	vtkSmartPointer<vtkPolyDataMapper> objectMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();

	objectMapper->SetInputConnection(data[id]);

	vtkSmartPointer<vtkActor> objectActor =
		vtkSmartPointer<vtkActor>::New();
	objectActor->SetMapper(objectMapper);

	// VTK Renderer
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor(objectActor);

	// VTK/Qt wedded
	this->qvtkWidgetLeft->GetRenderWindow()->AddRenderer(renderer);
	this->qvtkWidgetLeft->GetRenderWindow()->GetInteractor()->Render();
	qvtkWidgetLeft->update();
	qvtkWidgetLeft->updateGeometry();
	this->update();
	this->updateGeometry();
	cout << "qvtkWidget updated." << endl;
}
