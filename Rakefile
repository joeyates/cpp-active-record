require 'rake/cpp'

Rake::Cpp.new do |cpp|
  cpp.target               = 'libactive_record_sqlite.a'
  cpp.source_search_paths  = [ 'src' ]
  cpp.header_search_paths  = [ 'include/active_record' ]
  cpp.objects_path         = 'objects'
  cpp.include_paths        = [ './include', '/opt/local/include', '/usr/include/c++/4.2.1' ]
  cpp.library_dependencies = [ 'sqlite3' ]
end

Rake::Cpp.new do |cpp|
  cpp.task_namespace       = :test
  cpp.target               = 'test/active_record_test'
  cpp.source_search_paths  = [ 'test' ]
  cpp.header_search_paths  = [ 'test' ]
  cpp.objects_path         = 'test/lib_objects'
  cpp.include_paths        = [ './include', './test', '/opt/local/include', '/usr/include/c++/4.2.1' ]
  cpp.library_dependencies = [ 'stdc++', 'sqlite3', 'gtest', 'gtest_main', 'active_record_sqlite' ]
  cpp.library_paths        = [ '.', '/usr/local/lib' ]
  cpp.prerequesites        = [ 'rake:build' ]
  cpp.default              = [ :run ]
end

