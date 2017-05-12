#!/bin/bash

# Package up font files w/ LICENSE.txt

datestamp=`date +%Y%m%d`
tar_filename="didont-$datestamp.tar.gz"
zip_filename="didont-$datestamp.zip"

files="LICENSE-OFL-Didont.txt Didont-Regular.otf Didont-Regular.ttf Didont-Regular.woff"

# Make a tarball
#
# A sed pattern used by tar to prepend a path to each filename.
transform_pattern=s:^:didont-$datestamp/:
gtar czvf $tar_filename --transform="$transform_pattern" $files

# Also make a .zip file.
zip $zip_filename $files
