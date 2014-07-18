Shared libraries from matlab
----------------------------

The main matlab documentation for running shared libraries is at
http://www.mathworks.es/es/help/matlab/using-c-shared-library-functions-in-matlab-.html
This document is based on the above page.

We have included a sample shared library, libmylib, implemented in `mylib.c`

It has a single method:

    int* mymethod(int, int*);

It returns an array of integers, and the size of this array in the second
argument. The first argument is printed to the screen.

Compile it, and a sample C program like this

    $ gcc -shared -fPIC mylib.c -o libmylib.so
    $ gcc -o caller caller.c -lmylib -L.

And run the sample program like this:

    $ LD_LIBRARY_PATH=. ./caller
    Hello from mymethod: 2
    size is 2
    arr is [3 6]

You can run the same method in matlab.

    >> loadlibrary('libmylib.so', 'mylib.h')        

    >> sz = libpointer('int32Ptr', 1);
    >> xret = calllib('mylib', 'mymethod', 1, sz)
    Hello from mymethod: 1
     
    >> sz.Value

    ans =

               2

    >> setdatatype(xret,'int32Ptr', 1, sz.Value)
    >> get(xret)
           Value: [3 6]
        DataType: 'int32Ptr'

Some notes
----------

You can print all of the methods of a library (including what matlab
thinks the return types are) by running

    >> libfunctions libmylib -full

This is useful to find the pointer types to use with `libpointer()`

To pass a pointer argument, you must first create it. If you don't give it
a default value, it seems that it creates a NULL pointer, so be careful here.
For example:

    >> sz = libpointer('int32Ptr');
    >> xret = calllib('mylib', 'mymethod', 1, sz)

    void mymethod(int a, int* b) {
        *b = 1;
    }

Will crash. Use this to create sz instead:

    >> sz = libpointer('int32Ptr', 1);

If you return a pointer to data you need to tell matlab how big it is:

    >> setdatatype(xret,'int32Ptr', 1, sz.Value)
