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

	connect(this->checkBoxX, SIGNAL(clicked()), this, SLOT(enable_planes()));
	connect(this->checkBoxY, SIGNAL(clicked()), this, SLOT(enable_planes()));
	connect(this->checkBoxZ, SIGNAL(clicked()), this, SLOT(enable_planes()));
	connect(this->spinBoxX, SIGNAL(valueChanged(int)), this, SLOT(show_cutting_planes()));
	connect(this->spinBoxY, SIGNAL(valueChanged(int)), this, SLOT(show_cutting_planes()));
	connect(this->spinBoxZ, SIGNAL(valueChanged(int)), this, SLOT(show_cutting_planes()));
	
	// Let the horizontal slider be disabled before loading file.
	// otherwise changing value will cause segfault
	this->horizontalSlider->setEnabled(false);
	this->radioButton->setEnabled(false);
	this->radioButton_2->setEnabled(false);
	this->spinBoxX->setEnabled(false);
	this->spinBoxY->setEnabled(false);
	this->spinBoxZ->setEnabled(false);
	this->verticalSliderX->setEnabled(false);
	this->verticalSliderY->setEnabled(false);
	this->verticalSliderZ->setEnabled(false);
	this->infoButton->setEnabled(false);
	
	this->checkBoxX->setEnabled(false);
	this->checkBoxY->setEnabled(false);
	this->checkBoxZ->setEnabled(false);

	this->comboBox->setEnabled(false);
	this->updateButton->setEnabled(false);
}

void VTK_GUI_Qt::slotExit()
{
	qApp->exit();
}

void VTK_GUI_Qt::enable_planes()
{
	if (this->checkBoxX->isChecked())
	{
		this->verticalSliderX->setEnabled(true);
		this->spinBoxX->setEnabled(true);
	}
	else
	{
		this->spinBoxX->setEnabled(false);
		this->verticalSliderX->setEnabled(false);
	}
		
	if (this->checkBoxY->isChecked())
	{
		this->verticalSliderY->setEnabled(true);
		this->spinBoxY->setEnabled(true);
	}
	else
	{
		this->spinBoxY->setEnabled(false);
		this->verticalSliderY->setEnabled(false);
	}
	
	if (this->checkBoxZ->isChecked())
	{
		this->verticalSliderZ->setEnabled(true);
		this->spinBoxZ->setEnabled(true);
	}
	else
	{
		this->spinBoxZ->setEnabled(false);
		this->verticalSliderZ->setEnabled(false);
	}

	show_cutting_planes();
}


