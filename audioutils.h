#ifndef AUDIOUTILS_H
#define AUDIOUTILS_H
#include <math.h>
#include <vector>

class AudioUtils
{
public:
    static double normalize(int val_in, double divisor);
    static double db_at_freq(double r, double i, int n_bins);
    static void hamming_window(std::vector<double> &data_in, int frames);
};


inline double AudioUtils::normalize(int val_in, double divisor)
{
    return (double(val_in) / divisor) - 0.5;
}

inline double AudioUtils::db_at_freq(double r, double i, int n_bins)
{
    return 20.0 * std::log10(std::sqrt(std::pow(r, 2.0) + std::pow(i, 2.0)) / n_bins);
}

inline void AudioUtils::hamming_window(std::vector<double> &data_in, int frames)
{
    for (int i = 0; i < frames; i++) {
        double multiplier = 0.54 - (0.46 * std::cos(2.0*M_PI*i/frames));
        data_in[i] *= multiplier;
    }
}


#endif // AUDIOUTILS_H
