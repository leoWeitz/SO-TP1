#!/bin/bash

# Create the "files" folder if it doesn't exist
mkdir -p files

# Loop to create 50 text files
for ((i=1; i<=50; i++))
do
    # Generate the file name
    filename="file$i.txt"

    # Create the file in the "files" folder
    touch files/$filename

    # Print the created file name
    echo "Created file: $filename"
done