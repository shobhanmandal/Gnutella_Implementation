//gcc hello.c -o hello1
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <mutex>
#include <pthread.h> 
#include <bits/stdc++.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

struct fileDetails
{
	string relativePath;
	string clientAlias;
}tempFileDetails;

struct clientDetails
{
	string clientIP;
	int clientPort;
	int clientDownloadPort;
	time_t timer;
}tempClientDetails;

map<string,vector <fileDetails>> repositoryFile;
map<string,clientDetails> activeClients;
int repoFileDescriptor,clientFileDescriptor;
char *listFile;
char *repoFile;
mutex mtx;
//

void fileSave()
{
	
	char *text;
	int i=0,j;
	clientFileDescriptor=open(listFile,O_RDWR|O_TRUNC);
	repoFileDescriptor=open(repoFile,O_RDWR|O_TRUNC);
	if(clientFileDescriptor==-1)
		cout<<"\n-----Issue in File Saving-------\n";
	else
	{
		 ofstream out(listFile);	
		 for(auto iter = activeClients.begin(); iter != activeClients.end(); ++iter )
		 {
				string str=iter->first;
				clientDetails temp=iter->second;
				str+=":"+temp.clientIP+":"+to_string(temp.clientPort)+":"+to_string(temp.clientDownloadPort)+"\n";
				out << str;
		 }
		 out.close();
		 
		 ofstream out1(repoFile);
		 for(auto iter = repositoryFile.begin(); iter != repositoryFile.end(); ++iter )
		 {
			string str=iter->first,str1;
			vector <fileDetails> temp=iter->second;
			for(fileDetails i:temp)
			{
				str1=str+":"+i.relativePath+":"+i.clientAlias+"\n";
				out1<<str1;
			}
		 }
		 out1.close();	  	
    }
   
    cout<<"\n-----File Saved-------\n";
}

//It will read the files into the datastructure
void readFiles(char *repoFile,char *listFile)
{
	//repoFile="/Client&Server/repo.txt";
	if(access(repoFile,F_OK)!=-1)
	{
		repoFileDescriptor=open(repoFile,O_RDONLY);
		if(repoFileDescriptor==-1)
			printf("\n *****Error in Opening the Repo file***********");
		else
		{
			string line,token;
			ifstream fileRead(repoFile);
			vector <string> tokenVector;
			while (getline(fileRead, line))
			{
			  stringstream temp(line);
			  while(getline(temp,token,':'))
			  {
				  tokenVector.push_back(token);
			  }
			  //cout<<tokenVector[0]<<":"<<tokenVector[1]<<":"<<tokenVector[2]<<"\n";
			  
			  if(repositoryFile.find(tokenVector[0])==repositoryFile.end())
			  {
				  tempFileDetails.relativePath=tokenVector[1];
				  tempFileDetails.clientAlias=tokenVector[2];
				  vector <fileDetails> temp;
				  temp.push_back(tempFileDetails);
				  repositoryFile.insert(pair<string,vector <fileDetails>>(tokenVector[0],temp));
			  }
			  else
			  {
				  tempFileDetails.relativePath=tokenVector[1];
				  tempFileDetails.clientAlias=tokenVector[2];
				  vector <fileDetails> temp=repositoryFile.at(tokenVector[0]);
				  temp.push_back(tempFileDetails);
				  repositoryFile[tokenVector[0]]=temp;
			  }
			  tokenVector.clear();	
			} 
		}
		/*cout<<"\n---------------Repository File---------------------\n";
		for(auto iter = repositoryFile.begin(); iter != repositoryFile.end(); ++iter )
		{
			string str=iter->first;
			cout<<str<<"::::::::";
			vector <fileDetails> temp=iter->second;
			cout<<temp.size()<<"\n";
			for(fileDetails i:temp)
			{
				cout<<i.relativePath<<":"<<i.clientAlias<<"\n";
			}
		}*/	
	}
	else
	{
		cout<<"\n----New Repository File Created----\n";
		repoFileDescriptor=open(repoFile,O_RDWR|O_CREAT);
	}

//--------------------------------------------------------------------------------------
	
	if(access(listFile,F_OK)!=-1)
	{
		clientFileDescriptor=open(listFile,O_RDONLY);
		if(clientFileDescriptor==-1)
			printf("\n *****Error in Opening the List file***********");
		else
		{
			string line,token;
			ifstream fileRead(listFile);
			vector <string> tokenVector;
			while (getline(fileRead, line))
			{
			  stringstream temp(line);
			  while(getline(temp,token,':'))
			  {
				  tokenVector.push_back(token);
			  }
			  tempClientDetails.clientIP=tokenVector[1];
			  tempClientDetails.clientPort=stoi(tokenVector[2]);
			  tempClientDetails.clientDownloadPort=stoi(tokenVector[3]);
			  time(&tempClientDetails.timer);
			  activeClients.insert(pair<string,clientDetails>(tokenVector[0],tempClientDetails));
			  
			  tokenVector.clear();
		    }
		}
		/*cout<<"\n---------------Client List File---------------------\n";
		for(auto iter = activeClients.begin(); iter != activeClients.end(); ++iter )
		{
			string str=iter->first;
			cout<<str<<"::::::::";
			clientDetails temp=iter->second;
			cout<<temp.clientIP<<":"<<temp.clientPort<<":"<<temp.clientDownloadPort<<"\n";
		}*/
	}
	else
	{
		cout<<"\n----New Client List File Created----\n";
		clientFileDescriptor=open(listFile,O_RDWR|O_CREAT);
	}
}