void VTK_GUI_Qt::show_cutting_planes()
{
	//Create a mapper and actor

	vtkSmartPointer<vtkDataSetMapper> mapper =
		vtkSmartPointer<vtkDataSetMapper>::New();

	sliderId = this->horizontalSlider->value();

	mapper->SetInputConnection(mapperDataVector[sliderId]->GetOutputPort());

	double bounds[6];
	if (mapperDataVector[sliderId]->IsFilePolyData())
	{
		vtkSmartPointer<vtkPolyData> polydata =
			vtkSmartPointer<vtkPolyData>::New();
		polydata = vtkPolyData::SafeDownCast(mapperDataVector[sliderId]->GetOutput());
		polydata->GetBounds(bounds);
	}
	else
	{
		vtkSmartPointer<vtkUnstructuredGrid> unstructuredgrid =
			vtkSmartPointer<vtkUnstructuredGrid>::New();
		unstructuredgrid = vtkUnstructuredGrid::SafeDownCast(mapperDataVector[sliderId]->GetOutput());
		unstructuredgrid->GetBounds(bounds);
	}

	int spinvalueX = this->verticalSliderX->value();
	double valueX = spinvalueX * (abs(bounds[0]) + abs(bounds[1])) / 100;

	int spinvalueY = this->verticalSliderY->value();
	double valueY = spinvalueY * (abs(bounds[2]) + abs(bounds[3])) / 100;

	int spinvalueZ = this->verticalSliderZ->value();
	double valueZ = spinvalueZ * (abs(bounds[4]) + abs(bounds[5])) / 100;

	// Create a plane to cut,here it cuts in the XZ direction (xz normal=(1,0,0);XY =(0,0,1),YZ =(0,1,0)
	vtkSmartPointer<vtkPlane> planeX =
		vtkSmartPointer<vtkPlane>::New();
	planeX->SetOrigin(0,(bounds[2]+bounds[3])*spinvalueX/100.0, 0);
	planeX->SetNormal(0, 1, 0);

	vtkSmartPointer<vtkPlane> planeY =
		vtkSmartPointer<vtkPlane>::New();
	planeY->SetOrigin((bounds[0] + bounds[1])*spinvalueY / 100.0, 0, 0);
	planeY->SetNormal(1, 0, 0);

	vtkSmartPointer<vtkPlane> planeZ =
		vtkSmartPointer<vtkPlane>::New();
	planeZ->SetOrigin(0, 0, (bounds[4] + bounds[5])*spinvalueZ / 100.0);
	planeZ->SetNormal(0, 0, 1);

	// Create cutter
	vtkSmartPointer<vtkCutter> cutterX =
		vtkSmartPointer<vtkCutter>::New();
	cutterX->SetCutFunction(planeX);
	cutterX->SetInputConnection(mapperDataVector[sliderId]->GetOutputPort());
	cutterX->Update();

	vtkSmartPointer<vtkCutter> cutterY =
		vtkSmartPointer<vtkCutter>::New();
	cutterY->SetCutFunction(planeY);
	cutterY->SetInputConnection(mapperDataVector[sliderId]->GetOutputPort());
	cutterY->Update();

	vtkSmartPointer<vtkCutter> cutterZ =
		vtkSmartPointer<vtkCutter>::New();
	cutterZ->SetCutFunction(planeZ);
	cutterZ->SetInputConnection(mapperDataVector[sliderId]->GetOutputPort());
	cutterZ->Update();

	vtkSmartPointer<vtkPolyDataMapper> cutterXMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	cutterXMapper->SetInputConnection(cutterX->GetOutputPort());

	vtkSmartPointer<vtkPolyDataMapper> cutterYMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	cutterYMapper->SetInputConnection(cutterY->GetOutputPort());

	vtkSmartPointer<vtkPolyDataMapper> cutterZMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	cutterZMapper->SetInputConnection(cutterZ->GetOutputPort());

	// Create plane actor
	vtkSmartPointer<vtkActor> planeXActor =
		vtkSmartPointer<vtkActor>::New();
	planeXActor->GetProperty()->SetColor(1, 1, 0);
	planeXActor->GetProperty()->SetLineWidth(2);
	planeXActor->SetMapper(cutterXMapper);

	vtkSmartPointer<vtkActor> planeYActor =
		vtkSmartPointer<vtkActor>::New();
	planeYActor->GetProperty()->SetColor(1, 0, 1);
	planeYActor->GetProperty()->SetLineWidth(2);
	planeYActor->SetMapper(cutterYMapper);

	vtkSmartPointer<vtkActor> planeZActor =
		vtkSmartPointer<vtkActor>::New();
	planeZActor->GetProperty()->SetColor(0, 1, 1);
	planeZActor->GetProperty()->SetLineWidth(2);
	planeZActor->SetMapper(cutterZMapper);

	vtkSmartPointer<vtkActor> actor =
		vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetOpacity(0.5);

	// VTK Renderer
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	if (this->checkBoxX->isChecked())
		renderer->AddActor(planeXActor);
	if (this->checkBoxY->isChecked())
		renderer->AddActor(planeYActor);
	if (this->checkBoxZ->isChecked())
		renderer->AddActor(planeZActor);
	renderer->AddActor(actor);

	vtkSmartPointer<vtkRenderer> rightRenderer =
		vtkSmartPointer<vtkRenderer>::New();
	// VTK/Qt wedded
	this->qvtkWidgetLeft->GetRenderWindow()->AddRenderer(renderer);

	this->qvtkWidgetLeft->update();

	cout << "QVTKWidget updated." << endl;
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
	this->comboBox->setEnabled(true);
	this->updateButton->setEnabled(true);

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
	this->comboBox->setEnabled(true);
	this->updateButton->setEnabled(true);

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

template <class T>
void MakeCellData(size_t const & tableSize, int arr_num, vtkSmartPointer<T> data, bool isPointData ,vtkLookupTable *lut, vtkUnsignedCharArray *colors)
{
	vtkSmartPointer<vtkFloatArray> retrievedArray;
	double arrayRange[2];
	if (isPointData)
	{
		data->GetPointData()->GetArray(arr_num)->GetRange(arrayRange);
		retrievedArray = vtkFloatArray::SafeDownCast(data->GetPointData()->GetArray(arr_num));
	}
	else
	{
		data->GetCellData()->GetArray(arr_num)->GetRange(arrayRange);
		retrievedArray = vtkFloatArray::SafeDownCast(data->GetCellData()->GetArray(arr_num));
	}

	double min = arrayRange[1], max = arrayRange[0];
	double scaleRange = max - min;
	 
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


void VTK_GUI_Qt::color_update()
{
	// Check if cell or point radiobutton was checked
	bool isPointData = false;

	if (this->radioButton->isChecked())
		isPointData = true;
		

	bool isPolyData = false;

	if (mapperDataVector[sliderId]->IsFilePolyData())
		isPolyData = true;

	vtkSmartPointer<vtkNamedColors> nc =
		vtkSmartPointer<vtkNamedColors>::New();

	QString arrName = this->comboBox->currentText();
	int arrNum = this->comboBox->itemData(this->comboBox->currentIndex()).toInt();

	cout << arrName.toStdString() << " " << arrNum << endl;

	// Create a lookup table to map cell data to colors
	vtkSmartPointer<vtkLookupTable> lookupTable =
		vtkSmartPointer<vtkLookupTable>::New();

	// Generate the colors for each point based on the color map
	vtkSmartPointer<vtkUnsignedCharArray> colorData =
		vtkSmartPointer<vtkUnsignedCharArray>::New();
	colorData->SetNumberOfComponents(3);
	colorData->SetName("Colors");

	vtkSmartPointer<vtkPolyData> polydata =
		vtkSmartPointer<vtkPolyData>::New();

	vtkSmartPointer<vtkUnstructuredGrid> unstructuredgrid =
		vtkSmartPointer<vtkUnstructuredGrid>::New();

	int tableSize;
	double arrayRange[2];

	if (isPolyData)
	{
		polydata = vtkPolyData::SafeDownCast(mapperDataVector[sliderId]->GetOutput());
		
		if (isPointData)
			polydata->GetPointData()->GetArray(arrNum)->GetRange(arrayRange);
		else
			polydata->GetCellData()->GetArray(arrNum)->GetRange(arrayRange);
		
		double scaleRange = arrayRange[1] - arrayRange[0];

		if (isPointData)
			tableSize = polydata->GetNumberOfPoints();
		else
			tableSize = polydata->GetNumberOfCells();
		std::cout << "Size: " << tableSize << std::endl;

		MakeLUTFromCTF(scaleRange, tableSize, lookupTable);
		MakeCellData<vtkPolyData>(tableSize, arrNum, polydata, isPointData, lookupTable, colorData);

		if (this->radioButton->isChecked() == true)
			polydata->GetPointData()->SetScalars(colorData);
		else
			polydata->GetCellData()->SetScalars(colorData);
	}
	else 
	{
		unstructuredgrid = vtkUnstructuredGrid::SafeDownCast(mapperDataVector[sliderId]->GetOutput());
		if (isPointData)
			unstructuredgrid->GetPointData()->GetArray(arrNum)->GetRange(arrayRange);
		else
			unstructuredgrid->GetCellData()->GetArray(arrNum)->GetRange(arrayRange);

		double scaleRange = arrayRange[1] - arrayRange[0];

		if (isPointData)
			tableSize = unstructuredgrid->GetNumberOfPoints();
		else
			tableSize = unstructuredgrid->GetNumberOfCells();
		std::cout << "Size: " << tableSize << std::endl;

		MakeLUTFromCTF(scaleRange, tableSize, lookupTable);
		MakeCellData<vtkUnstructuredGrid>(tableSize, arrNum, unstructuredgrid, isPointData, lookupTable, colorData);

		if (this->radioButton->isChecked() == true)
			unstructuredgrid->GetPointData()->SetScalars(colorData);
		else
			unstructuredgrid->GetCellData()->SetScalars(colorData);
	}

	cout << "Range of values for array" << arrayRange[0] << " " << arrayRange[1] << endl;

	//Create a mapper and actor
	vtkSmartPointer<vtkDataSetMapper> mapper =
		vtkSmartPointer<vtkDataSetMapper>::New();

	if (isPolyData)
		mapper->SetInputData(polydata);
	else
		mapper->SetInputData(unstructuredgrid);

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

		std::cout << "Output has " << polydata->GetNumberOfPoints() << " points." << std::endl;
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
	this->checkBoxX->setEnabled(true);
	this->checkBoxY->setEnabled(true);
	this->checkBoxZ->setEnabled(true);
	this->infoButton->setEnabled(true);

	//cout << "Data size: " << mapperDataVector.size() << endl;
	this->horizontalSlider->setMinimum(0);
	this->horizontalSlider->setMaximum(polydataVector.size() - 1);

	sliderId = this->horizontalSlider->value();
	cout << "Slider value: " << sliderId << endl;

	// Update display widget
	
	show_file();
}
