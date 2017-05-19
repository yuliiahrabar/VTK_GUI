#include "VTK_GUI_Qt.h"

#include <vtkGenericDataObjectReader.h>
#include <vtkCamera.h>
#include <vtkCleanPolyData.h>

// Constructor
VTK_GUI_Qt::VTK_GUI_Qt()
{
	this->setupUi(this);

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

QStringList VTK_GUI_Qt::getFileNames()
{
	return QFileDialog::getOpenFileNames(this, tr("Choose"), "", tr("Vtk files (*.vtk)"));
}

void VTK_GUI_Qt::fill_data_vector(const QStringList &filenames)
{
	for (int i = 0; i < filenames.size(); i++)
	{
		//convert to std::string once
		std::string filename = filenames[i].toStdString();
		cout << filename << endl;

		// GenericDataObjectReader allowes to work with files containing both PolyData and UnstructuredGrid 

		vtkSmartPointer<vtkGenericDataObjectReader> reader =
			vtkSmartPointer<vtkGenericDataObjectReader>::New();

		reader->SetFileName(filename.c_str());
		reader->Update();

		vtkSmartPointer<vtkPolyData> polydata =
			vtkSmartPointer<vtkPolyData>::New();

		polydata->ShallowCopy(reader->GetOutput());

		polydataVector.push_back(polydata);

		std::cout << "output has " << polydata->GetNumberOfPoints() << " points." << std::endl;

		vtkAlgorithmOutput * pd;
		pd = reader->GetOutputPort();
		mapperDataVector.push_back(pd);
		pd->Print(std::cout);


	}
}

void  VTK_GUI_Qt::show_file()
{
	//this->setupUi(this);
	
	int id = this->horizontalSlider->value();
	cout << "Slider value: " << id << endl;

	//Create a mapper and actor
	

	//vtkSmartPointer<vtkPolyDataMapper> mapper =
	//	vtkSmartPointer<vtkPolyDataMapper>::New();

	//vtkSmartPointer<vtkCleanPolyData> cleanPolydata =
	//	vtkSmartPointer<vtkCleanPolyData>::New();

	//cleanPolydata -> SetInputData(polydataVector[id]);

	//mapper->SetInputConnection(cleanPolydata->GetOutputPort());

	vtkSmartPointer<vtkDataSetMapper> mapper =
		vtkSmartPointer<vtkDataSetMapper>::New();

	//mapper->SetInputConnection(mapperDataVector[id]);
	
	mapper->SetInputData(polydataVector[id]);
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

	////Set up action signals and slots
	//connect(this->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));

	cout << "QVTKWidget updated." << endl;
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

//	cout << "Data size: " << mapperDataVector.size() << endl;
	this->horizontalSlider->setMinimum(0);
	this->horizontalSlider->setMaximum(polydataVector.size() - 1);

	this->qvtkWidgetLeft->update();
	// Update display widget
	//show_file();
}
