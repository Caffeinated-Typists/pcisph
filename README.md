# PCISPH Algorithm

Our implementation of the [PCISPH Algorithm](https://doi.org/10.1145/1576246.1531346), so far

## Steps to run the project

### Local Build
1. Clone the repository
```bash
git clone https://github.com/Caffeinated-Typists/pcisph/
```
2. Make a build directory
```bash
cd pcisph
mkdir build
```
3. Build the project by running `build.sh`
```bash
chmod +x build.sh # incase the script is not executable
./build.sh -c # -c flag for creating CMake files as well. Subsequent builds can be done without the -c flag
```
4. Run the project
```bash
./pcisph.out
```

### Docker Build
1. Clone the repository
```bash
git clone https://github.com/Caffeinated-Typists/pcisph/
```
2. Run the `run.sh` script
```bash
cd pcisph
chmod +x run.sh # incase the script is not executable
./run.sh
```
