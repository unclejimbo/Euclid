# Running the Tests

First of all, it's suggested to compile the tests in release mode unless you plan to debug a single test case. Since some modules involve intensive computation and it's too slow to run the whole test in debug mode.

After compiling the test, you'll find a `runtest` executable in the binary output directory.

Testing in Euclid is powered by [Catch2](https://github.com/catchorg/Catch2), which is already bundled in Euclid. It provides a lot of convenient ways to run the test.

You can execute `runtest` without any command line argument to run the whole test.

Other than that, you could run the test on a whole module like

```bash
runtest "[math]"
```

to test all the submodules in the math module. Or test a single submodule directly,

```bash
runtest "[vector]"
```

And you can run a single test section like

```bash
runtest "[vector]" -c "length"
```

For more command line options, please refer to the [Catch2 documentation](https://github.com/catchorg/Catch2/blob/master/docs/command-line.md#top).
