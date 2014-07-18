Calling essentia from matlab
----------------------------

Matlab documentation: http://www.mathworks.es/es/help/matlab/write-cc-mex-files.html

We have had success calling essentia code from a mex file. Included 
is a sample file `mexsalience.cpp` which computes the salience of an
input wav file.

Compile it like this:

    mex mexsalience.cpp -lessentia -lfftw3f -lsamplerate -lavcodec -lavformat -lavutil  -lyaml

and run it matlab like this:

    >> salience = mexsalience('somewavfile.wav');


TODO: some stuff about `mexFunction` arguments.

Debugging
---------

To run matlab under gdb do this:

    matlab -Dgdb
    (gdb) r -nojvm -nodesktop

It might be a good idea to compile essentia with debugging symbols:

    CXXFLAGS=-g ./waf configure .....

Run mex with the `-g` flag to do the same with your mexfile.

Notes
-----

Be careful about 2-d arrays that you create in C and use in matlab. You need
to swap the rows and columns in C and then transpose the result when you are
back in matlab.

Matlab comes bundled with its own version of libstdc++. If you compile a C++
program and link it against a newer version you may get this nice error:

    /usr/lib/gcc/i686-linux-gnu/4.7/cc1plus: /usr/local/MATLAB/R2012a/sys/os/glnx86/libstdc++.so.6: version `GLIBCXX_3.4.15' not found (required by /usr/lib/i386-linux-gnu/libppl_c.so.4)

The internet is divided on how to solve this problem. We ended up deleting `/usr/local/MATLAB/R2012a/sys/os/glnx86/libstdc++.so.6` so that matlab would use the system version, though it seems like there should be a better solution such as using
`LD_LIBRARY_PATH`.

You can set additional system variables by copying `/usr/local/MATLAB/R2012b/bin/.matlab7rc.sh` to your current directory or to `$HOME` and editing it.
Interesting variables include `LDPATH_PREFIX`, which will be *prepended* to
`LD_LIBRARY_PATH`.

To apply options to the mex compiler, copy `/usr/local/MATLAB/R2012b/bin/mexopts.sh` in a similar manner.

32-bit matlab has its own version of FFTW. If you're linking against this in your
program be careful. Our current workaround is to delete
`/usr/local/MATLAB/R2012b/bin/glnx86/fftw*` but we want to find a better way.


We needed the number of samples of the input audio file to compute the
size of the returned object. We did this by running `stat(2)` to get the
size of the file, removing the header, and assuming 16 bit 1 channel audio.
Don't do this at home.
