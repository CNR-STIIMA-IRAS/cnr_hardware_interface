#!/bin/bash

declare -a StringArray=("cnr_hardware_interface")

ws=~/target_ws

cp /home/runner/work/cnr_hardware_interface/cnr_hardware_interface/codecov.yml "$ws"/

cd "$ws"

for val in ${StringArray[@]}; do

    echo "Generating coverage for '$val'"

    catkin build "$val" -v --no-deps --catkin-make-args run_tests
    catkin build "$val" --no-deps --catkin-make-args coverage_report --cmake-args -DENABLE_COVERAGE_TESTING=ON -DCMAKE_BUILD_TYPE=Debug -DCATKIN_ENABLE_TESTING=ON

done

# Remove duplicated information
find "$ws" -name \*.info.cleaned -exec rm {} \;
find "$ws" -name \*.info.removed -exec rm {} \;
