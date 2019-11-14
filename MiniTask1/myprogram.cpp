
#include<iostream>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<vector>
#include<iomanip>
#define partitionSize 4294967296
// onegb 	1073741824
// fourgb	4294967296
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


string getFileName(string path,bool isSmall)
{
	static int fcount = 0;
	if(isSmall)
	{
		int i=0,count=0,j,temp;
		while(path[i++] != '\0');
		i--;
		while(path[i--] != '/')
			count++;
		i++;
		j=0;
		string name = "";
		while(count--)
			name = name + path[i++];
		return name;
	}else{
		int i=0,count=0,j,temp;
		while(path[i++] != '\0');
		i--;
		while(path[i--] != '/')
			count++;
		i++;
		j=0;
		string name = "";
		while(path[i] != '.')
			name = name + path[i++];
		name = name + "_" + to_string(fcount);
		fcount++;
		while(path[i]!='\0')
			name = name + path[i++];
		return name;
	}
}

string getSourceName(string path)
{
	static int fcount = 0;
	int i=0,count=0,j,temp;
	while(path[i++] != '\0');
	i--;
	while(path[i--] != '/')
		count++;
	i+=2;
	j=0;
	string name = "";
	while(path[i] != '.')
		name = name + path[i++];
	name = name + "_" + to_string(fcount);
	fcount++;
	while(path[i]!='\0')
		name = name + path[i++];
	return name;
}

int main(int argc, char *argv[])
{
	//check arguments are correct or not
	clock_t start, end;
	start = clock(); 
	if(argc<3)
	{
		cout<<"Error in number of arguments";
		return 2;
	}
	//check destination is directory or not
	int fd_dir,fd_filer,fd_filew;
	int errno;
	bool MERGE = false;
	string lastarg = argv[argc-1];
	if(lastarg[0]=='-' && lastarg[1] == 'm')
	{
		MERGE = true;
		argc = argc-1;
	}
	fd_dir = open(argv[argc-1],O_DIRECTORY,0700);
	if(fd_dir == -1)
	{
		cout<<"\nOpen Failed. Error : "<<strerror(errno);
		return 3;
	}
	int i,byte_c,filecount=0,parts;
	long long int size,byteCount=0;
	string name,destination,fcount;
	char buffer[1024];
	fd_filer = open(argv[1],O_RDONLY);
	size=0;
	while((byte_c = read(fd_filer,buffer,1024)) > 0)
		size = size + byte_c;
	parts = size/partitionSize;
	//0 to parts will be all parts of file
	for(i=1;i<(argc-1);i++)
	{
		if(parts == 0)
		{
			name = getFileName(argv[i],true);
			fd_filer = open(argv[i],O_RDONLY);
			destination = argv[argc-1] + name;
			int n = destination.length();
			char dest[n+1];
			strcpy(dest,destination.c_str());
			fd_filew = open(dest, O_CREAT| O_WRONLY, 0700);
			while((byte_c = read(fd_filer,buffer,1024)) > 0)
			{
				write(fd_filew,buffer,byte_c);
			}
		}else
		{
			name = getFileName(argv[i],false);
			fd_filer = open(argv[i],O_RDONLY);
			destination = argv[argc-1] + name;
			int n = destination.length();
			char dest[n+1];
			strcpy(dest,destination.c_str());
			fd_filew = open(dest, O_CREAT| O_WRONLY, 0700);
			while((byte_c = read(fd_filer,buffer,1024))>0)
			{
				byteCount = byteCount + byte_c;
				if(byteCount<partitionSize)
				{
					write(fd_filew,buffer,byte_c);
				}else{
					byteCount = 0;
					name = getFileName(argv[i],false);
					destination = argv[argc-1] + name;
					int n = destination.length();
					char dest[n+1];
					strcpy(dest,destination.c_str());
					fd_filew = open(dest, O_CREAT| O_WRONLY, 0700);
				}
			}
		}
	}

	if(MERGE)
	{
		name = getFileName(argv[1],true);
		destination = argv[argc-1]+name;
		int n = destination.length();
		char dest[n+1];
		strcpy(dest,destination.c_str());
		fd_filew = open(dest, O_CREAT| O_WRONLY, 0700);
		for(i=0;i<=parts;i++)
		{
			string source = argv[argc-1] + getSourceName(argv[1]);
			int n = source.length();
			char sourc[n+1];
			strcpy(sourc,source.c_str());
			fd_filer = open(sourc,O_RDONLY);
			while((byte_c = read(fd_filer,buffer,1024))>0)
			{
				write(fd_filew,buffer,byte_c);
			}
			int rem = remove(sourc);
			if(rem != 0)
				cout<<"Error Deleting"<<endl;
		}
	}else{
		return 0;
	}
	end = clock(); 
	double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
	cout << "Time taken by program is : "<<fixed << time_taken << setprecision(5); 
    cout << " sec " << endl; 
}