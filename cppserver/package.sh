#!/bin/bash
dir="$(cd $(dirname $0) && pwd)"
cd $dir
mv -f $dir/bin/linux/core.* $dir/
svn up .
cd $dir/build/
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j4 2>mak.log

process=$?
if [[ $process = "0" ]]
then
    echo "build ok!"
    cd $dir/bin/linux/
    filename="genie_trunk_"$(date +%Y%m%d_%H%M)_$(svn info | awk 'NR==10{print $4}')".tar.gz"
    echo $dir/bin/linux/
    tar zcvf $filename ./* --exclude=log --exclude=launch.sh --exclude=server_config --exclude=.svn
    echo $dir/bin/linux/$filename
    scp -P 12580 $filename root@47.111.249.6:/opt/server_package/
    ##rm -f $filename
else
    echo "build error!"
fi

