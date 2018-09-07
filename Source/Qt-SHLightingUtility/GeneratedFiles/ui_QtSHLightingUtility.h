/********************************************************************************
** Form generated from reading UI file 'QtSHLightingUtility.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTSHLIGHTINGUTILITY_H
#define UI_QTSHLIGHTINGUTILITY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtSHLightingUtilityClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtSHLightingUtilityClass)
    {
        if (QtSHLightingUtilityClass->objectName().isEmpty())
            QtSHLightingUtilityClass->setObjectName(QStringLiteral("QtSHLightingUtilityClass"));
        QtSHLightingUtilityClass->resize(600, 400);
        menuBar = new QMenuBar(QtSHLightingUtilityClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        QtSHLightingUtilityClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtSHLightingUtilityClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        QtSHLightingUtilityClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(QtSHLightingUtilityClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        QtSHLightingUtilityClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(QtSHLightingUtilityClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        QtSHLightingUtilityClass->setStatusBar(statusBar);

        retranslateUi(QtSHLightingUtilityClass);

        QMetaObject::connectSlotsByName(QtSHLightingUtilityClass);
    } // setupUi

    void retranslateUi(QMainWindow *QtSHLightingUtilityClass)
    {
        QtSHLightingUtilityClass->setWindowTitle(QApplication::translate("QtSHLightingUtilityClass", "QtSHLightingUtility", 0));
    } // retranslateUi

};

namespace Ui {
    class QtSHLightingUtilityClass: public Ui_QtSHLightingUtilityClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTSHLIGHTINGUTILITY_H
