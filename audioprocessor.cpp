#include "audioprocessor.h"
#include <fftw3.h>
#include <string.h>
#include <QDebug>

// temporary hack
const int buffer_length = 1764;

AudioProcessor::AudioProcessor() :
    fft_out(std::vector<fftw_complex>((buffer_length / 2) + 1))
{}

const std::vector<fftw_complex> & AudioProcessor::processBuffer(std::vector<double> buffer_in)
{
    fftw_plan my_plan;
    my_plan = fftw_plan_dft_r2c_1d(buffer_length,
                                   buffer_in.data(),
                                   fft_out.data(),
                                   FFTW_MEASURE);
    fftw_execute(my_plan);
    fftw_destroy_plan(my_plan);
    return fft_out;
}
