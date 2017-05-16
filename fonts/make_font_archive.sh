#!/bin/bash

# Package up font files w/ LICENSE.txt

datestamp=`date +%Y%m%d`
tar_filename="tuffy-$datestamp.tar.gz"
zip_filename="tuffy-$datestamp.zip"

files="LICENSE.txt tuffy_regular.sfd tuffy_bold.sfd tuffy_bold_italic.sfd tuffy_italic.sfd Tuffy.ttf Tuffy_Bold.ttf Tuffy_Bold_Italic.ttf Tuffy_Italic.ttf Tuffy.otf Tuffy_Bold.otf Tuffy_Bold_Italic.otf Tuffy_Italic.otf"

# Make a tarball
#
# A sed pattern used by tar to prepend a path to each filename.
transform_pattern=s:^:tuffy-$datestamp/:
gnutar czvf $tar_filename --transform="$transform_pattern" $files

# Also make a .zip file.
zip $zip_filename $files
