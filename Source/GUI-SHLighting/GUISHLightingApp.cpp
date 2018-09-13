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
	QString output;
	output += "Channel R:";
	for (int i = 0; i < shVector.size(); ++i)output += QString::number(shVector.at(i).x) + QString(" ")+ QString("\n");
	output += "Channel G:";
	for (int i = 0; i < shVector.size(); ++i)output += QString::number(shVector.at(i).y) + QString(" ") + QString("\n");
	output += "Channel B:";
	for (int i = 0; i < shVector.size(); ++i)output += QString::number(shVector.at(i).z) + QString(" ") + QString("\n");

	mUI.textEdit_ShCoefficient->setText(output);
}

/***********************************************

						Override Events

***************************************************/
