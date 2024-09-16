#!/bin/bash

mkdir -p files

for ((i=1; i<=50; i++))
do
    filename="file$i.txt"
    echo "Archivo N°$i" > "files/$filename" 
    echo "Created file: $filename"
done