string search(string text)
{
	string returnValue,previousStr,str,repoStr="",searchStr="";
	int counter=0,flag=0;
	
	vector <fileDetails> temp;
	
		for(auto iter = repositoryFile.begin(); iter != repositoryFile.end(); ++iter )
		{
			str=iter->first;
			repoStr="",searchStr="";
			for(int i=0;i<text.size();i++)
			{
				repoStr+=tolower(str.at(i));
				searchStr+=tolower(text.at(i));
			}
			//cout<<"Repo String :"<<repoStr<<": Search String :"<<searchStr<<":\n";
			if(repoStr.compare(searchStr) == 0)
			{
				temp=iter->second;
				flag=1;
				//cout<<temp.size()<<"\n";
				for(fileDetails i:temp)
				{
					 previousStr=returnValue;	
					 returnValue+="["+to_string(counter+1)+"] "+str+":"+i.relativePath+":"+i.clientAlias+":";	
					 if(activeClients.find(i.clientAlias)==activeClients.end())
					 	returnValue=previousStr; 
					 else
					 {
						 tempClientDetails=activeClients.at(i.clientAlias);
						 returnValue+=tempClientDetails.clientIP+":"+to_string(tempClientDetails.clientPort)+":"+to_string(tempClientDetails.clientDownloadPort)+'\n';
						 counter++;
					 }
				//cout<<i.relativePath<<":"<<i.clientAlias<<"\n";
				}
			}
		}
		
		if(counter==0&&flag==0)
			returnValue="FAILURE:No Match Found\n";
		else if(counter==0&&flag==1)
		   	returnValue="FAILURE:No Active Client to Address Request\n";
		else
		    returnValue="SUCCESS:Found:"+to_string(counter)+"\n"+returnValue;
	
	return returnValue;	       		
}

string execute(vector <string> tokenVector)
{
	string returnString="";
	if(activeClients.find(tokenVector[1])==activeClients.end())
		returnString="FAILURE:CLIENT_OFFLINE\n";
	else
	{
		tempClientDetails=activeClients.at(tokenVector[1]);
		returnString+=tempClientDetails.clientIP+":"+to_string(tempClientDetails.clientPort)+":"+tokenVector[2];//+":"+to_string(tempClientDetails.clientDownloadPort)+"||Success:"+relativePath; 
	}
	return returnString;	
}

