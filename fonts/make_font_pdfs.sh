#!/bin/bash

# Generate PDF proofs.

# fontplot is from the Adobe FDK

datestamp=`date +%Y%m%d`
#zip_filename="tuffy-$datestamp.zip"

args='--drawGlyph_GlyphBox 0 --drawGlyph_EMBox 1 --drawMeta_Hints 0 --drawMeta_SideBearings 0 --metaDataAboveGlyph 0 --drawGlyph_Baseline 1 --descenderSpace -100 --drawGlyph_XAdvance 1 --metaDataFont Helvetica --pageTitleFont Helvetica --pageTitleSize 10 --glyphHPadding 20 --glyphVPadding 20 --openPDFWhenDone 0'
fontplot $args -o Tuffy-$datestamp.pdf Tuffy.otf
fontplot $args -o Tuffy_Italic-$datestamp.pdf Tuffy_Italic.otf
fontplot $args -o Tuffy_Bold-$datestamp.pdf Tuffy_Bold.otf
fontplot $args -o Tuffy_Bold_Italic-$datestamp.pdf Tuffy_Bold_Italic.otf
