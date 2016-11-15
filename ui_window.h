/********************************************************************************
** Form generated from reading UI file 'window.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Window
{
public:
    QWidget *centralWidget;
    QTextEdit *messageField;
    QPushButton *clearButton;
    QPushButton *submitButton;
    QLabel *textLabel;
    QLabel *textLabel_2;
    QListWidget *clientList;
    QLineEdit *inputField;
    QCheckBox *checkBox;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *Window)
    {
        if (Window->objectName().isEmpty())
            Window->setObjectName(QStringLiteral("Window"));
        Window->resize(472, 371);
        centralWidget = new QWidget(Window);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        messageField = new QTextEdit(centralWidget);
        messageField->setObjectName(QStringLiteral("messageField"));
        messageField->setGeometry(QRect(30, 30, 251, 201));
        messageField->setFocusPolicy(Qt::StrongFocus);
        messageField->setReadOnly(true);
        clearButton = new QPushButton(centralWidget);
        clearButton->setObjectName(QStringLiteral("clearButton"));
        clearButton->setGeometry(QRect(300, 270, 141, 41));
        submitButton = new QPushButton(centralWidget);
        submitButton->setObjectName(QStringLiteral("submitButton"));
        submitButton->setGeometry(QRect(30, 270, 141, 41));
        submitButton->setAutoDefault(true);
        textLabel = new QLabel(centralWidget);
        textLabel->setObjectName(QStringLiteral("textLabel"));
        textLabel->setGeometry(QRect(30, 10, 61, 16));
        textLabel_2 = new QLabel(centralWidget);
        textLabel_2->setObjectName(QStringLiteral("textLabel_2"));
        textLabel_2->setGeometry(QRect(300, 10, 61, 16));
        clientList = new QListWidget(centralWidget);
        clientList->setObjectName(QStringLiteral("clientList"));
        clientList->setGeometry(QRect(300, 30, 141, 201));
        inputField = new QLineEdit(centralWidget);
        inputField->setObjectName(QStringLiteral("inputField"));
        inputField->setGeometry(QRect(30, 240, 251, 20));
        inputField->setClearButtonEnabled(false);
        checkBox = new QCheckBox(centralWidget);
        checkBox->setObjectName(QStringLiteral("checkBox"));
        checkBox->setGeometry(QRect(300, 240, 70, 17));
        Window->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(Window);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 472, 21));
        Window->setMenuBar(menuBar);
        mainToolBar = new QToolBar(Window);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        Window->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(Window);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        Window->setStatusBar(statusBar);
        QWidget::setTabOrder(inputField, submitButton);
        QWidget::setTabOrder(submitButton, clearButton);
        QWidget::setTabOrder(clearButton, messageField);
        QWidget::setTabOrder(messageField, clientList);

        retranslateUi(Window);
        QObject::connect(clearButton, SIGNAL(clicked()), inputField, SLOT(clear()));
        QObject::connect(inputField, SIGNAL(returnPressed()), submitButton, SLOT(click()));

        submitButton->setDefault(true);


        QMetaObject::connectSlotsByName(Window);
    } // setupUi

    void retranslateUi(QMainWindow *Window)
    {
        Window->setWindowTitle(QApplication::translate("Window", "Woof", 0));
        clearButton->setText(QApplication::translate("Window", "Clear", 0));
        submitButton->setText(QApplication::translate("Window", "Submit", 0));
        textLabel->setText(QApplication::translate("Window", "Messsages", 0));
        textLabel_2->setText(QApplication::translate("Window", "Clients", 0));
        checkBox->setText(QApplication::translate("Window", "Important", 0));
    } // retranslateUi

};

namespace Ui {
    class Window: public Ui_Window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WINDOW_H