string share(vector <string> tokenVector)
{
	mtx.lock();
	for(string temp:tokenVector)
		cout<<temp<<" : ";
	cout<<"\n";
	string str= tokenVector[1].substr(tokenVector[1].find_last_of("/")+1);
	//cout<<"Share Handling.... "<<str<<" : "<<tokenVector[1]<<"\n";
	 if(repositoryFile.find(str)==repositoryFile.end())
	  {
		  tempFileDetails.relativePath=tokenVector[1];
		  tempFileDetails.clientAlias=tokenVector[2];
		  vector <fileDetails> temp;
		  temp.push_back(tempFileDetails);
		  repositoryFile.insert(pair<string,vector <fileDetails>>(str,temp));
	  }
	  else
	  {
		  tempFileDetails.relativePath=tokenVector[1];
		  tempFileDetails.clientAlias=tokenVector[2];
		  vector <fileDetails> temp=repositoryFile.at(str);
		  temp.push_back(tempFileDetails);
		  repositoryFile[str]=temp;
	  }
	  mtx.unlock();	  
	  return "SUCCESS:FILE_SHARED\n";
}

string deleting(vector <string> tokenVector)
{
	mtx.lock();
	string str= tokenVector[1].substr(tokenVector[1].find_last_of("/")+1);
	
	string returnString="";
	int position=0;
	if(repositoryFile.find(str)!=repositoryFile.end())
	  {
		 /* tempFileDetails.relativePath=tokenVector[1];
		  tempFileDetails.clientAlias=tokenVector[2];*/
		  vector <fileDetails> temp=repositoryFile.at(str);
		  
		  if (temp.size()>1)
		  {
			for (fileDetails i:temp)
			{
			  if(i.clientAlias==tokenVector[2])
			   { //cout<<"\n-------------------------------Match Ho gya re--------------------------------------------------\n";
				   break;}
			  position++;  
			}
			temp.erase(temp.begin()+position);
			repositoryFile[str]=temp;
			//repositoryFile.insert(pair<string,vector <fileDetails>>(str,temp));
		  }
		  else
		    repositoryFile.erase(str);
		  
		  returnString="SUCCESS:FILE_REMOVED\n";
	  }
	  else
	  	  returnString="FAILURE:FILE_NOT_FOUND\n";
	  
	mtx.unlock();
	return returnString;
}

string get(vector <string> tokenVector)
{
	string returnString="";
	string str=tokenVector[3],relativePath;
	int check=0;
	if(tokenVector.size()==4)
	{
		if(repositoryFile.find(str)!=repositoryFile.end())
	    {
			vector <fileDetails> temp=repositoryFile.at(str);
			  for (fileDetails i:temp)
			  {
			    if(i.clientAlias==tokenVector[1]&&i.relativePath==tokenVector[2])
			    { 
					check=1;
					str=i.clientAlias;
					relativePath=i.relativePath;
					break;
				}
		      }
		      if(check==0)
		       returnString="FAILURE:FILE_NOT_FOUND_FOR_ALIAS\n";
		      else
		      {
				  if(activeClients.find(str)==activeClients.end())
					 	returnString="FAILURE:CLIENT_OFFLINE\n";
				  else
				  {
					 tempClientDetails=activeClients.at(str);
					 returnString+=tempClientDetails.clientIP+":"+to_string(tempClientDetails.clientPort)+":"+to_string(tempClientDetails.clientDownloadPort)+"||Success:"+relativePath; 
				  }
			  } 
		 }
		 else
		  returnString="FAILURE:FILE_NOT_FOUND\n";
	}
	return returnString;
}

void heart(vector <string> tokenVectors)
{
	mtx.lock();
	string alias=tokenVectors[1].substr(0,tokenVectors[1].find_first_of(":"));
	string token;
	//cout<<"Alias:"<<alias;
	if(activeClients.find(alias)==activeClients.end())
	{
		vector <string> tokenVector;
		stringstream temp(tokenVectors[1]);
		while(getline(temp,token,':'))
		{
			tokenVector.push_back(token);
		}
		tempClientDetails.clientIP=tokenVector[1];
		tempClientDetails.clientPort=stoi(tokenVector[2]);
		tempClientDetails.clientDownloadPort=stoi(tokenVector[3]);
		time(&tempClientDetails.timer);
		activeClients.insert(pair<string,clientDetails>(tokenVector[0],tempClientDetails));
		tokenVector.clear();		
	}
	else
	{
		tempClientDetails =activeClients.at(alias);
		time(&tempClientDetails.timer);
	    activeClients[alias]=tempClientDetails;
	}
	mtx.unlock();
}

