cd ./build
# add a flag to do cmake
while getopts ":cr" opt; do
  case $opt in
    c)
      echo "cmake"
      cmake ..
      ;;
    # add flag for release option
    r)
      echo "cmake with Release config"
      cmake -DCMAKE_BUILD_TYPE=Release ..
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
  esac
done
make -j${nproc}
cd ..
