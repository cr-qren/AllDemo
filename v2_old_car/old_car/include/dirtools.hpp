#ifndef dirtools_hpp
#define dirtools_hpp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include <string>
#include <iostream>

using namespace std;
int count_files(char *dir_name);

//vector<std::string> count_files(char *dir_name);

#endif
