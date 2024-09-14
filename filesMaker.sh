#!/bin/bash

# Create the "files" folder if it doesn't exist
mkdir -p files

# Loop to create 50 text files
for ((i=1; i<=50; i++))
do
    # Generate the file name
    filename="file$i.txt"

    # Generate random content and sleep for 1 second
    echo "Podonga $i" > "files/$filename" 

    # Print the created file name
    echo "Created file: $filename"
done