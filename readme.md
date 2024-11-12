# midterm variant 1
Authors (team): 
- Roman Zaletskyi (https://github.com/Romko-studik)

## Prerequisites

C++:
-boost
### Compilation

```bash
mkdir cmake-build-debug
cd cmake-build-debug
cmake ..
cmake --build .
```

### Usage

```bash
./build/capitilizer <input_file><output_file>
```

### Results

We implemented mycat utilite, which has the same functionality as system function cat. It can work with multiple files set by absolute or relative names. It will immidiately stop execution if one of the files listed doesn't exist or can't be opened.
    It`s execution time on large files is close to that of a system cat execution time.