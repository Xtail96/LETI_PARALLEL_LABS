# Using Open MPI on MacOS

## Installation

```
brew install open-mpi
```

## Compiling
```
mpicc -o <target_name> <file.c>
```

or

```
./assembly.sh build <file_name_without_ext>
```


## Running
```
mpirun --hostfile <hostfile> -np <process_count> <file>
```

or

```
./assembly.sh run <file_name_without_ext>
```