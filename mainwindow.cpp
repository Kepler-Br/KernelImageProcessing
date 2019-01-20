#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imageprocessor.h"
#include "matrixmisc.h"
#include <QFileDialog>
#include <QThread>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    filter(3, 3, 0.0)
{
    ui->setupUi(this);
    emit this->writeLogToViewer(tr("Logging goes here."), LogLevel::Info);

    unsigned concurentThreadsSupported = std::thread::hardware_concurrency();
    if(concurentThreadsSupported == 0)
    {
        concurentThreadsSupported = 2;
        emit this->writeLogToViewer(tr("Cannot detect avalible thread cound. Using 2."), LogLevel::Warning);
    }
    else
        emit this->writeLogToViewer(tr("Setting threads to ") +
                                    QString::number(concurentThreadsSupported) +
                                    tr("."), LogLevel::Info);
    ui->thread_spinBox->setValue(int(concurentThreadsSupported));
    updateMatrix();
    updateTable();
    QObject::connect(&imgWorker, SIGNAL(finished()), this, SLOT(processingFinished()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::processingFinished()
{
    ui->doit_button->setEnabled(true);
    QImage img = imgWorker.getProcessedImage();
    if(!img.save(targetPath, "PNG"))
    {
        emit this->writeLogToViewer(tr("Cannot save image."), LogLevel::Error);
        return;
    }
    emit this->writeLogToViewer(tr("Done."), LogLevel::Info);

}

void MainWindow::on_actionExit_triggered()
{
    QApplication::exit(0);
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open image"), "",
                                                    tr("Image (*.bmp *.jpg *.jpeg *.tiff *.png);;All Files (*)"));
    if(fileName.length() == 0)
    {
        emit writeLogToViewer("Target image was not set.", LogLevel::Info);
        return;
    }

    imagePath = fileName;

    emit writeLogToViewer("Target image set.", LogLevel::Info);

}

void MainWindow::writeLogToViewer(const QString &text, const LogLevel level)
{
    QString logLevelText = "";
    switch(level)
    {
    case LogLevel::Critical:
        logLevelText = "<b>CRITICAL</b>";
        break;
    case LogLevel::Error:
        logLevelText = "<b>Error</b>";
        break;
    case LogLevel::Info:
        logLevelText = "<b>Info</b>";
        break;
    case LogLevel::Warning:
        logLevelText = "<b>Warning</b>";
        break;
    }
    ui->logview->append(logLevelText + tr(": ") + text);
}

void MainWindow::on_actionOpen_matrix_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open matrix"), "",
                                                    tr("Text file (*.txt);;All Files (*)"));
    if(fileName.length() == 0)
        return;

    try
    {
        filter = MatAlg::loadMatrix<double>(fileName.toUtf8().constData());
    }
    catch (const std::logic_error &e)
    {
        emit writeLogToViewer(tr("Error while loading matrix: ") + tr(e.what()), LogLevel::Error);
        return;
    }
    uint filterRows = filter.getRowSize();
    uint filterColumn = filter.getColumnSize();
    ui->column_spinBox->setValue(int(filterColumn));
    ui->row_spinBox->setValue(int(filterRows));
    ui->preset_comboBox->setCurrentIndex(6);
    updateTable();
    emit writeLogToViewer("Loaded matrix successfully.", LogLevel::Info);
}

void MainWindow::on_actionSave_matrix_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save matrix"), "",
                                                    tr("Text file (*.txt);;All Files (*)"));
    if(fileName.length() == 0)
    {
        emit writeLogToViewer("Matrix was not saved.", LogLevel::Info);
        return;
    }
    updateMatrix();

    MatAlg::saveMatrix(fileName.toUtf8().constData(), filter);

    emit writeLogToViewer("Saved matrix successfully.", LogLevel::Info);
}

void MainWindow::updateTable()
{
    const int columnCount = ui->column_spinBox->value();
    const int rowCount = ui->row_spinBox->value();

    ui->matrix_table->clear();
    ui->matrix_table->setColumnCount(columnCount);
    ui->matrix_table->setRowCount(rowCount);

    for(int x = 0; x < rowCount; x++)
    {
        for(int y = 0; y < columnCount; y++)
        {
            double value = filter[uint(x)][uint(y)];
            ui->matrix_table->setItem(x, y, new QTableWidgetItem(QString::number(value)));
        }
    }
}

