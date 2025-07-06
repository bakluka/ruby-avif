require_relative 'lib/avif/version'

Gem::Specification.new do |s|
  s.name        = "ruby-avif"
  s.version     = Avif::VERSION
  s.authors     = ["Luka Bak"]
  s.email       = "bakluka@gmail.com"

  s.summary     = "Ruby FFI binding for libavif"
  s.description = "Ractor safe Ruby FFI binding for decoding AVIF images."
  s.homepage    = "https://github.com/bakluka/ruby-avif"
  s.license     = "MIT"

  s.files         = Dir["lib/**.rb", "lib/avif/**.rb", "ext/avif/**"]
  s.require_paths = ["lib"]
  s.extensions    = ["ext/avif/extconf.rb"]

  s.required_ruby_version = ">= 3.0.0"
end