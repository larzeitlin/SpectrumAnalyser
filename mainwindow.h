#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QtMultimedia>
#include <QtMultimediaWidgets>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionLoad_triggered();
    void on_playButton_clicked();
    void on_stopButton_clicked();

private:
    Ui::MainWindow *ui;
    QString currentFile;
    QMediaPlayer player;
    QAudioProbe probe;
};
#endif // MAINWINDOW_H
