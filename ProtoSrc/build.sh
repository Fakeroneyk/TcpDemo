#!/bin/bash

proto_path=.

for file_name in `find $proto_path -name "*.proto"`
do
    echo "building $file_name ..."
    protoc --proto_path=$proto_path --cpp_out=../Proto "$file_name"
done
