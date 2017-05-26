#include "VTK_GUI_Qt.h"

// Constructor
VTK_GUI_Qt::VTK_GUI_Qt()
{
	this->setupUi(this);

	// Set up action signals and slots
	connect(this->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
	connect(this->ExitButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(this->OpenButton, SIGNAL(clicked()), this, SLOT(loading_files()));
	connect(this->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(show_file()));

	connect(this->radioButton, SIGNAL(clicked()), this, SLOT(pointData_comboBox()));
	connect(this->radioButton_2, SIGNAL(clicked()), this, SLOT(cellData_comboBox()));
	connect(this->updateButton, SIGNAL(clicked()), this, SLOT(color_update()));

	// Let the horizontal slider be disabled before loading file.
	// otherwise changing value will cause segfault
	this->horizontalSlider->setEnabled(false);
	this->radioButton->setEnabled(false);
	this->radioButton_2->setEnabled(false);
	
}


void VTK_GUI_Qt::slotExit()
{
	qApp->exit();
}


void VTK_GUI_Qt::pointData_comboBox()
{
	vtkSmartPointer<vtkPolyData> polydata =
		vtkSmartPointer<vtkPolyData>::New();

	this->comboBox->clear();
	polydata->ShallowCopy(mapperDataVector[sliderId]->GetOutput());

	vtkIdType numberOfPointArrays = polydata->GetPointData()->GetNumberOfArrays();
	std::cout << "Number of PointData arrays: " << numberOfPointArrays << std::endl;
	double arrayRange[2];
	for (vtkIdType i = 0; i < numberOfPointArrays; i++)
	{
		polydata->GetCellData()->GetArray(i)->GetRange(arrayRange);
		if (arrayRange[0]!=arrayRange[1])
			this->comboBox->addItem(polydata->GetPointData()->GetArrayName(i), QVariant(i));
	}
}

void VTK_GUI_Qt::cellData_comboBox()
{
	vtkSmartPointer<vtkPolyData> polydata =
		vtkSmartPointer<vtkPolyData>::New();

	this->comboBox->clear();
	polydata->ShallowCopy(mapperDataVector[sliderId]->GetOutput());

	vtkIdType numberOfCellArrays = polydata->GetCellData()->GetNumberOfArrays();
	double arrayRange[2];
	for (vtkIdType i = 0; i < numberOfCellArrays; i++)
	{
		polydata->GetCellData()->GetArray(i)->GetRange(arrayRange);
		if (arrayRange[0] != arrayRange[1])
			this->comboBox->addItem(polydata->GetCellData()->GetArrayName(i), QVariant(i));
	}
}


template <typename T>
void PrintColour(T &rgb)
{
	// Don't do this in real code! Range checking etc. is needed.
	for (size_t i = 0; i < 3; ++i)
	{
		if (i < 2)
		{
			std::cout << static_cast<double>(rgb[i]) << " ";
		}
		else
		{
			std::cout << static_cast<double>(rgb[i]);
		}
	}
}

void MakeLUTFromCTF(double scaleRange, int numberOfColors, vtkLookupTable *lut)
{
	vtkSmartPointer<vtkColorTransferFunction> ctf =
		vtkSmartPointer<vtkColorTransferFunction>::New();
	ctf->SetColorSpaceToDiverging();
	cout << "Range: " << scaleRange << endl;
	//// Green to tan.
	//ctf->AddRGBPoint(0.0, 0.085, 0.532, 0.201);
	//ctf->AddRGBPoint(0.5, 0.865, 0.865, 0.865);
	//ctf->AddRGBPoint(1.0, 0.677, 0.492, 0.093);

	//Blue to red
	ctf->AddRGBPoint(0.0, 0.2, 0.1, 0.9);
	ctf->AddRGBPoint(0.5, 1.0, 1.0, 1.0);
	ctf->AddRGBPoint(1.0, 0.7, 0.0, 0.0);

	lut->SetNumberOfTableValues(numberOfColors);
	lut->Build();
	int i = 0; double k = 0;
	while (i < numberOfColors)
	{
		double *rgb;
		rgb = ctf->GetColor(k / scaleRange);
		lut->SetTableValue(i, rgb);
		i++; k += scaleRange / numberOfColors;
	}
}

void MakeCellData(size_t const & tableSize, int arr_num, vtkPolyData* polydata, vtkLookupTable *lut,
	vtkUnsignedCharArray *colors)
{
	double arrayRange[2];
	polydata->GetCellData()->GetArray(arr_num)->GetRange(arrayRange);
	double min = arrayRange[1], max = arrayRange[0];
	double scaleRange = max - min;
	vtkSmartPointer<vtkFloatArray> retrievedArray = vtkFloatArray::SafeDownCast(polydata->GetCellData()->GetArray(arr_num));
	for (int i = 0; i < tableSize; i++)
	{
		double rgb[3];
		unsigned char ucrgb[3];
		float value = retrievedArray->GetValue(i);
		//cout << i << ": " << value << " " << static_cast<double>(value) << endl;
		//lut->GetColor(static_cast<double>(i) / (tableSize - 1), rgb);
		lut->GetColor((static_cast<double>(value) - min) / scaleRange, rgb);
		for (size_t j = 0; j < 3; ++j)
		{
			ucrgb[j] = static_cast<unsigned char>(rgb[j] * 255);
		}
		colors->InsertNextTuple3(ucrgb[0], ucrgb[1], ucrgb[2]);
	}
}


void VTK_GUI_Qt::color_polydata(int arrNum)
{
	vtkSmartPointer<vtkNamedColors> nc =
		vtkSmartPointer<vtkNamedColors>::New();

	vtkSmartPointer<vtkPolyData> dataToColor =
		vtkSmartPointer<vtkPolyData>::New();

	dataToColor = vtkPolyData::SafeDownCast(mapperDataVector[sliderId]->GetOutput());
	
	double arrayRange[2];
	if (this->radioButton->isChecked() == true)
	{
		dataToColor->GetPointData()->GetArray(arrNum)->GetRange(arrayRange);
	}
	else 
	{
		dataToColor->GetCellData()->GetArray(arrNum)->GetRange(arrayRange);
	}
	
	cout << "Range of values for array: " << arrayRange[0] << ":" << arrayRange[1] << endl;
	
	double scaleRange = arrayRange[1] - arrayRange[0];
	
	int tableSize;
	if (this->radioButton->isChecked() == true)
	{
		tableSize = dataToColor->GetNumberOfPoints();
		std::cout << "There are " << tableSize << " points." << std::endl;
	}
	else 
	{
		tableSize = dataToColor->GetNumberOfCells();
		std::cout << "There are " << tableSize << " cells." << std::endl;
	}

	// Create a lookup table to map cell data to colors
	vtkSmartPointer<vtkLookupTable> lookupTable =
		vtkSmartPointer<vtkLookupTable>::New();

	// Generate the colors for each point based on the color map
	vtkSmartPointer<vtkUnsignedCharArray> colors =
		vtkSmartPointer<vtkUnsignedCharArray>::New();
		colors->SetNumberOfComponents(3);
		colors->SetName("Colors");

	int numberOfColors = tableSize;
	MakeLUTFromCTF(scaleRange, numberOfColors, lookupTable);

	vtkSmartPointer<vtkUnsignedCharArray> colorData =
		vtkSmartPointer<vtkUnsignedCharArray>::New();
	colorData->SetName("colors");
	colorData->SetNumberOfComponents(3);

	MakeCellData(tableSize, arrNum, dataToColor, lookupTable, colorData);

	if (this->radioButton->isChecked() == true)
	{
		dataToColor->GetPointData()->SetScalars(colors);
	}
	else
	{
		dataToColor->GetCellData()->SetScalars(colorData);
	}

	//Create a mapper and actor
	vtkSmartPointer<vtkDataSetMapper> mapper =
		vtkSmartPointer<vtkDataSetMapper>::New();

	mapper->SetInputData(dataToColor);
	mapper->SetScalarRange(0, tableSize - 1);
	mapper->SetLookupTable(lookupTable);
	mapper->Update();

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

	this->qvtkWidgetLeft->update();

	cout << "QVTKWidget updated." << endl;

}

void VTK_GUI_Qt::color_unstructuredGrid(int arrNum)
{
	double arrayRange[2];
	vtkSmartPointer<vtkUnstructuredGrid > dataToColor =
		vtkSmartPointer<vtkUnstructuredGrid>::New();
	dataToColor = vtkUnstructuredGrid::SafeDownCast(mapperDataVector[sliderId]->GetOutput());
	dataToColor->GetCellData()->GetArray(arrNum)->GetRange(arrayRange);
	cout << "Range of values for array" << arrayRange[0] << " " << arrayRange[1] << endl;
	double scaleRange = arrayRange[1] - arrayRange[0];

}

void VTK_GUI_Qt::color_update()
{
	vtkSmartPointer<vtkNamedColors> nc =
		vtkSmartPointer<vtkNamedColors>::New();

	// Get the name and id of array used for coloring
	QString arrName = this->comboBox->currentText();
	int arrNum = this->comboBox->itemData(this->comboBox->currentIndex()).toInt();
	cout << "Array: " << arrName.toStdString() << " " << arrNum << endl;
	
	//Check whenether file structure is Polydata or UnstructuredGrid
	if (vtkPolyData::SafeDownCast(mapperDataVector[sliderId]->GetOutput()))
	{
		std::cout << "File is a polydata" << std::endl;
		color_polydata(arrNum);
	}
	else if (vtkUnstructuredGrid::SafeDownCast(mapperDataVector[sliderId]->GetOutput()))
	{
		std::cout << "File is an unstructured grid" << std::endl;
		color_unstructuredGrid(arrNum);
	}
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

		mapperDataVector.push_back(reader);

		vtkSmartPointer<vtkPolyData> polydata =
			vtkSmartPointer<vtkPolyData>::New();

		polydata->ShallowCopy(reader->GetOutput());

		polydataVector.push_back(polydata);

		std::cout << "output has " << polydata->GetNumberOfPoints() << " points." << std::endl;
	}
}

void  VTK_GUI_Qt::show_file()
{
	//Create a mapper and actor

	vtkSmartPointer<vtkDataSetMapper> mapper =
		vtkSmartPointer<vtkDataSetMapper>::New();

	mapper->SetInputConnection(mapperDataVector[sliderId]->GetOutputPort());
	
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
	this->radioButton->setEnabled(true);
	this->radioButton_2->setEnabled(true);

	this->horizontalSlider->setMinimum(0);
	this->horizontalSlider->setMaximum(polydataVector.size() - 1);

	sliderId = this->horizontalSlider->value();
	cout << "Slider value: " << sliderId << endl;

	// Update display widget
	this->qvtkWidgetLeft->update();
	show_file();
}
