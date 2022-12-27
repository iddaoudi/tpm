# TPM
Task power management

## Principle
* Several applications in the ```algorithms``` directory;
* Tracing them is done in the ```trace``` directory with ```tpm_library``` using OMPT callbacks;
* Energy consumption management is done in the ```power``` directory using ZeroMQ and CPUFreq.

## Dependences
* Clang10 (or later)
* Numactl (latest)
* OpenBLAS (latest)
* Lapacke (latest)
* LLVM OpenMP with OMPT support (latest)
* CPUFreq and CPUFreqUtils (latest)
* ZeroMQ (version 3)

## Usage
Either use or get the inspiration from the ```launch_script```
