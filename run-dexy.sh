set -e

### "xvfb"
Xvfb :1 -screen 0 1024x768x24 &
export DISPLAY=":1"

### "clean"
rm -r build

### "build-lib"
mkdir build
pushd build
cmake ..                     \
-DCMAKE_BUILD_TYPE=Release   \
-DCMAKE_CXX_COMPILER=clang++ 
make

# Copy the spreading_drop_2d binary
cp tests/sandbox/spreading_drop_2d/spreading_drop_2d ..

# Copy the .py files (they should be moved so this is no longer necessary)
cp tests/sandbox/spreading_drop_2d/*.py ..
cp tests/tutorial/7_shallow_water/*.py ..

popd
chmod +x spreading_drop_2d

### "run-dexy"
dexy -loglevel DEBUG
