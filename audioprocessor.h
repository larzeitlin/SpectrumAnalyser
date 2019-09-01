#ifndef AUDIOPROCESSOR_H
#define AUDIOPROCESSOR_H
#include <vector>

class AudioProcessor
{
public:
    AudioProcessor();
    void processBuffer(std::vector<double> buffer_in);
};

#endif // AUDIOPROCESSOR_H
