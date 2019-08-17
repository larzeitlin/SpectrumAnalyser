#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    currentFile = "";
    ui->label->setText("No File Selected");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionLoad_triggered()
{
    currentFile = QFileDialog::getOpenFileName(this, "open a file");
    ui->label->setText(currentFile);
}

void MainWindow::on_playButton_clicked()
{
    if (QFile(currentFile).exists())
    {
        player.setMedia(QUrl::fromLocalFile(currentFile));
        player.setVolume(50);
        player.play();
    } else {
        ui->label->setText("Can't Play File");
    }
}
void MainWindow::on_stopButton_clicked()
{
   player.stop();
}
