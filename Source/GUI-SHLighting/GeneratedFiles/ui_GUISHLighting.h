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
#include <QtWidgets/QTextBrowser>
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
    QWidget *centralwidget;
    QTextBrowser *textBrowser_filePath;
    QLabel *label_MonteCarlo;
    QPushButton *btn_SelectSphericalMap;
    QFrame *line;
    QPushButton *btn_ComputeSH;
    QLabel *label_envMapPath;
    QTextEdit *textEdit_ShCoefficient;
    QTextEdit *textEdit_shOrder;
    QTextEdit *textEdit_monteCarlo;
    QLabel *label_SHOrder;
    QPushButton *btn_SelectCubeMap;
    QLabel *label_CalSHCoeff;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QMenu *menuCamera;
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
        centralwidget = new QWidget(SHLightingGuiApp);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        textBrowser_filePath = new QTextBrowser(centralwidget);
        textBrowser_filePath->setObjectName(QStringLiteral("textBrowser_filePath"));
        textBrowser_filePath->setGeometry(QRect(840, 50, 321, 21));
        QFont font;
        font.setFamily(QStringLiteral("Microsoft YaHei UI Light"));
        font.setPointSize(8);
        textBrowser_filePath->setFont(font);
        textBrowser_filePath->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textBrowser_filePath->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        label_MonteCarlo = new QLabel(centralwidget);
        label_MonteCarlo->setObjectName(QStringLiteral("label_MonteCarlo"));
        label_MonteCarlo->setGeometry(QRect(840, 220, 141, 31));
        QFont font1;
        font1.setFamily(QStringLiteral("Microsoft YaHei UI Light"));
        font1.setPointSize(10);
        font1.setBold(false);
        font1.setWeight(50);
        label_MonteCarlo->setFont(font1);
        btn_SelectSphericalMap = new QPushButton(centralwidget);
        btn_SelectSphericalMap->setObjectName(QStringLiteral("btn_SelectSphericalMap"));
        btn_SelectSphericalMap->setGeometry(QRect(1010, 80, 151, 51));
        QFont font2;
        font2.setFamily(QStringLiteral("Microsoft YaHei UI Light"));
        font2.setPointSize(10);
        btn_SelectSphericalMap->setFont(font2);
        line = new QFrame(centralwidget);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(820, 20, 20, 591));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);
        btn_ComputeSH = new QPushButton(centralwidget);
        btn_ComputeSH->setObjectName(QStringLiteral("btn_ComputeSH"));
        btn_ComputeSH->setGeometry(QRect(840, 570, 321, 41));
        btn_ComputeSH->setFont(font2);
        label_envMapPath = new QLabel(centralwidget);
        label_envMapPath->setObjectName(QStringLiteral("label_envMapPath"));
        label_envMapPath->setGeometry(QRect(840, 20, 251, 31));
        label_envMapPath->setFont(font1);
        textEdit_ShCoefficient = new QTextEdit(centralwidget);
        textEdit_ShCoefficient->setObjectName(QStringLiteral("textEdit_ShCoefficient"));
        textEdit_ShCoefficient->setGeometry(QRect(840, 320, 321, 241));
        textEdit_ShCoefficient->setFont(font2);
        textEdit_ShCoefficient->setInputMethodHints(Qt::ImhDigitsOnly|Qt::ImhMultiLine);
        textEdit_ShCoefficient->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textEdit_ShCoefficient->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textEdit_shOrder = new QTextEdit(centralwidget);
        textEdit_shOrder->setObjectName(QStringLiteral("textEdit_shOrder"));
        textEdit_shOrder->setGeometry(QRect(840, 180, 181, 21));
        textEdit_shOrder->setFont(font2);
        textEdit_shOrder->setInputMethodHints(Qt::ImhDigitsOnly);
        textEdit_shOrder->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textEdit_shOrder->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textEdit_monteCarlo = new QTextEdit(centralwidget);
        textEdit_monteCarlo->setObjectName(QStringLiteral("textEdit_monteCarlo"));
        textEdit_monteCarlo->setGeometry(QRect(840, 250, 181, 21));
        textEdit_monteCarlo->setFont(font2);
        textEdit_monteCarlo->setInputMethodHints(Qt::ImhDigitsOnly);
        textEdit_monteCarlo->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textEdit_monteCarlo->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        label_SHOrder = new QLabel(centralwidget);
        label_SHOrder->setObjectName(QStringLiteral("label_SHOrder"));
        label_SHOrder->setGeometry(QRect(840, 150, 141, 31));
        label_SHOrder->setFont(font1);
        btn_SelectCubeMap = new QPushButton(centralwidget);
        btn_SelectCubeMap->setObjectName(QStringLiteral("btn_SelectCubeMap"));
        btn_SelectCubeMap->setGeometry(QRect(840, 80, 151, 51));
        btn_SelectCubeMap->setFont(font2);
        label_CalSHCoeff = new QLabel(centralwidget);
        label_CalSHCoeff->setObjectName(QStringLiteral("label_CalSHCoeff"));
        label_CalSHCoeff->setGeometry(QRect(840, 290, 141, 31));
        label_CalSHCoeff->setFont(font1);
        SHLightingGuiApp->setCentralWidget(centralwidget);
        menubar = new QMenuBar(SHLightingGuiApp);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 1200, 23));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        menuCamera = new QMenu(menubar);
        menuCamera->setObjectName(QStringLiteral("menuCamera"));
        SHLightingGuiApp->setMenuBar(menubar);
        statusbar = new QStatusBar(SHLightingGuiApp);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        SHLightingGuiApp->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuCamera->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionExit);
        menuHelp->addAction(actionAbout);
        menuCamera->addAction(actionCameraOrthographic);
        menuCamera->addAction(actionCameraPerspective);

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
#ifndef QT_NO_STATUSTIP
        textBrowser_filePath->setStatusTip(QString());
