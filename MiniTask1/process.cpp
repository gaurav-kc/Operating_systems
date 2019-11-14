#include<unistd.h>
#include<limits.h>
#include<string>
#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<sstream>
#include<dirent.h>
#include<iomanip>
using namespace std;

void getFileList(string path,vector<string> &files)
{
	DIR *dir;
	struct dirent *ent;
	string temp;
	if ((dir = opendir(path.c_str())) != NULL) 
	{
		while((ent = readdir (dir)) != NULL) {
			temp = ent->d_name;
			if(temp[0]=='.')
				continue;
	    	files.push_back(ent->d_name);
	  	}
	 	closedir (dir);
	}else
	{
		cout<<"Error opening directory";
	}
}


string do_readlink(string const& path) {
    char buff[PATH_MAX];
    ssize_t len = ::readlink(path.c_str(), buff, sizeof(buff)-1);
    if (len != -1) {
      buff[len] = '\0';
      return std::string(buff);
    }
}

void printProcessStats(string pid)
{
	string filePath = "/proc/" + pid + "/status";
	bool NAME=true,STATE=true,PPID=true;
	ifstream fileInput1,fileInput2,fileInput3,fileInput4,fileInput5,fileInput6,fileInput7;
	fileInput1.open(filePath);
	string line,name,state,ppid;
	string nsearch = "Name:";
	int current = 0;
	while(getline(fileInput1, line)) 
	{
    	if (line.find(nsearch) != string::npos) {
        	name = line.substr(6,(line.length() - 6));
    		}
    }
    string ssearch = "State:";
    fileInput2.open(filePath);
    while(getline(fileInput2, line)) 
	{
    	if (line.find(ssearch) != string::npos) {
        	state = line.substr(7,1);
    		}
    }
    string psearch = "PPid:";
    fileInput3.open(filePath);
    while(getline(fileInput3, line)) 
	{
    	if (line.find(psearch) != string::npos) {
        	ppid = line.substr(6,(line.length() - 6));
    		}
    }
    string sesspath = "/proc/"+pid+"/sessionid";
    fileInput4.open(sesspath);
    getline(fileInput4,line);
    string sessionid = line;
    string environpath = "/proc/"+pid+"/environ";
    string environ = "";
    fileInput5.open(environpath);
    while(getline(fileInput5,line))
    	environ = environ + line;
    string fdpath = "/proc/"+pid+"/fd/";
    vector<string> filelist;
    getFileList(fdpath,filelist);
    auto it = filelist.begin();
    string cwdpath = "/proc/"+pid+"/cwd";
    string cwd = do_readlink(cwdpath);
    cout<<"Name : "<<name<<"\nState : "<<state<<"\nPPID : "<<ppid<<"\nSessionID : "<<sessionid<<"\nEnvironment var : "<<environ<<"\nFile descriptors : ";
   	while(it!=filelist.end())
    {
    	cout<<*it<<" ";
    	it++;
    }
    cout<<endl;
    cout<<"Process root folder : "<<cwd<<endl;
}
	
void printGeneralStats()
{
	string path = "/proc/stat";
	ifstream fileInput1,fileInput2,fileInput3;
	fileInput1.open(path);
	fileInput2.open(path);
	fileInput3.open(path);
	string csearch = "ctxt";
	string prsearch = "procs_running";
	string pbsearch = "procs_blocked";
	string context,procs_running,procs_blocked;
	string line;
	while(getline(fileInput1,line))
	{
		if(line.find(csearch)!= string::npos){
			context = line.substr(5,(line.length()-5));
		}
	}
	while(getline(fileInput2,line))
	{
		if(line.find(prsearch)!= string::npos){
			procs_running = line.substr(14,(line.length()-14));
		}
	}
	while(getline(fileInput3,line))
	{
		if(line.find(pbsearch)!= string::npos){
			procs_blocked = line.substr(14,(line.length()-14));
		}
	}
	cout<<"Context Switches : "<<context<<"\nRunning proc : "<<procs_running<<"\nBlocked proc : "<<procs_blocked<<endl;
}

int main(int argc, char *argv[])
{
	clock_t start, end;
	start = clock(); 
	if(argc>1)
		printProcessStats(argv[1]);
	else{
		vector<string> files;
		getFileList("/proc/",files);
		auto it = files.begin();
		while(it!=files.end())
		{
			string s = *it;
			stringstream ss(s);
			int x=0;
			ss>>x;
			if(x>0 && x<17000)
				printProcessStats(to_string(x));
		}
	}
	printGeneralStats();
	end = clock(); 
	double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
	cout << "Time taken by program is : "<<fixed << time_taken << setprecision(5); 
    cout << " sec " << endl; 
	return 0;
}