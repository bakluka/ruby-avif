require 'rake/extensiontask'

Rake::ExtensionTask.new("avif") do |task|
  task.lib_dir = "lib/avif"
end

task default: [ :clean, :clobber, :compile ]