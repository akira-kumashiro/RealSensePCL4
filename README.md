# RealSensePCL4
PCLのプロジェクトをGitHubにあげるときは  
## 1. `.vcxproj`/`.vcxproj.filters`の修正  
## 2. `.gitignore`に以下の文を追加  
`CMakeFiles/generate*.*`  
`CMakeFiles/feature*.*`  
`CMakeFiles/*.txt`  
`CMakeFiles/*.check_cache`  
`CMakeFiles/*/`  
`CMakeCache.txt`  
`cmake_install.cmake`  
を忘れないようにしよう
