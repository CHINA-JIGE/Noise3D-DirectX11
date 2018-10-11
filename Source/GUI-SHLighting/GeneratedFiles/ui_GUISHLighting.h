/********************************************************************************
** Form generated from reading UI file 'GUISHLighting.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUISHLIGHTING_H
#define UI_GUISHLIGHTING_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SHLightingGuiApp
{
public:
    QAction *actionAbout;
    QAction *actionExit;
    QAction *actionCameraOrthographic;
    QAction *actionCameraPerspective;
    QAction *actionSaveSHFactors;
    QAction *actionLoadCubeMap;
    QAction *actionLoadCommonTexture;
    QWidget *centralwidget;
    QLabel *label_MonteCarlo;
    QFrame *line;
    QPushButton *btn_ComputeSH;
    QTextEdit *textEdit_ShCoefficient;
    QTextEdit *textEdit_shOrder;
    QTextEdit *textEdit_monteCarlo;
    QLabel *label_SHOrder;
    QLabel *label_CalSHCoeff;
    QPushButton *btn_CamOrtho;
    QPushButton *btn_CamPerspective;
    QLabel *label_camProjType;
    QPushButton *btn_SaveSHCoefficients;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *SHLightingGuiApp)
    {
        if (SHLightingGuiApp->objectName().isEmpty())
            SHLightingGuiApp->setObjectName(QStringLiteral("SHLightingGuiApp"));
        SHLightingGuiApp->resize(1200, 680);
        SHLightingGuiApp->setMaximumSize(QSize(1200, 680));
        actionAbout = new QAction(SHLightingGuiApp);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionExit = new QAction(SHLightingGuiApp);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionCameraOrthographic = new QAction(SHLightingGuiApp);
        actionCameraOrthographic->setObjectName(QStringLiteral("actionCameraOrthographic"));
        actionCameraPerspective = new QAction(SHLightingGuiApp);
        actionCameraPerspective->setObjectName(QStringLiteral("actionCameraPerspective"));
        actionSaveSHFactors = new QAction(SHLightingGuiApp);
        actionSaveSHFactors->setObjectName(QStringLiteral("actionSaveSHFactors"));
        actionLoadCubeMap = new QAction(SHLightingGuiApp);
        actionLoadCubeMap->setObjectName(QStringLiteral("actionLoadCubeMap"));
        actionLoadCommonTexture = new QAction(SHLightingGuiApp);
        actionLoadCommonTexture->setObjectName(QStringLiteral("actionLoadCommonTexture"));
        centralwidget = new QWidget(SHLightingGuiApp);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        label_MonteCarlo = new QLabel(centralwidget);
        label_MonteCarlo->setObjectName(QStringLiteral("label_MonteCarlo"));
        label_MonteCarlo->setGeometry(QRect(850, 180, 141, 31));
        QFont font;
        font.setFamily(QStringLiteral("Microsoft YaHei UI Light"));
        font.setPointSize(10);
        font.setBold(false);
        font.setWeight(50);
        label_MonteCarlo->setFont(font);
        line = new QFrame(centralwidget);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(820, 20, 20, 601));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);
        btn_ComputeSH = new QPushButton(centralwidget);
        btn_ComputeSH->setObjectName(QStringLiteral("btn_ComputeSH"));
        btn_ComputeSH->setGeometry(QRect(850, 280, 161, 51));
        QFont font1;
        font1.setFamily(QStringLiteral("Microsoft YaHei UI Light"));
        font1.setPointSize(10);
        btn_ComputeSH->setFont(font1);
        textEdit_ShCoefficient = new QTextEdit(centralwidget);
        textEdit_ShCoefficient->setObjectName(QStringLiteral("textEdit_ShCoefficient"));
        textEdit_ShCoefficient->setGeometry(QRect(850, 340, 331, 291));
        textEdit_ShCoefficient->setFont(font1);
        textEdit_ShCoefficient->setInputMethodHints(Qt::ImhDigitsOnly|Qt::ImhMultiLine);
        textEdit_ShCoefficient->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textEdit_ShCoefficient->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textEdit_shOrder = new QTextEdit(centralwidget);
        textEdit_shOrder->setObjectName(QStringLiteral("textEdit_shOrder"));
        textEdit_shOrder->setGeometry(QRect(850, 150, 181, 21));
        textEdit_shOrder->setFont(font1);
        textEdit_shOrder->setInputMethodHints(Qt::ImhDigitsOnly);
        textEdit_shOrder->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textEdit_shOrder->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textEdit_monteCarlo = new QTextEdit(centralwidget);
        textEdit_monteCarlo->setObjectName(QStringLiteral("textEdit_monteCarlo"));
        textEdit_monteCarlo->setGeometry(QRect(850, 210, 181, 21));
        textEdit_monteCarlo->setFont(font1);
        textEdit_monteCarlo->setInputMethodHints(Qt::ImhDigitsOnly);
        textEdit_monteCarlo->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textEdit_monteCarlo->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        label_SHOrder = new QLabel(centralwidget);
        label_SHOrder->setObjectName(QStringLiteral("label_SHOrder"));
        label_SHOrder->setGeometry(QRect(850, 120, 141, 31));
        label_SHOrder->setFont(font);
        label_CalSHCoeff = new QLabel(centralwidget);
        label_CalSHCoeff->setObjectName(QStringLiteral("label_CalSHCoeff"));
        label_CalSHCoeff->setGeometry(QRect(850, 250, 141, 31));
        label_CalSHCoeff->setFont(font);
        btn_CamOrtho = new QPushButton(centralwidget);
        btn_CamOrtho->setObjectName(QStringLiteral("btn_CamOrtho"));
        btn_CamOrtho->setGeometry(QRect(1030, 50, 141, 51));
        btn_CamPerspective = new QPushButton(centralwidget);
        btn_CamPerspective->setObjectName(QStringLiteral("btn_CamPerspective"));
        btn_CamPerspective->setGeometry(QRect(860, 50, 151, 51));
        label_camProjType = new QLabel(centralwidget);
        label_camProjType->setObjectName(QStringLiteral("label_camProjType"));
        label_camProjType->setGeometry(QRect(850, 20, 141, 31));
        label_camProjType->setFont(font);
        btn_SaveSHCoefficients = new QPushButton(centralwidget);
        btn_SaveSHCoefficients->setObjectName(QStringLiteral("btn_SaveSHCoefficients"));
        btn_SaveSHCoefficients->setGeometry(QRect(1020, 280, 161, 51));
        btn_SaveSHCoefficients->setFont(font1);
        SHLightingGuiApp->setCentralWidget(centralwidget);
        menubar = new QMenuBar(SHLightingGuiApp);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 1200, 23));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        SHLightingGuiApp->setMenuBar(menubar);
        statusbar = new QStatusBar(SHLightingGuiApp);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        SHLightingGuiApp->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionLoadCommonTexture);
        menuFile->addAction(actionLoadCubeMap);
        menuFile->addAction(actionExit);
        menuHelp->addAction(actionAbout);

        retranslateUi(SHLightingGuiApp);

        QMetaObject::connectSlotsByName(SHLightingGuiApp);
    } // setupUi

    void retranslateUi(QMainWindow *SHLightingGuiApp)
    {
        SHLightingGuiApp->setWindowTitle(QApplication::translate("SHLightingGuiApp", "Spherical Harmonic Lighting Utility - sunhonglian", 0));
        actionAbout->setText(QApplication::translate("SHLightingGuiApp", "About", 0));
        actionExit->setText(QApplication::translate("SHLightingGuiApp", "Exit", 0));
        actionCameraOrthographic->setText(QApplication::translate("SHLightingGuiApp", "Orthographic", 0));
        actionCameraPerspective->setText(QApplication::translate("SHLightingGuiApp", "Perspective", 0));
        actionSaveSHFactors->setText(QApplication::translate("SHLightingGuiApp", "Save SH Coefficients to Files", 0));
        actionLoadCubeMap->setText(QApplication::translate("SHLightingGuiApp", "Load Cube Map", 0));
        actionLoadCommonTexture->setText(QApplication::translate("SHLightingGuiApp", "Load 2D Texture", 0));
        label_MonteCarlo->setText(QApplication::translate("SHLightingGuiApp", "<html><head/><body><p>\350\222\231\347\211\271\345\215\241\346\264\233\347\247\257\345\210\206\351\207\207\346\240\267\346\225\260\351\207\217:</p></body></html>", 0));
        btn_ComputeSH->setText(QApplication::translate("SHLightingGuiApp", "\350\256\241\347\256\227SH\347\263\273\346\225\260", 0));
        textEdit_ShCoefficient->setHtml(QApplication::translate("SHLightingGuiApp", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Microsoft YaHei UI Light'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", 0));
        textEdit_shOrder->setHtml(QApplication::translate("SHLightingGuiApp", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Microsoft YaHei UI Light'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'SimSun'; font-size:9pt;\">3</span></p></body></html>", 0));
        textEdit_monteCarlo->setHtml(QApplication::translate("SHLightingGuiApp", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Microsoft YaHei UI Light'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'SimSun'; font-size:9pt;\">10000</span></p></body></html>", 0));
        label_SHOrder->setText(QApplication::translate("SHLightingGuiApp", "<html><head/><body><p>\347\220\203\350\260\220\346\234\200\351\253\230\351\230\266\346\225\260(0~n):</p></body></html>", 0));
        label_CalSHCoeff->setText(QApplication::translate("SHLightingGuiApp", "<html><head/><body><p>\350\276\223\345\207\272\347\220\203\350\260\220\347\263\273\346\225\260:</p></body></html>", 0));
        btn_CamOrtho->setText(QApplication::translate("SHLightingGuiApp", "\345\210\207\346\215\242\345\210\260\346\255\243\344\272\244\346\212\225\345\275\261", 0));
        btn_CamPerspective->setText(QApplication::translate("SHLightingGuiApp", "\345\210\207\346\215\242\345\210\260\351\200\217\350\247\206\346\212\225\345\275\261", 0));
        label_camProjType->setText(QApplication::translate("SHLightingGuiApp", "<html><head/><body><p>\346\221\204\345\203\217\346\234\272\346\212\225\345\275\261\347\261\273\345\236\213:</p></body></html>", 0));
        btn_SaveSHCoefficients->setText(QApplication::translate("SHLightingGuiApp", "\344\277\235\345\255\230SH\347\263\273\346\225\260\345\210\260\346\226\207\344\273\266", 0));
        menuFile->setTitle(QApplication::translate("SHLightingGuiApp", "File", 0));
        menuHelp->setTitle(QApplication::translate("SHLightingGuiApp", "Help", 0));
    } // retranslateUi

};

namespace Ui {
    class SHLightingGuiApp: public Ui_SHLightingGuiApp {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUISHLIGHTING_H
