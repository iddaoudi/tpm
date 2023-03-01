# TPM
Task power management

## Principle
* Several applications in the ```algorithms``` directory;

## Dependences
* Clang10
* Numactl (latest)
* OpenBLAS (latest)
* LLVM OpenMP with OMPT support (latest)

## Environment installation
Simply use the script called ```install_dependencies```: launching it will install all needed dependencies

## Usage
Either use or get the inspiration from the ```launch_script```: parameters that can be modified are enclosed by clear signs (```Can be modified``` and ```No modifications after this line```)

## Best parameters for the algorithms
* Cholesky: matrix sizes of ```8192```, ```16384```, ```24576```, ```32768``` with bloc sizes of ```512``` or ```1024```
* QR: matrix sizes of ```8192```, ```16384```, ```24576```, ```32768``` with bloc sizes of ```512``` or ```1024```
* LU: matrix sizes of ```8192```, ```16384```, ```24576```, ```32768``` with bloc sizes of ```512``` or ```1024```
* SparseLU: matrix sizes of ```64```, ```128```, ```256``` with bloc sizes of ```32```, ```64``` or ```128```
* Poisson: TBD

Always make sure to use powers of 2, and bloc sizes always have to divide matrix sizes.
