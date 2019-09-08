#ifndef AUDIOUTILS_H
#define AUDIOUTILS_H
#include <math.h>
#include <vector>

class AudioUtils
{
public:
    static double normalize(int val_in, double divisor);
    static double amplitude_at_freq(double r, double i, int n_bins);
    static void han_window(std::vector<double> &data_in, int frames);
};


inline double AudioUtils::normalize(int val_in, double divisor)
{
    return (double(val_in) / divisor) - 0.5;
}

inline double AudioUtils::amplitude_at_freq(double r, double i, int n_bins)
{
    return std::sqrt(std::pow(r, 2.0) + std::pow(i, 2.0)) / n_bins;
}

inline void AudioUtils::han_window(std::vector<double> &data_in, int frames)
{
    for (int i = 0; i < frames; i++) {
        double multiplier = 0.5 * (1 - std::cos(2.0*M_PI*i/(frames - 1)));
        data_in[i] *= multiplier;
    }
}


#endif // AUDIOUTILS_H
