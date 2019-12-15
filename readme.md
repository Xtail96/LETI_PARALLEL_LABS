# Using Open MPI on MacOS

## Installation

```
brew install open-mpi
```

## Compiling
```
mpicc -o <target_name> <file.c>
```

## Running
```
mpirun --hostfile <hostfile> -np <process_count> <file>
```