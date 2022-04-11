# History DAG scratchpad

Executing `make` will build and run the tests.

Address and UB sanitizers can be disabled by `make SANITIZE=no`. Disabling them may be helpful if *asan* and *ubsan* libraries are not available on the system.

Valgrind's *memcheck* can be conveniently called by `make MEMCHECK=yes`. This option disables sanitizers, and `make clean` should be run first if there is an existing build with sanitizers enabled.

Passing *nocatch* to the tests executable will allow exceptions to escape, which is useful for debugging. A gdb session can be started with `gdb --args ./out/history-dag-test nocatch`.
