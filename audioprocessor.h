#ifndef AUDIOPROCESSOR_H
#define AUDIOPROCESSOR_H
#include <vector>
#include <fftw3.h>
class AudioProcessor
{
public:
    AudioProcessor();
    const std::vector<fftw_complex> & processBuffer(std::vector<double> buffer_in, int n_spectrum_bins);

private:
    std::vector<fftw_complex> fft_out;
};

#endif // AUDIOPROCESSOR_H
