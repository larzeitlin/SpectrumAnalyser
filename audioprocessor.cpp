#include "audioprocessor.h"
#include <fftw3.h>
#include <string.h>
#include <QDebug>

AudioProcessor::AudioProcessor()
{}

void AudioProcessor::processBuffer(std::vector<double> buffer_in)
{
    qInfo() << "hi, I'm here \n";
    int N = int(buffer_in.size());

    std::vector<fftw_complex> out(N);

    fftw_plan my_plan;
    my_plan = fftw_plan_dft_r2c_1d(N,
                                   &buffer_in.front(),
                                   &out.front(),
                                   FFTW_ESTIMATE);
    fftw_execute(my_plan);

    for (auto i: out)
    {
        qInfo() << "Real Part: " << i[0] << " Ima Part: " << i[1] << "\n";
    }

    fftw_destroy_plan(my_plan);
}
