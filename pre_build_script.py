#!/usr/bin/env python3

"""
This script generates the index_src.h file from index.html and webpage_src.template
since the HTML file is easier to edit separately.
Run this script after editing index.html to update index_src.h
so that the changes will be included in the build.
"""

import os
import shutil

# Change to the src directory
# os.chdir("src")

split_marker = "1234567\n"
template_file = "src/webpage_src.template"
tmp_file = "src/test.h"
output_file = "src/index_src.h"
input_file = "src/index.html"
# Read up to and including the marker, but do not include lines with the marker itself
with open(template_file, "r", encoding="utf-8") as infile, open(tmp_file, "w", encoding="utf-8") as outfile:
    for line in infile:
        if split_marker.strip() in line:
            break
        if split_marker.strip() not in line:
            outfile.write(line)

# Append all contents of index.html to the temp file
with open(input_file, "r", encoding="utf-8") as htmlfile, open(tmp_file, "a", encoding="utf-8") as outfile:
    shutil.copyfileobj(htmlfile, outfile)

# Append the rest of the template file after the marker
found_marker = False
with open(template_file, "r", encoding="utf-8") as infile, open(tmp_file, "a", encoding="utf-8") as outfile:
    for line in infile:
        if not found_marker:
            if split_marker.strip() in line:
                found_marker = True
            continue
        outfile.write(line)

# Move the temporary file to override index_src.h
os.replace(tmp_file, output_file)
