require 'mkmf'

abort "libavif not found. Please install libavif." unless have_library("avif", "avifDecoderCreate")
$CFLAGS << ' -std=c99'

create_makefile("avif")