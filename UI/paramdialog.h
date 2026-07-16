

#ifndef PARAMDIALOG_H
#define PARAMDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QMap>
#include <QVector>
#include <QMessageBox>

class ParamDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ParamDialog(QWidget *parent = nullptr);
    ~ParamDialog();

    // Метод для получения всех параметров
    QMap<QString, QMap<QString, QString>> getParameters() const;

private slots:
    void onComboBoxChanged(int index);
    void onComboBox2Changed(int index);
    void onComboBox3Changed(int index);
    void onApplyButtonClicked();
    void onShowParametersClicked();

private:
    struct FieldInfo {
        QString fieldName;
        QString fieldType;
        QWidget* widget;
        QString structName;
    };

    void createStructFields();
    void clearRightPanel();
    void displayStructFields(const QString &structName);
    void collectAndSaveValues();
    void showAllParameters();

    QComboBox *comboBox;
    QComboBox *comboBox_2;
    QComboBox *comboBox_3;
    QWidget *rightContentWidget;
    QVBoxLayout *rightLayout;

    QString currentStructName;
    QMap<QString, QVector<FieldInfo>> structFields;

    // Хранилище всех параметров
    QMap<QString, QMap<QString, QString>> allParameters;
};

#endif // PARAMDIALOG_H
