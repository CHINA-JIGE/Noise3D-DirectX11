#include "GUISHLightingApp.h"

using namespace Noise3D;

GUISHLightingApp::GUISHLightingApp(QWidget *parent)
	: QMainWindow(parent)
{
	mUI.setupUi(this);
	m_pRenderCanvas = new Widget_RenderCanvas(mUI.centralwidget);
	m_pRenderCanvas->Initialize(QRect(20, 20, 800, 600));

	//"register "signal&slot (must after setupUI)
	connect(mUI.btn_SelectSphericalMap, &QPushButton::clicked, this, &GUISHLightingApp::Slot_LoadSphericalTexture);
	connect(mUI.actionExit, &QAction::triggered, this, &GUISHLightingApp::Slot_Menu_Exit);
	connect(mUI.actionAbout, &QAction::triggered, this, &GUISHLightingApp::Slot_Menu_About);
	connect(mUI.btn_ComputeSH, &QPushButton::clicked, this, &GUISHLightingApp::Slot_ComputeShCoefficient);
	connect(mUI.actionCameraOrthographic, &QAction::triggered, this, &GUISHLightingApp::Slot_CameraProj_Ortho);
	connect(mUI.actionCameraPerspective, &QAction::triggered, this, &GUISHLightingApp::Slot_CameraProj_Perspective);

	//init text codec(to support Chinese?)
	QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
}


GUISHLightingApp::~GUISHLightingApp()
{

}

void GUISHLightingApp::Slot_Menu_About()
{
	QMessageBox::information(this, tr(u8"关于"), tr(u8"作者：sunhonglian"));
}

void GUISHLightingApp::Slot_Menu_Exit()
{
	QApplication::exit();
}

void GUISHLightingApp::Slot_LoadSphericalTexture()
{
	mTextureType = SH_TEXTURE_TYPE::COMMON;

	//file dialog
	QFileDialog *fileDialog = new QFileDialog(this);
	fileDialog->setWindowTitle(tr(u8"打开图片"));
	fileDialog->setDirectory(u8"./");//default directory
	fileDialog->setNameFilter(tr(u8"图像(*.png *.jpg *.jpeg *.bmp *.tga *.gif)"));//file format filter
	fileDialog->setFileMode(QFileDialog::FileMode::ExistingFile);//select what kind of object, file/files/dir??
	fileDialog->setViewMode(QFileDialog::ViewMode::Detail);

	//open the file dialog
	QStringList fileNames;
	if (fileDialog->exec()==QFileDialog::Accepted)
	{
		fileNames = fileDialog->selectedFiles();
	}

	//check if user successfully selected a file
	if (fileNames.isEmpty())
	{
		QMessageBox::information(this, tr(u8"错误"), tr(u8"请选择一个图像文件！"));
	}
	else
	{
		//store file path info
		mFilePath = std::string(fileNames.at(0).toStdString());
		mUI.textBrowser_filePath->setText(fileNames.at(0));
		bool loadSucceeded = m_pRenderCanvas->GetMain3dApp().LoadOriginalTexture(mFilePath);
		if (!loadSucceeded)
		{
			QMessageBox::information(this, tr(u8"错误"), tr(u8"纹理加载失败！"));
		}
	}
}

void GUISHLightingApp::Slot_LoadCubeMap()
{
	mTextureType = SH_TEXTURE_TYPE::CUBE_MAP;

	//file dialog
	QFileDialog *fileDialog = new QFileDialog(this);
	fileDialog->setWindowTitle(tr(u8"打开图片"));
	fileDialog->setDirectory(u8"./");//default directory
	fileDialog->setNameFilter(tr(u8"CubeMap图片格式(*.dds)"));//file format filter
	fileDialog->setFileMode(QFileDialog::FileMode::ExistingFile);//select what kind of object, file/files/dir??
	fileDialog->setViewMode(QFileDialog::ViewMode::Detail);

	//open the file dialog
	QStringList fileNames;
	if (fileDialog->exec() == QFileDialog::Accepted)
	{
		fileNames = fileDialog->selectedFiles();
	}

	//check if user successfully selected a file
	if (fileNames.isEmpty())
	{
		QMessageBox::information(this, tr(u8"错误"), tr(u8"请选择一个图像文件！"));
	}
	else
	{
		//store file path info
		mFilePath = std::string(fileNames.at(0).toStdString());
		mUI.textBrowser_filePath->setText(fileNames.at(0));
		bool loadSucceeded = m_pRenderCanvas->GetMain3dApp().LoadOriginalTexture(mFilePath);
		if (!loadSucceeded)
		{
			QMessageBox::information(this, tr(u8"错误"), tr(u8"纹理加载失败！"));
		}
	}
}

void GUISHLightingApp::Slot_ComputeShCoefficient()
{
	//get param from GUI text edit
	int shOrder =  mUI.textEdit_shOrder->toPlainText().toInt();
	int monteCarloSampleCount = mUI.textEdit_monteCarlo->toPlainText().toInt();

	//compute SH texture
	std::vector<NVECTOR3> shVector;
	m_pRenderCanvas->GetMain3dApp().ComputeShTexture(mTextureType, shOrder, monteCarloSampleCount, shVector);

	//output to text edit in given format
	QString output;
	output += "<font color=red><b>Channel R:</b></font><font color=black><br/>";
	for (int L = 0; L <= shOrder; ++L)
	{
		output += QString("<b>Band ")+QString::number(L)+QString(": </b><br/>");
		for (int M = -L; M <= L; ++M)
		{
			output += QString::number(shVector.at(GI::SH_FlattenIndex(L, M)).x) + QString(" ");
		}
		output += QString("<br/>");
	}

	output += "<font color=Green><b>Channel G:</b></font><font color=black><br/>";
	for (int L = 0; L <= shOrder; ++L)
	{
		output += QString("<b>Band ") + QString::number(L) + QString(": </b><br/>");
		for (int M = -L; M <= L; ++M)
		{
			output += QString::number(shVector.at(GI::SH_FlattenIndex(L, M)).y) + QString(" ");
		}
		output += QString("<br/>");
	}

	output += "<font color=Blue><b>Channel B:</b></font><font color=black><br/>";
	for (int L = 0; L <= shOrder; ++L)
	{
		output += QString("<b>Band ") + QString::number(L) + QString(": </b><br/>");
		for (int M = -L; M <= L; ++M)
		{
			output += QString::number(shVector.at(GI::SH_FlattenIndex(L, M)).z) + QString(" ");
		}
		output += QString("<br/>");
	}

	mUI.textEdit_ShCoefficient->setText(output);
}

void GUISHLightingApp::Slot_CameraProj_Ortho()
{
	m_pRenderCanvas->GetMain3dApp().SetCamProjType(false);
}

void GUISHLightingApp::Slot_CameraProj_Perspective()
{
	m_pRenderCanvas->GetMain3dApp().SetCamProjType(true);
}


/***********************************************

						Override Events

***************************************************/
