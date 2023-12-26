#pragma once

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class SearchLineEditUi
{
public:
    QPushButton* btnSearch;

    void setupUi(QLineEdit* SearchLineEdit)
    {
        if (SearchLineEdit->objectName().isEmpty())
            SearchLineEdit->setObjectName("SearchLineEdit");
        SearchLineEdit->setStyleSheet(QString::fromUtf8("QLineEdit\n"
                                                        "{\n"
                                                        "border-radius:4px;\n"
                                                        "border:1px solid #e0e0e0;\n"
                                                        "padding:4px;\n"
                                                        "}\n"
                                                        "\n"
                                                        "QLineEdit:hover\n"
                                                        "{\n"
                                                        "border:1px solid #c0c0c0;\n"
                                                        "}\n"
                                                        "\n"
                                                        "QLineEdit:focus\n"
                                                        "{\n"
                                                        "border:1px solid #c0c0c0;\n"
                                                        "}\n"
                                                        "\n"));
        //        SearchLineEdit->resize(453, 48);
        btnSearch = new QPushButton(SearchLineEdit);
        btnSearch->setObjectName("btnSearch");
        //        btnSearch->setGeometry(QRect(410, 8, 32, 32));
        btnSearch->setStyleSheet(QString::fromUtf8(".QPushButton\n"
                                                   "{\n"
                                                   "border-radius:4px;\n"
                                                   "border:none;\n"
                                                   "padding:4px;\n"
                                                   "image:url(:/MainWindow/search.svg);\n"
                                                   "}"));
//        btnSearch->setStyleSheet(QString::fromUtf8(".QPushButton\n"
//                                                   "{\n"
//                                                   "border-radius:4px;\n"
//                                                   "border:none;\n"
//                                                   "padding:4px;\n"
//                                                   "image:url(:/MainWindow/search.svg);\n"
//                                                   "}"));

        retranslateUi(SearchLineEdit);

        QMetaObject::connectSlotsByName(SearchLineEdit);
    }  // setupUi

    void retranslateUi(QLineEdit* SearchLineEdit)
    {
        SearchLineEdit->setWindowTitle(QCoreApplication::translate("SearchLineEdit", "SearchLineEdit", nullptr));
        btnSearch->setText(QString());
    }  // retranslateUi
};

namespace Ui
{
class SearchLineEdit : public SearchLineEditUi
{
};
}  // namespace Ui

QT_END_NAMESPACE