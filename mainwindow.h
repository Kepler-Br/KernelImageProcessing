#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "convmatfactory.h"
#include "matrix.h"
#include "imageworker.h"


enum LogLevel
{
    Info = 0, Warning, Error, Critical
};


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void processingFinished();

private slots:
    void writeLogToViewer(const QString &text, const LogLevel level = LogLevel::Info);

    void on_actionExit_triggered();
    void on_actionOpen_triggered();
    void on_actionOpen_matrix_triggered();
    void on_actionSave_matrix_triggered();
    void on_preset_comboBox_currentIndexChanged(const QString &arg1);
    void on_genMatrix_button_clicked();
    void on_doit_button_clicked();
    void on_actionSave_image_triggered();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    MatAlg::Matrix<double> filter;
    QString targetPath = "./out.bmp";
    QString imagePath = "";
    ImageWorker imgWorker;

    void updateTable();
    void updateMatrix();

};

#endif // MAINWINDOW_H
