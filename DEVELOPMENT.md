## Compiling

- Be sure to clone and build EOS

- Create the build output dir

  ```
  mkdir build
  cd build

  ```

- CMake

  ```
  cmake -DEOS_HOME=/path/to/your/eos/root -DLLVM_DIR=/usr/local/opt/llvm@4/lib/cmake/llvm ..
  ```
  
  (note LLVM_DIR above is for Mac. For Linux use ${HOME}/opt/wasm/lib/cmake/llvm)

- Make

  ```
  make
  ```  
  

