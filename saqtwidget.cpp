#include "saqtwidget.h"
#include <QOpenGLShaderProgram>
#include "vertex.h"
#include "cuboidverts.h"
#include <math.h>
#include "transform3d.h"
#include "audioutils.h"


static const  std::array<Vertex, 1> vertexArray = {Vertex(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f))};

Saqtwidget::Saqtwidget(QWidget *parent) :
    QOpenGLWidget(parent)
  , newAudioFile(false)
  , n_channels(2)
  , frameCount(1764)
  , sampleCount(n_channels * frameCount)
  , n_spectrumBins(1 + (frameCount / 2))
  , amp_spectrum_l(std::vector<double>(n_spectrumBins))
  , amp_spectrum_r(std::vector<double>(n_spectrumBins))



{
    int nboxes = frameCount;
    timer.start();
    for (int i = 0; i < nboxes; ++i) {
        transforms.push_back(Transform3D());
        //transforms.back().translate(-0.9f + float(i) * (1.8f / float(n_spectrumBins)), -0.9f, 0.0f);
        transforms.back().translate(-0.9f + (float(std::log(i) / std::log(n_spectrumBins) ) * 1.8f), -0.9f, 0.0f);
    }
}


void Saqtwidget::initializeGL()
{
    initializeOpenGLFunctions();
    connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
    glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    program = new QOpenGLShaderProgram();
    {
      program = new QOpenGLShaderProgram();
      program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
      program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.frag");
      program->link();
      program->bind();

      modelToWorld = program->uniformLocation("modelToWorld");
      worldToView = program->uniformLocation("worldToView");
      timeElapsed = program->uniformLocation("timeElapsedInMillis");
      fftVal = program->uniformLocation("FFTVal");

      gl_buffer.create();
      gl_buffer.bind();
      gl_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
      // consider StreamDraw when changing buffer data... https://paroj.github.io/gltut/Positioning/Tutorial%2003.html
      gl_buffer.allocate(vertexArray.data(), sizeof(vertexArray[0]) * vertexArray.size());

      vbObject.create();
      vbObject.bind();
      program->enableAttributeArray(0);
      program->enableAttributeArray(1);
      program->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(),
                                    Vertex::PositionTupleSize, Vertex::stride());

      program->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(),
                                    Vertex::ColorTupleSize, Vertex::stride());

      vbObject.release();
      gl_buffer.release();
      program->release();
    }
}


void Saqtwidget::resizeGL(int w, int h)
{
    projection.setToIdentity();
    projection.perspective(45.0f, w / float(h), 0.0f, 1000.0f);
}


void Saqtwidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    program->bind();
    program->setUniformValue(worldToView, projection);
    glUniform1f(timeElapsed, timer.elapsed());
    vbObject.bind();

    for (int i = 0; i < int(amp_spectrum_l.size()); ++i)
    {
        program->setUniformValue(modelToWorld, transforms[i].toMatrix());
        program->setUniformValue(fftVal, float(amp_spectrum_l[i]));
        glDrawArrays(GL_POINTS, 0, vertexArray.size());
    }

    vbObject.release();
    program->release();
}


void Saqtwidget::update()
{
  QOpenGLWidget::update();
}


Saqtwidget::~Saqtwidget() {
    vbObject.destroy();
    gl_buffer.destroy();
    delete program;
}

void Saqtwidget::newAudioFileFlag()
{
    newAudioFile = true;
}

double smoothing(double newVal, double smoothCoef, double previousVal)
{
    return (smoothCoef * newVal) + ((1.0 - smoothCoef) * previousVal);
}

void Saqtwidget::processAudioBuffer(QAudioBuffer buffer)
{
    if (newAudioFile)
    {
        n_channels = buffer.format().channelCount();
        frameCount = buffer.frameCount();
        sampleCount = buffer.sampleCount();
        n_spectrumBins = 1 + (frameCount / 2);
        newAudioFile = false;
    }


    if (int(amp_spectrum_l.size()) != n_spectrumBins)
    {
        amp_spectrum_l.resize(n_spectrumBins);
        amp_spectrum_r.resize(n_spectrumBins);
    }
    // this is a nieve implementation to get things goings, should be optimized.
    std::vector<double> l_channel;
    std::vector<double> r_channel;
    const quint16 *data = buffer.constData<quint16>();

    for (int sample = 0; sample < sampleCount; ++sample)
    {
        if (sample % 2 == 0)
            l_channel.push_back(AudioUtils::normalize(data[sample], 65535.0));
        else
            r_channel.push_back(AudioUtils::normalize(data[sample], 65535.0));
    }

    AudioUtils::hamming_window(l_channel, int(l_channel.size()));
    AudioUtils::hamming_window(r_channel, int(r_channel.size()));

    const std::vector<fftw_complex> &fft_out_l = processor.processBuffer(l_channel, n_spectrumBins);
    const std::vector<fftw_complex> &fft_out_r = processor.processBuffer(r_channel, n_spectrumBins);

    double smoothFactor = 0.4;

    for (int i = 0; i < n_spectrumBins; ++i)
    {
        amp_spectrum_l[i] = smoothing(AudioUtils::db_at_freq(fft_out_l[i][0], fft_out_l[i][1], n_spectrumBins), smoothFactor, amp_spectrum_l[i]);
        amp_spectrum_r[i] = smoothing(AudioUtils::db_at_freq(fft_out_r[i][0], fft_out_r[i][1], n_spectrumBins), smoothFactor, amp_spectrum_l[i]);
    }
 }
