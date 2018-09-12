/********************************************************************************
** Form generated from reading UI file 'Widget_RenderCanvas.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_RENDERCANVAS_H
#define UI_WIDGET_RENDERCANVAS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget_RenderCanvas
{
public:

    void setupUi(QWidget *Widget_RenderCanvas)
    {
        if (Widget_RenderCanvas->objectName().isEmpty())
            Widget_RenderCanvas->setObjectName(QStringLiteral("Widget_RenderCanvas"));
        Widget_RenderCanvas->resize(400, 300);

        retranslateUi(Widget_RenderCanvas);

        QMetaObject::connectSlotsByName(Widget_RenderCanvas);
    } // setupUi

    void retranslateUi(QWidget *Widget_RenderCanvas)
    {
        Widget_RenderCanvas->setWindowTitle(QApplication::translate("Widget_RenderCanvas", "Widget_RenderCanvas", 0));
    } // retranslateUi

};

namespace Ui {
    class Widget_RenderCanvas: public Ui_Widget_RenderCanvas {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_RENDERCANVAS_H
