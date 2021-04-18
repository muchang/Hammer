# AIGCover
A Coverage-Guided Fuzzer for Hardware Model Checkers

-----
## Introduction
We introduce AIGCover, an effective coverage-guided fuzzer for validating the implementation of hardware model checker.
The key idea is to mutate the existing And-Inverter Graph (AIG) by manipulating the relations among the components in the graph while
preserving the validity of the mutant. This mutation process is guided by the coverage information to generate diverse models for
drilling deeply into the checkers. In our empirical evaluation, AIGCover increases the code coverage comparing to the mutation seed
files and found 9 unique issues in state-of-the-art hardware model checkers, which shows its effectiveness.

## Installation     
AIGCover requires golang environment. You can use the following command to install golang environment in Ubuntu:
```shell
sudo apt-get install golang-go
```
Next, clone the source code of AIGCover, enter the AIGCover directory:
```
git clone https://github.com/aigcover/AigCover.git
cd AIGCover
```
Then, make AIGCover:
```shell
cd aiger-1.9.9
./configure.sh && make
cd ..
./build.sh
```

## Run AIGCover

Next, we demonstrate how to run AIGCover with ABC hardware model checker. 

### Compiling ABC with afl-gcc
Clone ABC with the following command:
```shell
git clone https://github.com/berkeley-abc/abc.git
```
You can find more detail about ABC at https://github.com/berkeley-abc/abc.   

Then, replace gcc with afl-gcc in the Makefile of ABC:
```makefile
#CC   := gcc
#CXX  := g++
CC := $AIGCover/afl-gcc
CXX := $AIGCover/afl-g++
```
`$AIGCover` is the path to the directory you installed AIGCover.

Then, build ABC by the following command:
```
make ABC_USE_NO_READLINE=1
```

You can find more details about afl-gcc and afl-g++ at https://github.com/google/AFL .

### Running AIGCover
Due to the limited space of the GitHub repository, here we just provided a small subset of aiger benchmarks in /AigCover/benchmarkExample. We will submit the full benchmarks to artifact evaluation later.

Use following command to run AIGCover for testing ABC:
```shell
./afl-fuzz -m 1000 -i $AIGCover/benchmarkExample -o $AIGCover/out -f $AIGCover/Temp/temp.aig $abcPath/abc -c 'read @@;pdr;'
```
`$abcPath` is the path to the ABC installation directory.  

## Issue Triggering Files

We provide the issue triggering files in `AIGCover/issues` directory. The model checker versions used in our evaluation are: ABC `901560b`, AVY `f2e3044`, NuXmv `2.0.0`.  The list of the files are as follows:

| File Name | Tool | Type | Issue Triggering Command |
|----|----|----|----|
|int.aig| abc | assertion failure | .../abc -c "read .../int.aig;int;" |
|pdr.aig| abc | assertion failure | .../abc -c "read .../pdr.aig;pdr;" |
|temporVec.aig| abc |assertion failure | .../abc -c "read .../tempor.aig;tempor;"|
|bmc2.aig| abc |Segmentation fault| .../abc -c "read .../bmc2.aig;fold;bmc2;"|
|avyCrashDupWith.aig| avy | Segmentation fault |.../avy .../avy.aig|
|avyAssertionWecEntey.aig| avy | assertion failure| .../avy .../avy.aig|
|NuXmv.aig| nuXmv|Segmentation fault|.../nuXmv -source ../command <br> command:<br>read_aiger_model -i .../NuXmv.aig <br> go <br> check_property <br> quit|
|abcUnsoundness.aig|abc|unsoundness|.../abc -c "read .../pdr.aig;pdr;" <br> .../abc/abc -c "read reduced.aig;logic;undc;strash;zero;pdr;write_cex -a out.cex"|
|avyUnsoundness.aig|avy|unsoundness|.../avy .../avyabcUnsoundness.aig <br> .../abc -c "read avyabcUnsoundness.aig;pdr"|

## Code and Path Coverage
In `AIGCover/AIGCover_Result`, You can find the mutants that covered new paths for each coverage evaluation. The number of files for each coverage evaluation matches the number of paths in Table 2. If you run the corresponding hardware model checker through the files for each coverage evaluation, the model checker can cover the same numbers of line, function, and branch in Table 2. Reproducing these evaluations would be time-consuming. The full automated coverage evaluation scripts will be provided when submitting to artifact evaluation. 