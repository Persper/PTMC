add_executable(not
  not.cpp
  )

target_link_libraries(not LLVMSystem)
if( MINGW )
  target_link_libraries(not imagehlp psapi)
endif( MINGW )
if( LLVM_ENABLE_THREADS AND HAVE_LIBPTHREAD )
  target_link_libraries(not pthread)
endif()
