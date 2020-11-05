pwd=`pwd`

cd ${pwd}/ProtoSrc
sh build.sh

cd ${pwd}/Proto
make clean && make

cd ${pwd}/Common
make clean && make



cd ${pwd}/DemoServer
make clean && make

cd ${pwd}/DemoClient
make clean && make

