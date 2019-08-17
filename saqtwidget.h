#ifndef SAQTWIDGET_H
#define SAQTWIDGET_H

#include<QOpenGLWidget>

class Saqtwidget : public QOpenGLWidget
{
public:
    Saqtwidget(QWidget *parent);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
};

#endif // SAQTWIDGET_H