void MainWindow::updateMatrix()
{
    int columnCount = ui->column_spinBox->value();
    int rowCount = ui->row_spinBox->value();
    QString currentPreset = ui->preset_comboBox->currentText();

    if(currentPreset.compare("Gauss blur") == 0)
    {
        filter = ConvMatFactory::gaussBlurMatrix(uint(columnCount), uint(rowCount));
        return;
    }
    if(currentPreset.compare("Box blur") == 0)
    {
        filter = ConvMatFactory::blurMatrix(uint(columnCount), uint(rowCount));
        return;
    }
    if(currentPreset.compare("Edge enhance") == 0)
    {
        filter = ConvMatFactory::edgeEnhanceMatrix();
        return;
    }
    if(currentPreset.compare("Sharpen") == 0)
    {
        filter = ConvMatFactory::sharpenMatrix();
        return;
    }
    if(currentPreset.compare("Edge detect") == 0)
    {
        filter = ConvMatFactory::edgeDetectMatrix();
        return;
    }
    if(currentPreset.compare("Emboss") == 0)
    {
        filter = ConvMatFactory::embossMatrix();
        return;
    }
    if(currentPreset.compare("Custom") == 0)
    {
        int rows = ui->row_spinBox->value();
        int columns = ui->column_spinBox->value();
        filter = MatAlg::Matrix<double>(uint(rows), uint(columns), 0.0);

        for(int x = 0; x < ui->matrix_table->rowCount(); x++)
        {
            for(int y = 0; y < ui->matrix_table->columnCount(); y++)
            {
                bool ok = false;
                QString data = ui->matrix_table->item(x, y)->text();
                double value = data.toDouble(&ok);
                if(!ok)
                {
                    emit writeLogToViewer(tr("Error parsing data from table: ") + data);
                    return;
                }
                filter[uint(x)][uint(y)] = value;
            }
        }
    }
}

void MainWindow::on_preset_comboBox_currentIndexChanged(const QString &arg1)
{
    constexpr int maximumValue = 100500;
    constexpr int minimumValue = 1;

    if(arg1.compare("Gauss blur") == 0)
    {
        ui->column_spinBox->setMaximum(maximumValue);
        ui->column_spinBox->setMinimum(minimumValue);

        ui->row_spinBox->setMaximum(maximumValue);
        ui->row_spinBox->setMinimum(minimumValue);
        return;
    }
    if(arg1.compare("Box blur") == 0)
    {
        ui->column_spinBox->setMaximum(maximumValue);
        ui->column_spinBox->setMinimum(minimumValue);

        ui->row_spinBox->setMaximum(maximumValue);
        ui->row_spinBox->setMinimum(minimumValue);
        return;
    }
    if(arg1.compare("Edge enhance") == 0)
    {
        ui->column_spinBox->setValue(3);
        ui->column_spinBox->setMaximum(3);
        ui->column_spinBox->setMinimum(3);

        ui->row_spinBox->setValue(3);
        ui->row_spinBox->setMaximum(3);
        ui->row_spinBox->setMinimum(3);
        return;
    }
    if(arg1.compare("Sharpen") == 0)
    {
        ui->column_spinBox->setValue(3);
        ui->column_spinBox->setMaximum(3);
        ui->column_spinBox->setMinimum(3);

        ui->row_spinBox->setValue(3);
        ui->row_spinBox->setMaximum(3);
        ui->row_spinBox->setMinimum(3);
        return;
    }
    if(arg1.compare("Edge detect") == 0)
    {
        ui->column_spinBox->setValue(3);
        ui->column_spinBox->setMaximum(3);
        ui->column_spinBox->setMinimum(3);

        ui->row_spinBox->setValue(3);
        ui->row_spinBox->setMaximum(3);
        ui->row_spinBox->setMinimum(3);
        return;
    }
    if(arg1.compare("Emboss") == 0)
    {
        ui->column_spinBox->setValue(3);
        ui->column_spinBox->setMaximum(3);
        ui->column_spinBox->setMinimum(3);

        ui->row_spinBox->setValue(3);
        ui->row_spinBox->setMaximum(3);
        ui->row_spinBox->setMinimum(3);
        return;
    }
    if(arg1.compare("Custom") == 0)
    {
        ui->column_spinBox->setMaximum(maximumValue);
        ui->column_spinBox->setMinimum(minimumValue);

        ui->row_spinBox->setMaximum(maximumValue);
        ui->row_spinBox->setMinimum(minimumValue);
        return;
    }
}

void MainWindow::on_genMatrix_button_clicked()
{
    updateMatrix();
    updateTable();
    ui->tabWidget->setCurrentWidget(ui->tab_2);
}

void MainWindow::on_doit_button_clicked()
{
    updateMatrix();
    updateTable();
    if(imagePath.length() == 0)
    {
        emit this->writeLogToViewer(tr("Target image is not set."), LogLevel::Info);
        return;
    }
    ui->doit_button->setEnabled(false);
    emit this->writeLogToViewer(tr("Processing image..."), LogLevel::Info);
    double divisor = ui->divisor_spinBox->value();
    if(divisor == 0.0)
    {
        divisor = MatAlg::elementSum(filter);
    }
    if(divisor == 0.0)
    {
        divisor = 1.0;
    }
    double offset = ui->offset_spinBox->value();
    QImage img;
    if(!img.load(imagePath))
    {
        emit this->writeLogToViewer(tr("Cannot open image."), LogLevel::Error);
        return;
    }
    int threadNumber = ui->thread_spinBox->value();
    int passes = ui->passes_spinBox->value();
    imgWorker.setup(filter, offset, divisor, img, threadNumber, passes);
    imgWorker.start();

}

void MainWindow::on_actionSave_image_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save image"), "",
                                                    tr("PNG image (*.png)"));
    if(fileName.length() == 0)
    {
        emit writeLogToViewer("Save target image not set.", LogLevel::Info);
        return;
    }

    targetPath = fileName;

    emit writeLogToViewer("Save target image set.", LogLevel::Info);
}

void MainWindow::on_pushButton_clicked()
{
    ui->logview->clear();
}