string operations(vector <string> tokenVectors)
{
	string returnValue="";
	
	if(tokenVectors[0].compare("search")==0)
	{
		if(tokenVectors.size()!=2)
			returnValue="FAILURE:INVALID_ARGUMENTS\n";
		else 
			returnValue=search(tokenVectors[1]);
	}
	else if(tokenVectors[0].compare("get")==0)
	{
		//returnValue="Get Something....\n";
		if(tokenVectors.size()==4)
			returnValue=get(tokenVectors);
		else
		{
			returnValue="FAILURE:INVALID_ARGUMENTS\n";
		}
	}
	else if(tokenVectors[0].compare("share")==0)
	{
		//returnValue="Share Something....\n";
		if(tokenVectors.size()!=3)
			returnValue="FAILURE:INVALID_ARGUMENTS\n";
		else
		{
			returnValue=share(tokenVectors);
		}
	}
	else if(tokenVectors[0].compare("del")==0)
	{
		//returnValue="Deleting Something....\n";
		if(tokenVectors.size()!=3)
			returnValue="FAILURE:INVALID_ARGUMENTS\n";
		else
		{
			returnValue=deleting(tokenVectors);
		}
	}
	else if(tokenVectors[0].compare("exec")==0)
	{
		//returnValue="Will Execute Something....\n";
		if(tokenVectors.size()!=3)
			returnValue="FAILURE:INVALID_ARGUMENTS\n";
		else
		{
			returnValue=execute(tokenVectors);
		}
	}
	else if(tokenVectors[0].compare("heart")==0)
	{
		cout<<"\nThe heartbeat:::"<<tokenVectors[1]<<endl;
		   heart(tokenVectors);
		return "heart";
	}
	else
	 returnValue="A Wrong Command has been entered... Please do check it\n";
	
	return returnValue;
}

void *threadInitiatorFunction(void *arguments)
{
	int client_socketFileDescriptor=*((int *) arguments);
	free(arguments);
    
    char *returnValue = (char *)malloc(sizeof(*returnValue));
    string command="",restOfString="",inputString="";
	vector <string> tokenVectors;
	int flag=0,i=0,j=0;
	char ch;//int i=0,j=0;
	returnValue=(char *)"continue";
	while(1)
	{
		if(read(client_socketFileDescriptor,&ch,1)>=1)
		{
			//write(client_socketFileDescriptor,&ch,1);			
			if(ch==27)
			{
			  close(client_socketFileDescriptor);
			  cout<<"Exiting Gracefully!!!!\n";	
			  returnValue=(char *)"exit";
			  break;
			}
			else if(ch=='\n')
			{
				tokenVectors.push_back(inputString);
				cout<<"\nInput from Client:::";
				for(string temp:tokenVectors)
					cout<<temp<<" : ";
				cout<<"\n";
				
				string temporary=operations(tokenVectors);
				if(temporary.compare("heart")!=0)
				{
					char *returnedValue= new char[temporary.length() + 1];
					strcpy(returnedValue, temporary.c_str());
					returnedValue[strlen(returnedValue)]='\0';
					
					cout<<returnedValue;
					write(client_socketFileDescriptor,returnedValue,strlen(returnedValue)+1);
				}
				tokenVectors.clear();
				inputString="";
				close(client_socketFileDescriptor);
				//FD_CLR(client_socketFileDescriptor, &readfds);
				break;
			}
			else
			{
				inputString+=ch;
				if(inputString.length()>3)
				{
					if(inputString.compare(inputString.size()-3,3,"!&!") == 0)
					{
						inputString=inputString.substr(0,inputString.size()-3);
						tokenVectors.push_back(inputString);
						inputString="";
					}
					
				}
			}
		}
	}
    
    
    //returnValue="exit";
    //cout<<"\nReturnValue:"<<returnValue<<" Some Value"<<endl;
    pthread_exit(returnValue);
}

