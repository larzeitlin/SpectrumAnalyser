#include "saqtwidget.h"
#include <QOpenGLShaderProgram>
#include "vertex.h"
#include "cuboidverts.h"
#include <math.h>
#include "transform3d.h"

static const  std::array<Vertex, 36> vertexArray = CuboidVerts(1.0f, 1.0f, 1.0f).getVertexArray();
// for now we will hardcode these, they should however be pulled from audio file when loaded
const int n_channels = 2;
const int frameCount = 1764;
const int sampleCount = n_channels * frameCount;
const int n_spectrumBins = 1 + (frameCount / 2);

void han_window(std::vector<double> &data_in, int frames)
{
    for (int i = 0; i < frames; i++) {
        double multiplier = 0.5 * (1 - std::cos(2.0*M_PI*i/(frames - 1)));
        data_in[i] *= multiplier;
    }
}

Saqtwidget::Saqtwidget(QWidget *parent) :
    QOpenGLWidget(parent)
  , real_spectrum_l(std::vector<double>(n_spectrumBins))
  , real_spectrum_r(std::vector<double>(n_spectrumBins))


{
    // Set up our spinny cubes ---------------------

    int nboxes = frameCount;
    timer.start();
    for (int i = 0; i < nboxes; ++i) {
        transforms.push_back(Transform3D());
        transforms.back().translate(-2.0f + (0.05f * i), -0.5f, -5.0f);
        transforms.back().setScale(0.02f);
        transforms.back().rotate(5.0f, -1.0f, -1.0f, 1.0f);
    }

    // Set up our spinny cubes ----------------END

    // Just testing out the FFT here ------------

    std::vector<double> testvec {1.0, 0.0, -1.0, 0.0, 1.0, 0.0, -1.0, 0.0};
    // processor.processBuffer(testvec);

     // Just testing out the FFT here ---------END
}

void Saqtwidget::initializeGL()
{
    // Set up the rendering context
    initializeOpenGLFunctions();

    connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));

    // Tells openGL to only draw faces that wind anti-clockwise
    glEnable(GL_CULL_FACE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    program = new QOpenGLShaderProgram();
    {
      // Create Shader (Do not release until VAO is created)
      program = new QOpenGLShaderProgram();
      program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
      program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.frag");
      program->link();
      program->bind();

      // Cache Uniform Locations (  Waaahh?  )
      modelToWorld = program->uniformLocation("modelToWorld");
      worldToView = program->uniformLocation("worldToView");
      timeElapsed = program->uniformLocation("timeElapsedInMillis");
      fftVal = program->uniformLocation("FFTVal");

      // Create Buffer (Do not release until VAO is created)
      gl_buffer.create();
      gl_buffer.bind();
      gl_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
      // consider StreamDraw when changing buffer data... https://paroj.github.io/gltut/Positioning/Tutorial%2003.html

      gl_buffer.allocate(vertexArray.data(), sizeof(vertexArray[0]) * vertexArray.size());


      // Create Vertex Array Object
      vbObject.create();
      vbObject.bind();
      program->enableAttributeArray(0);
      program->enableAttributeArray(1);
      program->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(),
                                    Vertex::PositionTupleSize, Vertex::stride());
      program->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(),
                                    Vertex::ColorTupleSize, Vertex::stride());

      // Release (unbind) all
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
    float timeInMillis = timer.elapsed();
    // Clear
    glClear(GL_COLOR_BUFFER_BIT);
    // Render using our shader
    program->bind();
    program->setUniformValue(worldToView, projection);
    glUniform1f(timeElapsed, timeInMillis);
    vbObject.bind();

    for (int i = 0; i < real_spectrum_l.size(); ++i)
    {
        program->setUniformValue(modelToWorld, transforms[i].toMatrix());
        program->setUniformValue(fftVal, float(real_spectrum_l[i]));
        glDrawArrays(GL_TRIANGLES, 0, vertexArray.size());
    }


  //  for (auto t : transforms) {
  //      program->setUniformValue(modelToWorld, t.toMatrix());
  //      program->setUniformValue(fftVal, float(real_spectrum_l[counter]));
  //      glDrawArrays(GL_TRIANGLES, 0, vertexArray.size());
  //      counter += 1;
  //  }



    vbObject.release();
    program->release();
}

void Saqtwidget::update()
{
  // Schedule a redraw
  QOpenGLWidget::update();
}

Saqtwidget::~Saqtwidget() {
    vbObject.destroy();
    gl_buffer.destroy();
    delete program;
}

double normalize(int val_in, double divisor)
{
    return (double(val_in) / divisor) - 0.5;
}



void Saqtwidget::processAudioBuffer(QAudioBuffer buffer)
{
    if (real_spectrum_l.size() != n_spectrumBins)
    {
        real_spectrum_l.resize(n_spectrumBins);
        real_spectrum_r.resize(n_spectrumBins);
    }
    // this is a nieve implementation to get things goings, should be optimized.
    std::vector<double> l_channel;
    std::vector<double> r_channel;
    const quint16 *data = buffer.constData<quint16>();

    // qInfo() << normalize(data[10], 65535.0);

    for (int sample = 0; sample < sampleCount; ++sample)
    {
        if (sample % 2 == 0)
            l_channel.push_back( normalize(data[sample], 65535.0));
        else
            r_channel.push_back( normalize(data[sample], 65535.0));
    }
    han_window(l_channel, int(l_channel.size()));
    han_window(r_channel, int(r_channel.size()));

    const std::vector<fftw_complex> &fft_out_l = processor.processBuffer(l_channel);
    const std::vector<fftw_complex> &fft_out_r = processor.processBuffer(r_channel);

    for (int i = 0; i < n_spectrumBins; ++i)
    {
        real_spectrum_l[i] = std::sqrt(std::pow(fft_out_l[i][0], 2.0) + std::pow(fft_out_l[i][1], 2.0)) / n_spectrumBins;
        real_spectrum_r[i] = std::sqrt(std::pow(fft_out_r[i][0], 2.0) + std::pow(fft_out_r[i][1], 2.0)) / n_spectrumBins;
    }
 }
