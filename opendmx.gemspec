Gem::Specification.new do |s|
  s.name = "opendmx"
  s.version = "0.0.1"
  s.date = "2015-09-25"
  s.summary = "A Ruby interface for EntTec Open DMX"
  # s.require_paths = ["lib"]
  s.email = "justin@thethinkingtree.com"
  s.authors = ["Justin Schumacher"]
  s.description = "Sends DMX commands to EntTec Open DMX USB interface"
  # s.required_ruby_version = Gem::Version::Requirement.new(">=1.9.1")
  s.files = ["ext/extconf.rb", "ext/opendmx.c", "ext/ftd2xx.h", "ext/WinTypes.h", "opendmx.gemspec"]
  s.extensions = ["ext/extconf.rb"]
end