#include "VTK_GUI_Qt.h"

#include <vtkPolyDataReader.h>
#include <vtkCamera.h>


// Constructor
VTK_GUI_Qt::VTK_GUI_Qt()
{
	this->setupUi(this);
	renderer = vtkSmartPointer<vtkRenderer>::New();

	// VTK/Qt aedded
	this->qvtkWidgetLeft->GetRenderWindow()->AddRenderer(renderer);
	this->qvtkWidgetLeft->GetRenderWindow()->GetInteractor()->Render();

	objectMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	objectActor = vtkSmartPointer<vtkActor>::New();
	// Create pipeline
	objectActor->SetMapper(objectMapper);
	renderer->AddActor(objectActor);

	qvtkWidgetLeft->update();

	// Set up action signals and slots
	connect(this->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
	connect(this->OpenButton, SIGNAL(clicked()), this, SLOT(loading_files()));
	connect(this->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(show_file()));

	// Let the horizontal slider be disabled before loading file.
	// otherwise changing value will cause segfault
	this->horizontalSlider->setEnabled(false);
}


void VTK_GUI_Qt::slotExit()
{
	qApp->exit();
}

template<class TReader>
vtkSmartPointer<vtkPolyData> readVTKfile(std::string fileName)
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
		vtkSmartPointer<vtkPolyData> output = reader->GetOutput();
		std::cout << "output has " << output->GetNumberOfPoints() << " points." << std::endl;
	}
	vtkSmartPointer<vtkPolyData> pd = reader->GetOutput();
	return pd;
}

QStringList VTK_GUI_Qt::getFileNames()
{
	return QFileDialog::getOpenFileNames(this, tr("Choose"), "", tr("Vtk files (*.vtk)"));
}

void VTK_GUI_Qt::fill_data_vector(const QStringList &filenames)
{
	// Let's use the vtkPolyData directly later we can change it for
	// something more general
	vtkSmartPointer<vtkPolyDataReader> reader =
		vtkSmartPointer<vtkPolyDataReader>::New();

	for (int i = 0; i < filenames.size(); i++)
	{
		//convert to std::string once
		std::string fname = filenames[i].toStdString();
		cout << fname << endl;
		data.push_back(readVTKfile<vtkPolyDataReader>(fname));
	}
}

void  VTK_GUI_Qt::loading_files()
{
	QStringList files = getFileNames();
	if (files.empty())
	{
		return;
	}

	fill_data_vector(files);

	//enable slider;
	this->horizontalSlider->setEnabled(true);

	cout << "Data size: " << data.size() << endl;
	this->horizontalSlider->setMinimum(0);
	this->horizontalSlider->setMaximum(data.size() - 1);

	// Update display widget
	show_file();
}

void VTK_GUI_Qt::show_file()
{
	int id = this->horizontalSlider->value();
	cout << "Slider value: " << id << endl;

	//Change data
	objectMapper->SetInputData(data[id]);

	//Automatically set up the camera based on the visible actors.
	renderer->ResetCamera();

	// Calls also update for all connected objects in pipeline
	// like actor, object mapper etc.
	qvtkWidgetLeft->update();

	cout << "qvtkWidget updated." << endl;
}
