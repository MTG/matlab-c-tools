/*
 * Author: Alastair Porter, Vignesh Ishwar
 * Affiliation: Universitat Pompeu Fabra
 *
 */

#include "mex.h"
#include "matrix.h"

#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <essentia/algorithmfactory.h>
#include <essentia/essentiamath.h>
#include <essentia/pool.h>

using std::cout;
using std::endl;
using std::vector;
using namespace essentia;
using namespace essentia::standard;

Algorithm* _audioload;
Algorithm* _equalLoudness;

Algorithm* _frameCutter;
Algorithm* _windowing;

// Spectral peaks
Algorithm* _spectrum;
Algorithm* _spectralPeaks;
// Pitch salience contours
Algorithm* _pitchSalienceFunction;

//intermediate values
vector<Real> audioBuffer;
vector<Real> spectrum, freqs, mags;
vector<Real> frame, windowedFrame;
vector<Real> frameSalience;


void ourinit() {

    int frameSize = 2048;
    int hopSize = 128;
    float sampleRate = 44100.0;
    
    essentia::init();
    // Create algorithms
    AlgorithmFactory& factory = standard::AlgorithmFactory::instance();

    _frameCutter = factory.create("FrameCutter");

    _windowing                  = factory.create("Windowing");
    _spectrum                   = factory.create("Spectrum");
    _spectralPeaks              = factory.create("SpectralPeaks");
    _pitchSalienceFunction      = factory.create("PitchSalienceFunction");

    _audioload = factory.create("MonoLoader");

    _equalLoudness = factory.create("EqualLoudness");

    // Wire the algorithms together
    _audioload->output("audio").set(audioBuffer);

    _frameCutter->input("signal").set(audioBuffer);
    _frameCutter->output("frame").set(frame);
    _windowing->input("frame").set(frame);

    _windowing->output("frame").set(windowedFrame);
    _spectrum->input("frame").set(windowedFrame);


    _spectrum->output("spectrum").set(spectrum);
    _spectralPeaks->input("spectrum").set(spectrum);

    _spectralPeaks->output("frequencies").set(freqs);
    _spectralPeaks->output("magnitudes").set(mags);

    _pitchSalienceFunction->input("frequencies").set(freqs);
    _pitchSalienceFunction->input("magnitudes").set(mags);

    _pitchSalienceFunction->output("salienceFunction").set(frameSalience);


    // Configure parameters for algorithms
    _frameCutter->configure("frameSize", frameSize,
                          "hopSize", hopSize,
                          "startFromZero", false);

    _windowing->configure("size", frameSize,
                        "zeroPadding", 3 * frameSize,
                        "type", "hann");
    _spectrum->configure("size", frameSize * 4);

    _spectralPeaks->configure("minFrequency", 1,
                            "maxFrequency", 20000,
                            "maxPeaks", 100,
                            "sampleRate", sampleRate,
                            "magnitudeThreshold", 0,
                            "orderBy", "magnitude");
}

int get_num_samples(char *filename) {
    struct stat fs;
    int fp = open(filename, O_RDONLY);
    if (fp == -1) 
    {
                printf("Error opening file %s\n", filename);
    }

    if (fstat(fp, &fs) == -1)
    {
        printf("Error statting file %s\n", filename);
    }
    int size = fs.st_size;
    // XXX: This is really rough - we don't check any header information,
    // channels, sample size, etc..., and it only works on wav files
    // 128 = hop size
    int numsamples = (int)ceil((double)(size-44) / 2.0 / 128.0);
    return numsamples;
}

void compute_frames(char* filename, double *retSamples) {


    _audioload->configure("filename", filename,
                        "sampleRate", 44100.0,
                        "downmix", "mix");

  _audioload->compute();

  int max_size = 0;
  int count = 0;

  while (true) {

    // compute a frame
    _frameCutter->compute();

    // if it was the last one (ie: it was empty), then we're done.
    if (!frame.size()) {
      break;
    }

    _windowing->compute();
    _spectrum->compute();
    _spectralPeaks->compute();
    _pitchSalienceFunction->compute();


    for (int i = 0; i < 600; i++) {
        retSamples[count*600+i] = frameSalience[i];
    }

    count++;
  }
}

void mexFunction ( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] )
{
    
    char *filename = mxArrayToString(prhs[0]);
    mexPrintf("hello %s\n", filename);

    ourinit();
    int numsamples = get_num_samples(filename);

    plhs[0] = mxCreateDoubleMatrix(numsamples, 600, mxREAL);
    nlhs = 1;
    double* pathOutPtr = mxGetPr(plhs[0]);
    compute_frames(filename, pathOutPtr);
}
