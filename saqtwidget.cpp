#include "saqtwidget.h"
#include <QOpenGLShaderProgram>
#include "vertex.h"
#include <math.h>
#include "transform3d.h"
#include "audioutils.h"


// Default values
static const int default_n_channels = 2;
static const int default_frame_count = 1764;
static const int default_sample_count = default_n_channels * default_frame_count;
static const int default_n_spectrum_bins = (default_frame_count / 2) + 1;
static const Vertex green_middle_point = Vertex(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));

Saqtwidget::Saqtwidget(QWidget *parent) :
    QOpenGLWidget(parent)
  , newAudioFile(false)
  , n_channels(default_n_channels)
  , frameCount(default_frame_count)
  , sampleCount(default_sample_count)
  , n_spectrumBins(default_n_spectrum_bins)
  , points(std::vector<Vertex>(n_spectrumBins, green_middle_point))
  , amp_spectrum_l(std::vector<double>(n_spectrumBins))
  , amp_spectrum_r(std::vector<double>(n_spectrumBins))
{
    reset_points();
}

void Saqtwidget::reset_points()
{
    points = std::vector<Vertex>(n_spectrumBins, green_middle_point);
    static const int far_left = -1.0;
    double full_width = 2.0;
    for (std::size_t i = 0; i != points.size(); ++i) {
        points[i].setPosition(QVector3D(far_left + (log10(i) / log10(points.size()) * full_width) , 0.0f, 0.0f));
    }
}

void Saqtwidget::update_points()
{
    for (std::size_t i = 0; i != points.size(); ++i) {
        QVector3D position = points[i].position();
        position.setY(0.4 + (amp_spectrum_l[i] / 50.0));
        points[i].setPosition(position);
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
      gl_buffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
      gl_buffer.allocate(points.data(), sizeof(points[0]) * points.size());

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
    update_points();
    gl_buffer.bind();
    auto ptr = gl_buffer.map(QOpenGLBuffer::WriteOnly);
    memcpy(ptr, points.data(), points.size() * sizeof(points[0]));
    gl_buffer.unmap();
    gl_buffer.release();
    glClear(GL_COLOR_BUFFER_BIT);
    program->bind();
    program->setUniformValue(worldToView, projection);
    glUniform1f(timeElapsed, timer.elapsed());
    vbObject.bind();
    program->setUniformValue(modelToWorld, QMatrix4x4());
    glDrawArrays(GL_LINE_STRIP, 0, points.size());
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
        reset_points();
    }


    if (int(amp_spectrum_l.size()) != n_spectrumBins)
    {
        amp_spectrum_l.resize(n_spectrumBins);
        amp_spectrum_r.resize(n_spectrumBins);
    }
    // There must be a better way...
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

    double smoothFactor = 0.2;

    for (int i = 0; i < n_spectrumBins; ++i)
    {
        amp_spectrum_l[i] = smoothing(AudioUtils::db_at_freq(fft_out_l[i][0], fft_out_l[i][1], n_spectrumBins), smoothFactor, amp_spectrum_l[i]);
        amp_spectrum_r[i] = smoothing(AudioUtils::db_at_freq(fft_out_r[i][0], fft_out_r[i][1], n_spectrumBins), smoothFactor, amp_spectrum_l[i]);
    }
 }
