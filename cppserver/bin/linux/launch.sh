#!/bin/bash
launch_dir="$(cd $(dirname $0) && pwd)"
kill -9 $(pgrep -d\  tcore3)

echo "launche string...."

./tcore3 --name=master --env=env --console_ip=172.17.1.24 --console_port=8888
./tcore3 --name=center --noder_ip=172.17.1.24 --noder_port=9100 --noder_area=1 --allarea --env=env
./tcore3 --name=slb --noder_ip=172.17.1.24 --noder_port=9200 --noder_area=1 --slb_port=10000 --max_ssize=64 --max_rsize=8 --env=env
./tcore3 --name=gate --noder_ip=172.17.1.24 --noder_port=9300 --noder_area=1 --door_ip=172.17.1.24 --door_port=28001 --max_ssize=64 --max_rsize=8 --env=env
./tcore3 --name=gate --noder_ip=172.17.1.24 --noder_port=9301 --noder_area=1 --door_ip=172.17.1.24 --door_port=28002 --max_ssize=64 --max_rsize=8 --env=env
./tcore3 --name=game --noder_ip=172.17.1.24 --noder_port=9400 --noder_area=1 --allarea --env=env --pause
./tcore3 --name=cache --noder_ip=172.17.1.24 --noder_port=9500 --noder_area=1 --allarea --env=env --cache_id=1
./tcore3 --name=cache --noder_ip=172.17.1.24 --noder_port=9501 --noder_area=1 --allarea --env=env --cache_id=2