void *timer_function(void *arguments)
{
  while(1)
  {	
	cout<<"\nTimer Function called\n";
	sleep(600);
	mtx.lock();
	time_t now;
	double seconds;
	for(auto iter = activeClients.begin(); iter != activeClients.end(); ++iter )
	{
		string str=iter->first;
		clientDetails temp=iter->second;
		/*str+=":"+temp.clientIP+":"+to_string(temp.clientPort)+":"+to_string(temp.clientDownloadPort)+"\n";
		out << str;*/
		time(&now);
		seconds=difftime(now,temp.timer);
		cout<<str<<" ----->> "<<temp.timer<<" : "<<now<<" : "<<seconds<<endl;
		if(seconds>720)
		  activeClients.erase(str);		
	}
	
	fileSave();
	mtx.unlock();
  }
}

int main(int argc, char *argv[])
{
	char ch;int i=0,j=0;
	//fd_set readfds;
	int server_socketFileDescriptor,client_socketFileDescriptor,threadCreationResult;
	unsigned int serverLength,clientLength;
	struct sockaddr_in serverAddress;
	struct sockaddr_in clientAddress;
	
	pthread_t client_communication_threads,timer_function_thread;
	void *thread_result;
	
	if(argc<6)
	{
		cout<<"One of the argument is missing";
	}
	else
	{
		listFile=(char *) malloc(strlen(argv[5])+strlen(argv[4]));
		repoFile=(char *) malloc(strlen(argv[5])+strlen(argv[3]));
		
		//Storing the filename as per path
		for(j=0;j<strlen(argv[5]);j++)
		{
		 listFile[j]=argv[5][j];
		 repoFile[j]=argv[5][j];
		}
		listFile[j]=repoFile[j]='/';
				
		i=strlen(argv[5])+1;
		//appending the listfile with argv[4]
		for(j=0;j<strlen(argv[4]);j++)
		 listFile[i++]=argv[4][j];
		
		i=strlen(argv[5])+1;
		//appending the repofile with argv[4]
		for(j=0;j<strlen(argv[3]);j++)
		  repoFile[i++]=argv[3][j];
		
		//Read both the files
		readFiles(repoFile,listFile);
		
		atexit(fileSave);
		
		//creation of the socket on the server side
		server_socketFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
		
		//Name the socket
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr=inet_addr(argv[1]);//htonl(argv[1]); //INADDR_ANY will allow the server to accept the connections from any interface in the computer, later we need to identify the local interface and WAN interface
		//string port=argv[2];
		serverAddress.sin_port=htons(stoi((string)argv[2]));//61100
		serverLength=sizeof(serverAddress);
		
		//After the naming the socket with the various details now bind it
		bind(server_socketFileDescriptor,(struct sockaddr *)&serverAddress,serverLength);
		
		/*A connection queue is created to listen to the clients and wait for them, 3 refers to the maximum number of connections that
		 it will keep as a backlog, post it the connections from the client will be refused*/
		listen(server_socketFileDescriptor,3);
		
		cout<<"Server started .....\n";

		
		pthread_create(&timer_function_thread, NULL, timer_function,NULL);
		
		while(1)
		{
			
			clientLength=sizeof(clientAddress);
			//Now we have the client File Descriptor so we can read and write to it
			cout<<"\nWaiting for Connections....\n";
			client_socketFileDescriptor=accept(server_socketFileDescriptor,(struct sockaddr *)&clientAddress,&clientLength);
			cout<<"Connection Made with....:: "<<clientAddress.sin_addr.s_addr<<":"<<ntohs(clientAddress.sin_port)<<"\n";
			
			int *argument = (int *)malloc(sizeof(*argument));
			*argument=client_socketFileDescriptor;
			threadCreationResult=pthread_create(&client_communication_threads, NULL, threadInitiatorFunction, (void *)argument);
			if(threadCreationResult!=0)
			{
				perror("\nThread Creation Issue:\n");
				close(server_socketFileDescriptor);	
			}
			 
			threadCreationResult = pthread_join(client_communication_threads, &thread_result); //The second argument is a pointer to a pointer that itself points to the return value from the thread 
					
			
			
			string returnedValue((char *)thread_result);
			//cout<<"Thread Returning Result:"<<returnedValue<<endl;
			if(returnedValue.compare("exit")==0)
			 break;
		}
        close(server_socketFileDescriptor);	
        pthread_join(timer_function_thread, &thread_result);
		
	}
	
	
	
}
