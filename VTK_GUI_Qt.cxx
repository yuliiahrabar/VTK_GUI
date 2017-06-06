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
	connect(this->infoButton, SIGNAL(clicked()), this, SLOT(show_info()));
	
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

template <class T>
void FindAllData(vtkSmartPointer<T> data)
{
	vtkIdType numberOfPointArrays = data->GetPointData()->GetNumberOfArrays();
	std::cout << "Number of PointData arrays: " << numberOfPointArrays << std::endl;

	vtkIdType numberOfCellArrays = data->GetCellData()->GetNumberOfArrays();
	std::cout << "Number of CellData arrays: " << numberOfCellArrays << std::endl;

	std::cout << "Type table/key: " << std::endl;;
	//more values can be found in <VTK_DIR>/Common/vtkSetGet.h
	std::cout << VTK_UNSIGNED_CHAR << " unsigned char" << std::endl;
	std::cout << VTK_UNSIGNED_INT << " unsigned int" << std::endl;
	std::cout << VTK_FLOAT << " float" << std::endl;
	std::cout << VTK_DOUBLE << " double" << std::endl;

	cout << "PointArrays:" << endl;
	for (vtkIdType i = 0; i < numberOfPointArrays; i++)
	{
		int dataTypeID = data->GetPointData()->GetArray(i)->GetDataType();
		std::cout << "Array " << i << ": " << data->GetPointData()->GetArrayName(i)
			<< " (type: " << dataTypeID << ") ";
		double arrayRange[2];
		data->GetPointData()->GetArray(i)->GetRange(arrayRange);
		cout << "[" << arrayRange[0] << "," << arrayRange[1] << "]" << endl;
	}

	cout << "CellArrays:" << endl;
	for (vtkIdType i = 0; i < numberOfCellArrays; i++)
	{
		int dataTypeID = data->GetCellData()->GetArray(i)->GetDataType();
		std::cout << "Array " << i << ": " << data->GetCellData()->GetArrayName(i)
			<< " (type: " << dataTypeID << ") ";
		double arrayRange[2];
		data->GetCellData()->GetArray(i)->GetRange(arrayRange);
		cout << "[" << arrayRange[0] << "," << arrayRange[1] << "]" << endl;
	}
	std::cout << endl;
}

void VTK_GUI_Qt::show_info()
{
	if (mapperDataVector[sliderId]->IsFilePolyData())
	{
		std::cout << endl;
		std::cout << "File is a polydata" << std::endl;
		vtkSmartPointer<vtkPolyData> polydata =
			vtkSmartPointer<vtkPolyData>::New();
		polydata = vtkPolyData::SafeDownCast(mapperDataVector[sliderId]->GetOutput());
		FindAllData<vtkPolyData>(polydata);
	}
	else if (mapperDataVector[sliderId]->IsFileUnstructuredGrid())
	{
		std::cout << endl;
		std::cout << "File is an unstructured grid" << std::endl;
		vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid =
			vtkSmartPointer<vtkUnstructuredGrid>::New();
		unstructuredGrid = vtkUnstructuredGrid::SafeDownCast(mapperDataVector[sliderId]->GetOutput());
		FindAllData<vtkUnstructuredGrid>(unstructuredGrid);
	}

}

QStringList VTK_GUI_Qt::getFileNames()
{
	return QFileDialog::getOpenFileNames(this, tr("Choose"), "", tr("Vtk files (*.vtk)"));
}

void VTK_GUI_Qt::pointData_comboBox()
{
	this->comboBox->clear();
	if (mapperDataVector[sliderId]->IsFilePolyData())
	{
		vtkSmartPointer<vtkPolyData> polydata =
			vtkSmartPointer<vtkPolyData>::New();
		polydata = vtkPolyData::SafeDownCast(mapperDataVector[sliderId]->GetOutput());
		vtkIdType numberOfPointArrays = polydata->GetPointData()->GetNumberOfArrays();
		double arrayRange[2];
		for (vtkIdType i = 0; i < numberOfPointArrays; i++)
		{
			polydata->GetPointData()->GetArray(i)->GetRange(arrayRange);
			if (arrayRange[0] != arrayRange[1])
				this->comboBox->addItem(polydata->GetPointData()->GetArrayName(i), QVariant(i));
		}
	}
	else if (mapperDataVector[sliderId]->IsFileUnstructuredGrid())
	{
		vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid =
			vtkSmartPointer<vtkUnstructuredGrid>::New();
		unstructuredGrid = vtkUnstructuredGrid::SafeDownCast(mapperDataVector[sliderId]->GetOutput());
		vtkIdType numberOfPointArrays = unstructuredGrid->GetPointData()->GetNumberOfArrays();
		double arrayRange[2];
		for (vtkIdType i = 0; i < numberOfPointArrays; i++)
		{
			unstructuredGrid->GetPointData()->GetArray(i)->GetRange(arrayRange);
			if (arrayRange[0] != arrayRange[1]) 
				this->comboBox->addItem(unstructuredGrid->GetPointData()->GetArrayName(i), QVariant(i));
		}
	}
}

