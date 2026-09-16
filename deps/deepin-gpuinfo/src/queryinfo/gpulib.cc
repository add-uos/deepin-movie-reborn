/* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co., Ltd.
 * file name:  gpulib.cc
 * Author:     shuaijie <shuaijie@uniontech.com> 
 * Maintainer: shuaijie <shuaijie@uniontech.com>
 * descrition:  This file is part of deepin-gpuinfo. 
 *
 * This program is free software: you can redistribute it and/or modify,it underthe terms  of the GNU General Public 
 *  License as published by the Free Software Foundation, either version 3 of the License, or any later version. 
 *
 * This program is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even  the implied warranty 
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.You should have
 * received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

 #include "version.h"
 #include "gpulib.h"  
// #include "sysfs.h"
#include "osutils.h"
#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <map>
#include <iostream>

//#define PREFIX "../config"  
#include "../lspci/sysfs_pcidev.h"
#define DATADIR  PREFIX 

using namespace std;

#define INFOID_PATH DATADIR"/share/gpuinfolib.csv:/usr/share/deepin-gpuinfo/gpuinfolib.csv:/usr/local/share/deepin-gpuinfo/gpuinfolib.csv:../config/gpuinfolib.csv"

static bool infodb_loaded = false;

static map < string, string > info_ids;


static void parse_info_ids(const vector < string > & lines)
{
  
  for (vector < string >::const_iterator it = lines.begin();
      it != lines.end(); ++it)
  {
    const string & line = *it;
    if (line.length() < 1 || line.at(0) == ';'|| line.at(0) == '#')
      continue;
  
    string id = line.substr(0, 9);
    
    id[0] = tolower(id[0]);
    id[1] = tolower(id[1]);
    id[2] = tolower(id[2]);
    id[3] = tolower(id[3]);
    id[4] = tolower(id[4]);
    id[5] = tolower(id[5]);
    id[6] = tolower(id[6]);
    id[7] = tolower(id[7]);
    id[8] = tolower(id[8]);
          
    string desc = line.substr(10);
    info_ids[id] = desc;
  }
}


static void parse_info_ids_allsummary(const vector < string > & lines)
{
  
  for (vector < string >::const_iterator it = lines.begin();
      it != lines.end(); ++it)  {
    const string & line = *it;
    if (line.length() < 1 || line.at(0) == ';')
      continue;
    string id = line.substr(0, 10);    
    id[0] = tolower(id[0]);
    id[1] = tolower(id[1]);
    id[2] = tolower(id[2]);
    id[3] = tolower(id[3]);
    id[4] = tolower(id[4]);
    id[5] = tolower(id[5]);
    id[6] = tolower(id[6]);
    id[7] = tolower(id[7]);
    id[8] = tolower(id[8]);
    
        
    string desc = line.substr(10,40);
    cout <<  "--" << desc << endl;
    info_ids[id] = desc;
     cout <<  "::"<< id<< endl;
  }
}

static void load_infodb()
{
  vector < string > lines;
  vector < string > filenames;
  //bool is_fileload =false;

  filenames.clear();
  splitlines(INFOID_PATH, filenames, ':');

  for (int i = filenames.size() - 1; i >= 0; i--)  {
    lines.clear();
    if (loadfile(filenames[i], lines))
     {    
       parse_info_ids(lines);
        infodb_loaded = true;
     }
     
  }

  if(false == infodb_loaded) {
    printf("no gpuinfolib.csv ,please check /usr/share/deepin-gpuinfo/gpuinfolib.csv. \n");
    return;
  }
  
}

extern "C" void gpulib_allshow(void);

void gpulib_allshow(void)
{
 
  if(!infodb_loaded)  
    load_infodb();

  map < string, string >::const_iterator it;
  for (it = info_ids.begin();  it != info_ids.end(); it++)  {
       cout<<it->first.substr(0,120)<<" "<<it->second.substr(0,120)<<endl;
  }
}

//1002:6611   必须大写
void  gpulib_getdescription(const string & search_XvendorXdev)
{ 

  if (!infodb_loaded)
    load_infodb();

    string id = search_XvendorXdev;
    id[0] = tolower(id[0]);
    id[1] = tolower(id[1]);
    id[2] = tolower(id[2]);
    id[3] = tolower(id[3]);
    id[4] = tolower(id[4]);
    id[5] = tolower(id[5]);
    id[6] = tolower(id[6]);
    id[7] = tolower(id[7]);
    id[8] = tolower(id[8]);
    map < string, string >::const_iterator lookup = info_ids.find(id);
    if (lookup != info_ids.end())    {
      cout<<id<<" "<<lookup->second.substr(0)<<endl;   
    }  
}
extern "C"  void gpulib_search_description(char* ss);
void gpulib_search_description(char* ss)
{
  string  search_XvendorXdev;
 // printf("ss:%s\n",ss);
  if(ss)  {
     search_XvendorXdev= ss;
     cout << "search_pci [vendor:dev] " << search_XvendorXdev << endl;
     gpulib_getdescription(search_XvendorXdev);
  }
}

extern "C" void QueryCurrentGraphicsInfo(void);
void QueryCurrentGraphicsInfo(void)
{
  struct  pci_dev_driver tmp;
  char tmpc[32];
   
  unsigned int i = sysfs_readMediaDevice_number();
  /*打印输出*/
  printf("\n# show current system Multimedia device library info:\n");
  for (unsigned int j = 0; j < i; j++)  {   
                           
    //if(PCI_BASE_CLASS_DISPLAY == (stp->idclass>>8))     
    {
       tmp = sysfs_read_MediaDevInfo(j); 
       snprintf(tmpc,16,"%04x:%04x",tmp.base.vendor_id,tmp.base.device_id);
 
      printf("--------------------------------------------\n");  
      gpulib_search_description(tmpc);
    }
   
  }
  return;
}
