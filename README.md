# History DAG scratchpad

Requirements
------------

* GCC 11.1
* GNU make
* pkgconfig
* protobuf libraries and compiler
* zlib

For Ubuntu 20.04 LTS the following commands installs the requirements and performs a full build:

`sudo apt get install libasan6 libubsan1 make git g++-10 pkgconf protobuf-compiler libprotobuf-dev zlib1g-dev`

`make CXX=g++-10 -j16`

Building
--------

Executing `make` will build and run the tests.

Address and UB sanitizers can be disabled by `make SANITIZE=no`. Disabling them may be helpful if *asan* and *ubsan* libraries are not available on the system.

Valgrind's *memcheck* can be conveniently called by `make MEMCHECK=yes`. This option disables sanitizers, and `make clean` should be run first if there is an existing build with sanitizers enabled.

Passing *nocatch* to the tests executable will allow exceptions to escape, which is useful for debugging. A gdb session can be started with `gdb --args ./out/history-dag-test nocatch`.