void VTK_GUI_Qt::cellData_comboBox()
{
	this->comboBox->clear();
	if (mapperDataVector[sliderId]->IsFilePolyData())
	{
		vtkSmartPointer<vtkPolyData> polydata =
			vtkSmartPointer<vtkPolyData>::New();
		polydata = vtkPolyData::SafeDownCast(mapperDataVector[sliderId]->GetOutput());
		
		vtkIdType numberOfCellArrays = polydata->GetCellData()->GetNumberOfArrays();
		double arrayRange[2];
		for (vtkIdType i = 0; i < numberOfCellArrays; i++)
		{
			polydata->GetCellData()->GetArray(i)->GetRange(arrayRange);
			if (arrayRange[0] != arrayRange[1])
				this->comboBox->addItem(polydata->GetCellData()->GetArrayName(i), QVariant(i));
		}
	}
	else if (mapperDataVector[sliderId]->IsFileUnstructuredGrid())
	{
		vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid =
			vtkSmartPointer<vtkUnstructuredGrid>::New();
		unstructuredGrid = vtkUnstructuredGrid::SafeDownCast(mapperDataVector[sliderId]->GetOutput());
		unstructuredGrid->ShallowCopy(mapperDataVector[sliderId]->GetOutput());

		vtkIdType numberOfCellArrays = unstructuredGrid->GetCellData()->GetNumberOfArrays();
		double arrayRange[2];
		for (vtkIdType i = 0; i < numberOfCellArrays; i++)
		{
			unstructuredGrid->GetCellData()->GetArray(i)->GetRange(arrayRange);
			if (arrayRange[0] != arrayRange[1])
				this->comboBox->addItem(unstructuredGrid->GetCellData()->GetArrayName(i), QVariant(i));
		}
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

template <class T>
void draw_with_color(vtkSmartPointer<T> data)
{
	vtkSmartPointer<vtkNamedColors> nc =
		vtkSmartPointer<vtkNamedColors>::New();

	data->ShallowCopy(mapperDataVector[sliderId]->GetOutput());

	QString arrName = this->comboBox->currentText();
	int arrNum = this->comboBox->itemData(this->comboBox->currentIndex()).toInt();

	cout << arrName.toStdString() << " " << arrNum << endl;

	double arrayRange[2];
	data->GetCellData()->GetArray(arrNum)->GetRange(arrayRange);
	cout << "Range of values for array" << arrayRange[0] << " " << arrayRange[1] << endl;
	double scaleRange = arrayRange[1] - arrayRange[0];

	// Create a lookup table to map cell data to colors
	vtkSmartPointer<vtkLookupTable> lookupTable =
		vtkSmartPointer<vtkLookupTable>::New();

	// Generate the colors for each point based on the color map
	vtkSmartPointer<vtkUnsignedCharArray> colors =
		vtkSmartPointer<vtkUnsignedCharArray>::New();
	colors->SetNumberOfComponents(3);
	colors->SetName("Colors");

	int tableSize = data->GetNumberOfCells();
	std::cout << "There are " << tableSize << " cells." << std::endl;

	int numberOfColors = tableSize;
	MakeLUTFromCTF(scaleRange, numberOfColors, lookupTable);
	//MakeLUTFromCTF(tableSize,   lookupTable);

	vtkSmartPointer<vtkUnsignedCharArray> colorData =
		vtkSmartPointer<vtkUnsignedCharArray>::New();
	colorData->SetName("colors");
	colorData->SetNumberOfComponents(3);

	MakeCellData(tableSize, arrNum, data, lookupTable, colorData);
	////MakeCellData(tableSize,  lookupTable, colorData);

	if (this->radioButton->isChecked() == true)
		data->GetPointData()->SetScalars(colors);
	else
		data->GetCellData()->SetScalars(colorData);

	//Create a mapper and actor
	vtkSmartPointer<vtkDataSetMapper> mapper =
		vtkSmartPointer<vtkDataSetMapper>::New();

	//mapper->SetInputConnection(mapperDataVector[sliderId]->GetOutputPort());

	mapper->SetInputData(polydata);
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

	cout << "QVTKWidget updated." << endl;
}


void VTK_GUI_Qt::color_update()
{
	if (mapperDataVector[sliderId]->IsFilePolyData())
	{
		vtkSmartPointer<vtkPolyData> polydata =
			vtkSmartPointer<vtkPolyData>::New();
		polydata = vtkPolyData::SafeDownCast(mapperDataVector[sliderId]->GetOutput());
		//draw_with_color<vtkPolyData>(polydata);
	}
	else if (mapperDataVector[sliderId]->IsFileUnstructuredGrid())
	{
		vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid =
			vtkSmartPointer<vtkUnstructuredGrid>::New();
		unstructuredGrid = vtkUnstructuredGrid::SafeDownCast(mapperDataVector[sliderId]->GetOutput());
		//draw_with_color<vtkUnstructuredGrid>(unstructuredGrid);
	}
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

	sliderId = this->horizontalSlider->value();

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

	this->qvtkWidgetLeft->update();

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

//	cout << "Data size: " << mapperDataVector.size() << endl;
	this->horizontalSlider->setMinimum(0);
	this->horizontalSlider->setMaximum(polydataVector.size() - 1);

	sliderId = this->horizontalSlider->value();
	cout << "Slider value: " << sliderId << endl;

	// Update display widget
	
	show_file();
}