#endif // QT_NO_STATUSTIP
        textBrowser_filePath->setHtml(QApplication::translate("SHLightingGuiApp", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Microsoft YaHei UI Light'; font-size:8pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:9pt;\"><br /></p></body></html>", 0));
        label_MonteCarlo->setText(QApplication::translate("SHLightingGuiApp", "<html><head/><body><p>\350\222\231\347\211\271\345\215\241\346\264\233\347\247\257\345\210\206\351\207\207\346\240\267\346\225\260\351\207\217:</p></body></html>", 0));
        btn_SelectSphericalMap->setText(QApplication::translate("SHLightingGuiApp", "\351\200\211\346\213\251\346\231\256\351\200\232\347\272\271\347\220\206(\347\220\203\351\235\242\346\230\240\345\260\204)", 0));
        btn_ComputeSH->setText(QApplication::translate("SHLightingGuiApp", "\350\256\241\347\256\227SH\347\263\273\346\225\260", 0));
        label_envMapPath->setText(QApplication::translate("SHLightingGuiApp", "<html><head/><body><p>\347\216\257\345\242\203\350\264\264\345\233\276\346\226\207\344\273\266\350\267\257\345\276\204:</p></body></html>", 0));
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
        btn_SelectCubeMap->setText(QApplication::translate("SHLightingGuiApp", "\351\200\211\346\213\251Cube Map(.dds)", 0));
        label_CalSHCoeff->setText(QApplication::translate("SHLightingGuiApp", "<html><head/><body><p>\350\276\223\345\207\272\347\220\203\350\260\220\347\263\273\346\225\260:</p></body></html>", 0));
        menuFile->setTitle(QApplication::translate("SHLightingGuiApp", "File", 0));
        menuHelp->setTitle(QApplication::translate("SHLightingGuiApp", "Help", 0));
        menuCamera->setTitle(QApplication::translate("SHLightingGuiApp", "Camera", 0));
    } // retranslateUi

};

namespace Ui {
    class SHLightingGuiApp: public Ui_SHLightingGuiApp {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUISHLIGHTING_H
