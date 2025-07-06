# ruby-avif

[![Gem Version](https://badge.fury.io/rb/ruby-avif.svg)](https://badge.fury.io/rb/ruby-avif)  
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

A simple, Ractor and Fiber safe, Ruby binding for `libavif` to decode AVIF images.

## Requirements

This gem is a wrapper around the C library `libavif`. You must have `libavif` installed on your system before installing this gem.

**macOS (via Homebrew)**
```bash
brew install libavif
```

**Debian / Ubuntu**
```bash
sudo apt install libavif-dev
```

**Fedora / CentOS**
```bash
sudo dnf install libavif-devel
```

## Installation

Add this line to your application's Gemfile:

```ruby
gem 'ruby-avif'
```

Then execute:
```bash
bundle install
```

Or simply run:
```bash
bundle add ruby-avif
```

## Usage

The gem provides a single module method, `Avif.decode`, which takes a file path and returns the dimensions and raw pixel data of the first frame.

```ruby
require 'avif'

# Decode an AVIF image from a file path
width, height, pixel_buffer = Avif.decode('path/to/your/image.avif')

puts "Image dimensions: #{width}x#{height}"
#=> Image dimensions: 128x128

# The pixel_buffer is a frozen, binary string containing raw RGBA pixel data.
# Each pixel is represented by 4 bytes (Red, Green, Blue, Alpha).
puts "Pixel buffer size: #{pixel_buffer.bytesize} bytes"

# You can read the RGBA values by slicing and unpacking the pixel buffer
x = 10
y = 20

offset = (y * width * 4) + (x * 4)
pixel_slice = pixel_buffer[offset, 4]
pixel_rgba = pixel_slice.unpack('C*') # => [r, g, b, a]

puts "The pixel at (#{x}, #{y}) has RGBA values: #{pixel_rgba.inspect}"
#=> The pixel at (10, 20) has RGBA values: [255, 255, 255, 255]
```

## Development

After checking out the repo, you can compile the C extension by running the default rake task with:

```bash
rake default
```

or just:

```bash
rake
```

which will run ```clean```, ```clobber```, and ```compile``` tasks for you

## Contributing

Bug reports and pull requests are welcome on GitHub at [https://github.com/bakluka/ruby-avif](https://github.com/bakluka/ruby-avif).

## License

The gem is available as open source under the terms of the [MIT License](LICENSE).