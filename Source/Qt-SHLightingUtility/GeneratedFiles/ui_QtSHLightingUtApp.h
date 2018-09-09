/********************************************************************************
** Form generated from reading UI file 'QtSHLightingUtApp.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTSHLIGHTINGUTAPP_H
#define UI_QTSHLIGHTINGUTAPP_H

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

class Ui_MyUiWindow
{
public:
    QAction *actionAuthor;
    QAction *actionExit;
    QWidget *centralWidget;
    QWidget *renderCanvas;
    QLabel *label_envMapPath;
    QTextBrowser *textBrowser;
    QPushButton *btn_SelectCubeMap;
    QFrame *line;
    QLabel *label_MonteCarlo;
    QTextEdit *textEdit;
    QPushButton *btn_SelectSphericalMap;
    QTextEdit *textEdit_2;
    QLabel *label_SHOrder;
    QTextEdit *textEdit_3;
    QLabel *label_CalSHCoeff;
    QPushButton *btn_ComputeSH;
    QMenuBar *menuBar;
    QMenu *menu_File;
    QMenu *menu_Help;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MyUiWindow)
    {
        if (MyUiWindow->objectName().isEmpty())
            MyUiWindow->setObjectName(QStringLiteral("MyUiWindow"));
        MyUiWindow->resize(1200, 680);
        MyUiWindow->setMinimumSize(QSize(1200, 680));
        MyUiWindow->setMaximumSize(QSize(1200, 680));
        MyUiWindow->setWindowOpacity(1);
        actionAuthor = new QAction(MyUiWindow);
        actionAuthor->setObjectName(QStringLiteral("actionAuthor"));
        actionExit = new QAction(MyUiWindow);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        centralWidget = new QWidget(MyUiWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        renderCanvas = new QWidget(centralWidget);
        renderCanvas->setObjectName(QStringLiteral("renderCanvas"));
        renderCanvas->setGeometry(QRect(20, 20, 800, 600));
        QFont font;
        font.setPointSize(11);
        renderCanvas->setFont(font);
        label_envMapPath = new QLabel(centralWidget);
        label_envMapPath->setObjectName(QStringLiteral("label_envMapPath"));
        label_envMapPath->setGeometry(QRect(840, 20, 251, 31));
        QFont font1;
        font1.setFamily(QStringLiteral("Microsoft YaHei UI Light"));
        font1.setPointSize(10);
        font1.setBold(false);
        font1.setWeight(50);
        label_envMapPath->setFont(font1);
        textBrowser = new QTextBrowser(centralWidget);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));
        textBrowser->setGeometry(QRect(840, 50, 321, 21));
        QFont font2;
        font2.setFamily(QStringLiteral("Microsoft YaHei UI Light"));
        font2.setPointSize(8);
        textBrowser->setFont(font2);
        textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        btn_SelectCubeMap = new QPushButton(centralWidget);
        btn_SelectCubeMap->setObjectName(QStringLiteral("btn_SelectCubeMap"));
        btn_SelectCubeMap->setGeometry(QRect(840, 80, 151, 51));
        QFont font3;
        font3.setFamily(QStringLiteral("Microsoft YaHei UI Light"));
        font3.setPointSize(10);
        btn_SelectCubeMap->setFont(font3);
        line = new QFrame(centralWidget);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(820, 20, 20, 591));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);
        label_MonteCarlo = new QLabel(centralWidget);
        label_MonteCarlo->setObjectName(QStringLiteral("label_MonteCarlo"));
        label_MonteCarlo->setGeometry(QRect(840, 220, 141, 31));
        label_MonteCarlo->setFont(font1);
        textEdit = new QTextEdit(centralWidget);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setGeometry(QRect(840, 250, 181, 21));
        textEdit->setFont(font3);
        textEdit->setInputMethodHints(Qt::ImhDigitsOnly);
        textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        btn_SelectSphericalMap = new QPushButton(centralWidget);
        btn_SelectSphericalMap->setObjectName(QStringLiteral("btn_SelectSphericalMap"));
        btn_SelectSphericalMap->setGeometry(QRect(1010, 80, 151, 51));
        btn_SelectSphericalMap->setFont(font3);
        textEdit_2 = new QTextEdit(centralWidget);
        textEdit_2->setObjectName(QStringLiteral("textEdit_2"));
        textEdit_2->setGeometry(QRect(840, 180, 181, 21));
        textEdit_2->setFont(font3);
        textEdit_2->setInputMethodHints(Qt::ImhDigitsOnly);
        textEdit_2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textEdit_2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        label_SHOrder = new QLabel(centralWidget);
        label_SHOrder->setObjectName(QStringLiteral("label_SHOrder"));
        label_SHOrder->setGeometry(QRect(840, 150, 141, 31));
        label_SHOrder->setFont(font1);
        textEdit_3 = new QTextEdit(centralWidget);
        textEdit_3->setObjectName(QStringLiteral("textEdit_3"));
        textEdit_3->setGeometry(QRect(840, 320, 321, 241));
        textEdit_3->setFont(font3);
        textEdit_3->setInputMethodHints(Qt::ImhDigitsOnly|Qt::ImhMultiLine);
        textEdit_3->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textEdit_3->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        label_CalSHCoeff = new QLabel(centralWidget);
        label_CalSHCoeff->setObjectName(QStringLiteral("label_CalSHCoeff"));
        label_CalSHCoeff->setGeometry(QRect(840, 290, 141, 31));
        label_CalSHCoeff->setFont(font1);
        btn_ComputeSH = new QPushButton(centralWidget);
        btn_ComputeSH->setObjectName(QStringLiteral("btn_ComputeSH"));
        btn_ComputeSH->setGeometry(QRect(840, 570, 321, 41));
        btn_ComputeSH->setFont(font3);
        MyUiWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MyUiWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1200, 23));
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QStringLiteral("menu_File"));
        menu_Help = new QMenu(menuBar);
        menu_Help->setObjectName(QStringLiteral("menu_Help"));
        MyUiWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MyUiWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MyUiWindow->setStatusBar(statusBar);

        menuBar->addAction(menu_File->menuAction());
        menuBar->addAction(menu_Help->menuAction());
        menu_File->addAction(actionExit);
        menu_Help->addAction(actionAuthor);

        retranslateUi(MyUiWindow);
        QObject::connect(btn_SelectCubeMap, SIGNAL(clicked()), MyUiWindow, SLOT(update()));

        QMetaObject::connectSlotsByName(MyUiWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MyUiWindow)
    {
        MyUiWindow->setWindowTitle(QApplication::translate("MyUiWindow", "Spherical Harmonic Lighting Utility - sunhonglian", 0));
        actionAuthor->setText(QApplication::translate("MyUiWindow", "Author", 0));
        actionExit->setText(QApplication::translate("MyUiWindow", "Exit", 0));
        label_envMapPath->setText(QApplication::translate("MyUiWindow", "<html><head/><body><p>\347\216\257\345\242\203\350\264\264\345\233\276\346\226\207\344\273\266\350\267\257\345\276\204:</p></body></html>", 0));
#ifndef QT_NO_STATUSTIP
        textBrowser->setStatusTip(QString());
#endif // QT_NO_STATUSTIP
        textBrowser->setHtml(QApplication::translate("MyUiWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Microsoft YaHei UI Light'; font-size:8pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:9pt;\"><br /></p></body></html>", 0));
        btn_SelectCubeMap->setText(QApplication::translate("MyUiWindow", "\351\200\211\346\213\251Cube Map(.dds)", 0));
        label_MonteCarlo->setText(QApplication::translate("MyUiWindow", "<html><head/><body><p>\350\222\231\347\211\271\345\215\241\346\264\233\347\247\257\345\210\206\351\207\207\346\240\267\346\225\260\351\207\217:</p></body></html>", 0));
        textEdit->setHtml(QApplication::translate("MyUiWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Microsoft YaHei UI Light'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'SimSun'; font-size:9pt;\">10000</span></p></body></html>", 0));
        btn_SelectSphericalMap->setText(QApplication::translate("MyUiWindow", "\351\200\211\346\213\251\346\231\256\351\200\232\347\272\271\347\220\206(\347\220\203\351\235\242\346\230\240\345\260\204)", 0));
        textEdit_2->setHtml(QApplication::translate("MyUiWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Microsoft YaHei UI Light'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'SimSun'; font-size:9pt;\">3</span></p></body></html>", 0));
        label_SHOrder->setText(QApplication::translate("MyUiWindow", "<html><head/><body><p>\347\220\203\350\260\220\346\234\200\351\253\230\351\230\266\346\225\260(0~n):</p></body></html>", 0));
        textEdit_3->setHtml(QApplication::translate("MyUiWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Microsoft YaHei UI Light'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", 0));
        label_CalSHCoeff->setText(QApplication::translate("MyUiWindow", "<html><head/><body><p>\350\276\223\345\207\272\347\220\203\350\260\220\347\263\273\346\225\260:</p></body></html>", 0));
        btn_ComputeSH->setText(QApplication::translate("MyUiWindow", "\350\256\241\347\256\227SH\347\263\273\346\225\260", 0));
        menu_File->setTitle(QApplication::translate("MyUiWindow", "File", 0));
        menu_Help->setTitle(QApplication::translate("MyUiWindow", "Help", 0));
    } // retranslateUi

};

namespace Ui {
    class MyUiWindow: public Ui_MyUiWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTSHLIGHTINGUTAPP_H
