require "rake/builder"

ARCHITECTURE  = "x86_64"
PROFILED      = ENV["PROFILED"]
GTEST_FILTER  = ENV["GTEST_FILTER"]
LIBRARY_PATHS = (ENV["CPPAR_LIBRARYPATHS"] || "").split(",")
VERSION       = File.read("VERSION").chomp

task :default => "build"

desc "Print help information for this Rakefile"
task :help do
  puts <<-EOT
    Run all tests:
      $ rake test
    Test only certain files:
      $ GTEST_FILTER=Foo* rake test
    (See https://github.com/google/googletest/blob/master/googletest/docs/advanced.md#running-a-subset-of-the-tests)
    Build for profiling:
      $ PROFILED=1 rake ...
    See DEVELOPMENT.md for further options.
  EOT
end

namespace :configure do
  desc "Remove configure and related files files to allow rebuild"
  task :clean do
    `rm -rf aclocal.m4 autom4te.cache configure configure.ac Makefile.am Makefile.in config.status config.h.in`
  end

  desc "Rebuild configure and associated files"
  task rebuild: [
    :clean, "configure.ac", "aclocal.m4", "config.h.in", :autoconf, :automake
  ]

  task :"configure.ac" do
    Rake::Task["autoconf"].execute(
      OpenStruct.new(project_title: "cpp-active-record", version: VERSION)
    )
  end

  task :"aclocal.m4" do
    `aclocal`
  end

  task :"config.h.in" do
    `autoheader`
  end

  task :autoconf do
    `autoconf`
  end

  task :automake do
    `automake`
  end
end

AR_DEFAULT_INCLUDES = ["include"]
AR_DEFAULT_SOURCE_SEARCH = %w(
  src src/attribute src/connection src/row src/table
)
POSTGRES_INCLUDE_PATHS = ["include/postgresql", "include/postgresql/server"]

[
  {
    namespace: nil,
    include: AR_DEFAULT_INCLUDES + POSTGRES_INCLUDE_PATHS,
    options: ["-DAR_POSTGRES", "-DAR_SQLITE"],
    source: AR_DEFAULT_SOURCE_SEARCH + ["src/postgres", "src/sqlite"]
  },
  {
    namespace: :postgres,
    include: AR_DEFAULT_INCLUDES + POSTGRES_INCLUDE_PATHS,
    options: ["-DAR_POSTGRES"],
    source: AR_DEFAULT_SOURCE_SEARCH + ["src/postgres"]
  },
  {
    namespace: :sqlite,
    include: AR_DEFAULT_INCLUDES,
    options: ["-DAR_SQLITE"],
    source: AR_DEFAULT_SOURCE_SEARCH + ["src/sqlite"]
  }
].each do |build|
  name = "libactive_record"
  name += "_#{build[:namespace]}" if build[:namespace]
  name += "_#{ARCHITECTURE}"
  name += "_profiled" if PROFILED
  target = name + ".a"

  options = build[:options].clone
  options += ["-pg"] if PROFILED

  Rake::Builder.new do |builder|
    builder.task_namespace       = build[:namespace]
    builder.target               = target
    builder.architecture         = ARCHITECTURE
    builder.source_search_paths  = build[:source]
    builder.installable_headers  = ["include/**/*.h"]
    builder.objects_path         = "objects/#{name}"
    builder.include_paths        = build[:include]
    builder.compilation_options  = options
    builder.library_paths        = LIBRARY_PATHS
  end
end

TEST_NAME                = "#{ARCHITECTURE}_test"
TEST_SOURCE_SEARCH_PATHS = ["test", "test/base"]

test_target_parameters =
  if GTEST_FILTER
    ["--gtest_filter=#{GTEST_FILTER}"]
  else
    []
  end

Rake::Builder.new do |builder|
  builder.task_namespace       = :test
  builder.target               = "./active_record_#{TEST_NAME}"
  builder.architecture         = ARCHITECTURE
  builder.source_search_paths  = TEST_SOURCE_SEARCH_PATHS
  builder.installable_headers  = ["test"]
  builder.objects_path         = "test/objects/#{TEST_NAME}"
  builder.include_paths        = ["include"] + POSTGRES_INCLUDE_PATHS + ["test"]
  builder.linker_options       = ["-L."]
  builder.library_dependencies = [
    "gtest", "active_record_#{ARCHITECTURE}", "pq", "sqlite3", "pthread"
  ]
  builder.compilation_options  = ["-DAR_POSTGRES", "-DAR_SQLITE"]
  builder.library_paths        = LIBRARY_PATHS + ["."]
  builder.target_prerequisites = [:"rake:build"]
  builder.default_task         = :run
  builder.target_parameters    = test_target_parameters
end
