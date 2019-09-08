#include "audioprocessor.h"
#include <fftw3.h>
#include <string.h>
#include <QDebug>


AudioProcessor::AudioProcessor() :
    fft_out(std::vector<fftw_complex>((1764 / 2) + 1))
{}

const std::vector<fftw_complex> & AudioProcessor::processBuffer(std::vector<double> buffer_in, int n_spectrum_bins)
{
    if (int(fft_out.size()) != n_spectrum_bins)
    {
        fft_out = std::vector<fftw_complex>(n_spectrum_bins);
    }
    fftw_plan my_plan;
    my_plan = fftw_plan_dft_r2c_1d(int(buffer_in.size()),
                                   buffer_in.data(),
                                   fft_out.data(),
                                   FFTW_MEASURE);
    fftw_execute(my_plan);
    fftw_destroy_plan(my_plan);
    return fft_out;
}
