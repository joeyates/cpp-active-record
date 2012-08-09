require 'rake/builder'

Rake::Builder.new do | builder |
  builder.task_namespace       = :i386
  builder.target               = 'libactive_record_sqlite_i386.a'
  builder.architecture         = 'i386'
  builder.source_search_paths  = [ 'src' ]
  builder.header_search_paths  = [ 'include/**/*.h' ]
  builder.objects_path         = 'objects_i386'
  builder.include_paths        = [ 'include' ]
  builder.library_dependencies = [ 'sqlite3' ]
end

Rake::Builder.new do | builder |
  builder.task_namespace       = :x86_64
  builder.target               = 'libactive_record_sqlite_x86_64.a'
  builder.architecture         = 'x86_64'
  builder.source_search_paths  = [ 'src' ]
  builder.header_search_paths  = [ 'include/**/*.h' ]
  builder.objects_path         = 'objects_x86_64'
  builder.include_paths        = [ 'include' ]
  builder.library_dependencies = [ 'sqlite3' ]
end

Rake::Builder.new do | builder |
  builder.task_namespace       = :x86_64_profiled
  builder.target               = 'libactive_record_sqlite_x86_64_profiled.a'
  builder.architecture         = 'x86_64'
  builder.source_search_paths  = [ 'src' ]
  builder.header_search_paths  = [ 'include/**/*.h' ]
  builder.compilation_options  = [ '-pg' ]
  builder.objects_path         = 'objects_x86_64_profiled'
  builder.include_paths        = [ 'include' ]
  builder.library_dependencies = [ 'sqlite3' ]
end

Rake::Builder.new do | builder |
  builder.task_namespace       = :test
  builder.target               = 'active_record_test'
  builder.architecture         = 'x86_64'
  builder.source_search_paths  = [ 'test' ]
  builder.header_search_paths  = [ 'test' ]
  builder.objects_path         = 'test/objects_test'
  builder.include_paths        = [ 'include', 'test' ]
  builder.library_dependencies = [ 'gtest', 'active_record_sqlite_x86_64', 'sqlite3' ]
  builder.library_paths        = [ 'objects_x86_64' ]
  builder.linker_options       = [ '-L.' ]
  builder.target_prerequisites = [ :'rake:x86_64' ]
  builder.default_task         = [ :run ]
end

Rake::Builder.new do | builder |
  builder.task_namespace       = :single_test
  builder.target               = 'single_test'
  builder.architecture         = 'x86_64'
  builder.source_search_paths  = [ 'test/base_test.cpp', 'test/test_helper.cpp', 'test/main.cpp' ]
  builder.header_search_paths  = [ 'test' ]
  builder.objects_path         = 'test/objects_single_test'
  builder.include_paths        = [ 'include', 'test' ]
  builder.library_dependencies = [ 'gtest', 'active_record_sqlite_x86_64', 'sqlite3' ]
  builder.library_paths        = [ 'objects_x86_64' ]
  builder.linker_options       = [ '-L.' ]
  builder.target_prerequisites = [ :'rake:x86_64' ]
  builder.default_task         = [ :run ]
end
