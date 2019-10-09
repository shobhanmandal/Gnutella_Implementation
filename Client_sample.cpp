//'etc/services' contains all the ports that have been used by the current system
// hostname -I : unix system to know the ip address
#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <bits/stdc++.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;
int serverProcessID, downloadServerProcessID, heartBeatID;
void killProcesses()
{
	cout<<"\nThe Client server is killed:"<<serverProcessID<<"\n";
	cout<<"\nThe Client Download server is killed:"<<downloadServerProcessID<<"\n";
	cout<<"\nThe HeartBeat process is stopped:"<<heartBeatID<<"\n";
	kill(downloadServerProcessID, SIGKILL);
	kill(serverProcessID, SIGKILL);
	kill(heartBeatID, SIGKILL);
	
}
void connectingToClientServer(string inputCommand,string returnedResult,char *argv[])
{
	
	int clientmode_socketFileDescriptor2, clientServerResult;;
	unsigned int client_modeLength2;
	struct sockaddr_in clientmode_Address2;
	 
	 if(inputCommand.compare("get")==0)
	 {
		 char ch;
		 int i=0;
		 string token,command;
		 vector <string> tokenVector;
		 stringstream temp1(returnedResult);
		 
		 while(getline(temp1,token,'|'))
		  {
			  if(i==0)
			  {
				  stringstream temp2(token);
				  while(getline(temp2,token,':'))
					tokenVector.push_back(token);
				  i=1;
			  }
			  else
				returnedResult=token;
		  }
		  //cout<<"Text Returned from Server for Get ::::"<<returnedResult<<"\n";
		  //cout<<returnedResult<<"\n";
		  
		  char *text= new char[tokenVector[0].length() + 1];
		  strcpy(text, tokenVector[0].c_str());
		  //cout<<returnedValue;
		  
		  clientmode_Address2.sin_family=AF_INET;
		  clientmode_Address2.sin_addr.s_addr=inet_addr(text);
		  clientmode_Address2.sin_port = htons(stoi(tokenVector[1]));
		  client_modeLength2=sizeof(clientmode_Address2);
		  
		  clientmode_socketFileDescriptor2= socket(AF_INET, SOCK_STREAM, 0);
		  clientServerResult=connect(clientmode_socketFileDescriptor2,(struct sockaddr *)&clientmode_Address2, client_modeLength2);
		  if(clientServerResult==-1)
				//perror("\nSorry Client will not able to connect to the Downloading *Peer*\n");
				cout<<"\nFAILURE:CLIENT_OFFLINE\n";
		  else
		  {
			  command="";
			  int bytes=write(clientmode_socketFileDescriptor2,"get",3);
			  
			  while(1)
			  {
				  if(read(clientmode_socketFileDescriptor2,&ch,1)>=1)
				   command+=ch;
				  if(command.compare("initiate")==0)
				   break;
			  }
			  //cout<<"\nInitiate Command:"<<command<<":"<<bytes<<endl;
			  close(clientmode_socketFileDescriptor2);
			  //sleep(2);//This is to allow the download server to configure itself before the client sends the request to it
			 
			  if(command.compare("initiate")==0)
			  {
				  //Update the connection with the download server port
				  clientmode_Address2.sin_port = htons(stoi(tokenVector[2]));	
				  clientmode_socketFileDescriptor2= socket(AF_INET, SOCK_STREAM, 0);
				  clientServerResult=connect(clientmode_socketFileDescriptor2,(struct sockaddr *)&clientmode_Address2, client_modeLength2);
				  
				  
				  //cout<<"\nDownload will commence shortly........\n";
				  
				  if(clientServerResult==-1)
					//perror("\nSorry Client is not able to connect to the Downloading *Peer Server*\n");
					cout<<"\nFAILURE:CLIENT_OFFLINE\n";
				  else
				  {
					  returnedResult=returnedResult.substr(returnedResult.find_first_of(":")+1);
					  char *text=(char *) malloc(returnedResult.length() + 1); //new char[returnedResult.length() + 1];
					  strcpy(text, returnedResult.c_str());
					  text[strlen(text)]='\n';
					  write(clientmode_socketFileDescriptor2,text,strlen(text)+1);
					  
					  
					  returnedResult=returnedResult.substr(returnedResult.find_last_of("/")+1);
					  text= (char *) malloc(returnedResult.length() + 1);
					  strcpy(text, returnedResult.c_str());
					  
					  char fileCreation[strlen(argv[7])+strlen(text)];
					  strcpy (fileCreation,argv[7]);
					  strcat (fileCreation,"/");
					  strcat (fileCreation,text);
					  
					  //cout<<"\nYour File is being downloaded to:"<<fileCreation<<"\n";
					  
					  int newFileCreation=open(fileCreation,O_RDWR|O_CREAT);
					  char buf[10240];
					  long nbytes;
					  nbytes = sizeof(buf);
					  long fileSize;
					  read(clientmode_socketFileDescriptor2,&fileSize,sizeof(long));
					  //cout<<"\nSize of File to be Received::"<<fileSize<<endl;
					  
					  if(fileSize!=-100)
					  {
						  do
						  {
							  size_t num = min(fileSize, nbytes);
							  num =read(clientmode_socketFileDescriptor2,buf,num);
							  write(newFileCreation,buf,num);
							  fileSize-=num;
							  //cout<<fileSize<<"\n";
							  //if(fileSize<1024000) sleep(120);
						  }while(fileSize>0);
						  cout<<"\nSUCCESS:"<<argv[7]<<"\n";
						  /*while(read(clientmode_socketFileDescriptor2,buf,nbytes))
							 write(newFileCreation,buf,nbytes);*/
					  }
					  else
					  {
						  cout<<"\nFAILURE:FILE_NOT_FOUND\n";
					  }
					  	
				  }
				  close(clientmode_socketFileDescriptor2);
			  }
			  else
			   //cout<<"\nIssue in Downloading Process\n";
			   cout<<"\nFAILURE:CLIENT_OFFLINE\n";
		  } 
	 }
	 else if(inputCommand.compare("exec")==0)
	 {
		 vector <string> tokenVector;
		 string token;
		 char ch;
		 
		 stringstream temp1(returnedResult);
		 
		 while(getline(temp1,token,':'))
			tokenVector.push_back(token);
			
		  char *text= new char[tokenVector[0].length() + 1];
		  strcpy(text, tokenVector[0].c_str());
		  //cout<<returnedValue;
		  
		  clientmode_Address2.sin_family=AF_INET;
		  clientmode_Address2.sin_addr.s_addr=inet_addr(text);
		  clientmode_Address2.sin_port = htons(stoi(tokenVector[1]));
		  client_modeLength2=sizeof(clientmode_Address2);
		  
		  clientmode_socketFileDescriptor2= socket(AF_INET, SOCK_STREAM, 0);
		  clientServerResult=connect(clientmode_socketFileDescriptor2,(struct sockaddr *)&clientmode_Address2, client_modeLength2);
		  if(clientServerResult==-1)
				//perror("\nSorry Client will not able to connect to the Downloading *Peer*\n");
				cout<<"\nFAILURE:CLIENT_OFFLINE\n";
		  else
		  {
			  string returnedResult="";
			  text= new char[tokenVector[2].length() + 1];
			  strcpy(text, tokenVector[2].c_str());
			  write(clientmode_socketFileDescriptor2,text,strlen(text));
			  cout<<"\nSUCCESS:\n";
			  while(1)
			  {
				  if(read(clientmode_socketFileDescriptor2,&ch,1)>=1)
				  {
					  if(ch!='`')
					   returnedResult+=ch;
					  else
					   break; 
				  }
			  }
			  cout<<returnedResult<<endl;
			  
		  }
		  close(clientmode_socketFileDescriptor2);	
	 }
	 		
}

void actingClient(char *argv[])
{
	int result,spaceCount=0;
	int servermode_socketFileDescriptor,clientmode_socketFileDescriptor1,clientmode_socketFileDescriptor2,tempClientFD;
	unsigned int server_modeLength,client_modeLength1,client_modeLength2;
	struct sockaddr_in servermode_Address,clientmode_Address1,clientmode_Address2;
	
	//Naming of the socket as agreed with the server
	clientmode_Address1.sin_family=AF_INET;
	clientmode_Address1.sin_addr.s_addr=inet_addr(argv[4]);//hostname -I from server returned 10.4.21.70
	clientmode_Address1.sin_port = htons(stoi((string)argv[5]));
	client_modeLength1=sizeof(clientmode_Address1);
	
	//try
	//{
		{
		char readingInput[1];
		char *text=(char *) malloc(200);
		int i=0,quoteCount=0,j=0,k=0;
		cout<<"\nEnter Command:\n";
		result=-1;
		string returnedResult="";
		char ch[1];
		map <string,string> previousSearchResult;
		
		//FD_ZERO(&readfds);
		while(1)
		{
			if(read(0, &readingInput, 1) >= 1)
			{
				if(readingInput[0]==27||readingInput[0]=='`')
				{
					if(readingInput[0]=='`')
					 break;
					clientmode_socketFileDescriptor1= socket(AF_INET, SOCK_STREAM, 0);
					result=connect(clientmode_socketFileDescriptor1,(struct sockaddr *)&clientmode_Address1, client_modeLength1);
					if(result==-1)
						  perror("Sorry Client will not able to connect");
					else
					{
						write(clientmode_socketFileDescriptor1,&readingInput[0],1);
						close(clientmode_socketFileDescriptor1);	
					}
					cout<<"\nClient is exiting Gracefully\n";
					break;
				}
				else if(readingInput[0]=='\n')
				{
					string command(text,strlen(text));
					command=command.substr(0,command.find("!&!"));
					//cout<<"\n---------------"<<command<<"-------------------\n";
					
					if(command.compare("share")==0)
					{
						if(spaceCount!=1)
						{
							cout<<"FAILURE:INVALID_ARGUMENTS\n"<<"\nEnter Command:\n";
							i=0;
							spaceCount=0;
							text=(char *) malloc(200);
							result=-1;
							continue;
						}
						char *path=(char *)malloc(100);
						for(j=0;j<strlen(argv[7]);j++)
							path[j]=argv[7][j];
						path[j++]='/';
						
						for(k=8;k<strlen(text);k++)
						{
							if(text[k]=='!'&&text[k+1]=='&'&&text[k+2]=='!')
							 break;
							path[j++]=text[k];
						}
						//cout<<"\nPath="<<path<<"\n";
						if(access(path,F_OK)!=-1)
						//if(1)
						{
							text[i++]='!';text[i++]='&';text[i++]='!';
							for(int j=0;j<strlen(argv[1]);j++)
								text[i++]=argv[1][j];
						}
						else
						{
							cout<<"FAILURE:FILE_NOT_FOUND\n";
							i=0;
							spaceCount=0;
							text=(char *) malloc(200);
							result=-1;
							cout<<"\nEnter Command:\n";
							continue;
						}
						
					}
					else if(command.compare("del")==0)
					{
						text[i++]='!';text[i++]='&';text[i++]='!';
						for(int j=0;j<strlen(argv[1]);j++)
						  text[i++]=argv[1][j];
					}
					else if(command.compare("get")==0)
					{
						if(spaceCount!=3&&spaceCount!=2)
						{
							cout<<"FAILURE:INVALID_ARGUMENTS\n"<<"\nEnter Command:\n";
							i=0;
							spaceCount=0;
							text=(char *) malloc(200);
							result=-1;
							continue;
						}
						else if(spaceCount==3)
						{
							char *path=(char *)malloc(100);
							for(j=0;j<strlen(argv[7]);j++)
								path[j]=argv[7][j];
							path[j++]='/';
							
							for(k=strlen(text)-1;k>6;k--)
							{
								if(text[k]=='!'&&text[k-1]=='&'&&text[k-2]=='!')
								 break;
							}
							k+=1;
							for(;k<strlen(text);k++)
								path[j++]=text[k];
							//cout<<"\nPath::"<<path<<endl;
							cout<<endl;
							if(access(path,F_OK)!=-1)
							{
								cout<<"FAILURE:ALREADY_EXISTS\n";
								i=0;
								spaceCount=0;
								text=(char *) malloc(200);
								result=-1;
								cout<<"Enter Command:\n";
								continue;
						    }
						}
						else if(spaceCount==2)
						{
							if(previousSearchResult.size()==0)
							{
								cout<<"FAILURE:NO_SEARCH_EARLIER\n";
								i=0;
								spaceCount=0;
								text=(char *) malloc(200);
								result=-1;
								cout<<"Enter Command:\n";
								continue;
							}
							else
							{
								string tempString="";
								for(j=0;j<strlen(text);j++)
								{
									if(text[j]=='!'&&text[j+1]=='&'&&text[j+2]=='!'&&j>7)
									 break;
									tempString+=text[j]; 
								}
								//cout<<"TempString:"<<tempString<<endl;
								string str=previousSearchResult.at(tempString);
								text=(char *) malloc(200);
								strcpy(text, str.c_str());
								i=str.length();
							}
							
						}
						
					}
					
					if(result==-1)
					{
						clientmode_socketFileDescriptor1= socket(AF_INET, SOCK_STREAM, 0);//A new call to socket is to be made since the close function is being called making the FD a bad descriptor
						result=connect(clientmode_socketFileDescriptor1,(struct sockaddr *)&clientmode_Address1, client_modeLength1);
						
						if(result==-1)
						{
						  perror("SERVER_OFFLINE");
						  i=0;
						  spaceCount=0;
						  text=(char *) malloc(200);
						  result=-1;
						  cout<<"Enter Command:\n";	
					    }
						else
						{
							text[i++]='\n';
							//cout<<"Wired Text:"<<text<<"\n";
						
						    //cout<<"A\n";
						/********Sending to Server***********/
							i=0;	
							while(i<strlen(text))
								write(clientmode_socketFileDescriptor1,&text[i++],1);
							
  						/********Receiving from Server***********/	
							returnedResult="";
							i=0;
							
							while(read(clientmode_socketFileDescriptor1,&ch,1)==1)
							   returnedResult+=ch;

						    close(clientmode_socketFileDescriptor1);
						    
						    //cout<<"C\n";
						    i=0;
						    if(command.compare("get")==0&&returnedResult.compare(0,7,"FAILURE")!=0)
						        connectingToClientServer("get",returnedResult,argv);
							else if(command.compare("exec")==0&&returnedResult.compare(0,7,"FAILURE")!=0)
							    connectingToClientServer("exec",returnedResult,argv);
							else if(command.compare("search")==0&&returnedResult.compare(0,7,"FAILURE")!=0)
							{
								string line,token,subString[4],keyString;
								stringstream temp1(returnedResult);
								cout<<returnedResult<<"\n";
								while(getline(temp1,line,'\n'))
								{
									    if(line.compare(0,14,"SUCCESS:Found:")==0||line.length()==0)
									     continue;
									    j=0;
									    //cout<<line<<endl;
									    stringstream temp1(line); 
										while(getline(temp1,token,':'))
										{
											
											if(j==0)
											{
												subString[0]="get!&!"+token.substr(0,token.find_first_of(" "));
												subString[3]=token.substr(token.find_first_of(" ")+1,token.find_first_of(":"));//-token.find_first_of(" "));
											}
											if(j==1)
											    subString[2]="!&!"+token+"!&!";
											if(j==2)
											    subString[1]="get!&!"+token; 
											j+=1;    		 
										}
									    keyString=subString[1]+subString[2]+subString[3];
    									previousSearchResult.insert(pair<string,string>(subString[0],keyString));
								}
								
							}
							else
								//cout<<"Text Returned from Server::::"<<returnedResult<<"\n";
							    cout<<returnedResult<<"\n";
							
							//cout<<"D\n";
							cout<<"\nEnter Command:\n";	
							i=0;
							spaceCount=0;
							text=(char *) malloc(200);
							result=-1;	
							
						}  
					}					
				} 
				else
				{
					if(readingInput[0]==' '&&quoteCount==0)
					{	
						text[i++]='!';
						text[i++]='&';
						text[i++]='!';//space between characters
						spaceCount++;
					}
					else if(readingInput[0]=='"' && quoteCount==0)
					   quoteCount=1;
					else if(readingInput[0]=='"' && quoteCount==1)
					   quoteCount=0;
					else      
						text[i++]=readingInput[0];
				}	
		   }
	   }
	 }	

	//}
	//catch(int e)
	//{
	//	cout<<"An Exception occured::"<<e<<endl;
	//}
	
}

void actingHeartBeat(char *argv[])
{
	int result;//=-1;
	int clientmode_socketFileDescriptor1;//,clientmode_socketFileDescriptor2,tempClientFD;
	unsigned int client_modeLength1;
	struct sockaddr_in clientmode_Address1;//,clientmode_Address2;
	
	//Naming of the socket as agreed with the server
	clientmode_Address1.sin_family=AF_INET;
	clientmode_Address1.sin_addr.s_addr=inet_addr(argv[4]);//hostname -I from server returned 10.4.21.70
	clientmode_Address1.sin_port = htons(stoi((string)argv[5]));
	client_modeLength1=sizeof(clientmode_Address1);
	char *text=(char *) malloc(200);
	
	strcat(text,"heart!&!");
	strcat(text,argv[1]);
	strcat(text,":");
	strcat(text,argv[2]);
	strcat(text,":");
	strcat(text,argv[3]);
	strcat(text,":");
	strcat(text,argv[6]);
	strcat(text,"\n");
	
	cout<<"\nYour Heart Beat Side is up:"<<text<<endl;
	
	while(1)
	{
		
			clientmode_socketFileDescriptor1= socket(AF_INET, SOCK_STREAM, 0);//A new call to socket is to be made since the close function is being called making the FD a bad descriptor
			result=connect(clientmode_socketFileDescriptor1,(struct sockaddr *)&clientmode_Address1, client_modeLength1);
			
			if(result==-1)
			 { perror("SERVER_OFFLINE"); break;}
			else
			  write(clientmode_socketFileDescriptor1,text,strlen(text));
			
			close(clientmode_socketFileDescriptor1);
			//result=-1;
		    sleep(300);
	}
}

void downloadingServer(char *argv[], int client_socketFileDescriptor)
{
	
		char *filePath=(char *) malloc(100);
		int i=0;
		char ch;
		cout<<endl;
		
		while(1)
		{
			//cout<<"Loop Entered"<<endl;
			if(read(client_socketFileDescriptor,&ch,1)>=1)
			{
				if(ch=='\n')
				 break;
				else
				{
				 //cout<<"Ch="<<ch<<endl;	
				 filePath[i++]=ch;
			    }
			}
			
		}
		
		//cout<<"\nFilepath:::"<<filePath<<endl;
		char fileSource[strlen(argv[7])+strlen(filePath)];
		strcpy (fileSource,argv[7]);
		strcat (fileSource,"/");
		strcat (fileSource,filePath);
		
		cout<<"\nA Client Peer is downloading this file:"<<fileSource<<"\n";
		if(access(fileSource,F_OK)!=-1)
		{
			
			char buf[10240];
			long nbytes;
			nbytes = sizeof(buf);	
			
		
			int rd=open(fileSource,O_RDONLY);
			ifstream file( fileSource, ios::binary | ios::ate);
			
			long fileSize=file.tellg();
			//cout<<"\nFile Size to be shared:::"<<fileSize<<"\n";
			
			write(client_socketFileDescriptor,&fileSize,sizeof(long));
			
			do
			{
				size_t num = min(fileSize, nbytes);
				num=read(rd,buf,num);
				write(client_socketFileDescriptor,buf,num);
				fileSize-=num;
				//cout<<fileSize<<"\n";
				if(fileSize<10240) sleep(1);
		    }	
			while(fileSize>0);
			
			cout<<"\nFile Sharing Complete\n";
			/*while(read(rd,buf,nbytes))
				write(client_socketFileDescriptor,buf,nbytes);*/
		}
		else
		{
		   long fileSize=-100;
		   write(client_socketFileDescriptor,&fileSize,sizeof(long));	
           cout<<"\nFile is not Present\n";
	    }
         
		
		close(client_socketFileDescriptor);
	/*	break;  	
	}*/
	
	//exit(0);
}

void actingServer(char *argv[])
{
	cout<<"\nClient Side Server is up!\n";
	int servermode_socketFileDescriptor,client_socketFileDescriptor1,downloadservermode_socketFileDescriptor,client_socketFileDescriptor2;
	unsigned int server_modeLength,clientLength1,downloadserver_modeLength,clientLength2;
	struct sockaddr_in servermode_Address,downloadservermode_Address;
	struct sockaddr_in clientAddress1,clientAddress2;
	
	servermode_socketFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
	downloadservermode_socketFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
	
	servermode_Address.sin_family = AF_INET;
	servermode_Address.sin_addr.s_addr=inet_addr(argv[2]);
	servermode_Address.sin_port=htons(stoi((string)argv[3]));
	server_modeLength=sizeof(servermode_Address);
	
	downloadservermode_Address.sin_family = AF_INET;
	downloadservermode_Address.sin_addr.s_addr=inet_addr(argv[2]);
	downloadservermode_Address.sin_port=htons(stoi((string)argv[6]));
	downloadserver_modeLength=sizeof(downloadservermode_Address);
	
	bind(servermode_socketFileDescriptor,(struct sockaddr *)&servermode_Address,server_modeLength);
	listen(servermode_socketFileDescriptor,3);
	
	bind(downloadservermode_socketFileDescriptor,(struct sockaddr *)&downloadservermode_Address,downloadserver_modeLength);
	listen(downloadservermode_socketFileDescriptor,3);
	
	/*clientLength2=sizeof(clientAddress2);
	client_socketFileDescriptor2=accept(downloadservermode_socketFileDescriptor,(struct sockaddr *)&clientAddress2,&clientLength2);
	*/
	string command;
	char ch;
	while(1)
	{
		command="";
		clientLength1=sizeof(clientAddress1);
		client_socketFileDescriptor1=accept(servermode_socketFileDescriptor,(struct sockaddr *)&clientAddress1,&clientLength1);
		while(1)
			{
				if(read(client_socketFileDescriptor1,&ch,1)>=1)
				{
					command+=ch;
					if(command.length()==3)
					{
						if(command.compare("get")==0)
						{
							//cout<<"Get is hit\n";
							downloadServerProcessID=fork();
							atexit(killProcesses);
							if(downloadServerProcessID==0)
							{
							   clientLength2=sizeof(clientAddress2);
	                           client_socketFileDescriptor2=accept(downloadservermode_socketFileDescriptor,(struct sockaddr *)&clientAddress2,&clientLength2);
							   downloadingServer(argv,client_socketFileDescriptor2);
						    }
							else
							   {//The main server part of client will continue listening after closing the current file descriptor
								   cout<<"\nServer Mode:::::Connection in:"<<argv[3]<<" Made with....:: "<<clientAddress1.sin_addr.s_addr<<":"<<ntohs(clientAddress1.sin_port)<<"\n";
								   write(client_socketFileDescriptor1,"initiate",9);//Tell the client to connect to download server to initiate download
								   
								   close(client_socketFileDescriptor1);	
								   break; 
							   }
						}
						else
						{
						   string returnValue="";		
						   string sendCommand(argv[7]);	
						   sendCommand="cd "+ sendCommand  +" && " + command + " 2>&1 ";	
						   
						   //cout<<"SendCommand:"<<sendCommand<<endl;
						   char *text= new char[sendCommand.length() + 1];
						   strcpy(text, sendCommand.c_str());
						   
						   FILE* pipe = popen(text, "r");	
						   if (!pipe) 
						     returnValue= "COMMAND_EXECUTION_FALIURE";
						   else
						   {
							   char bufferArray[256];
								
								while(!feof(pipe)) {
									if(fgets(bufferArray, 256, pipe) != NULL)
										returnValue += bufferArray;
								}
								pclose(pipe);
						   }  
						  
						   text= new char[returnValue.length() + 1];
						   strcpy(text, returnValue.c_str());
						   text[returnValue.length()]='`';
						   write(client_socketFileDescriptor1,text,strlen(text));
						  
						   close(client_socketFileDescriptor1);	
						   break;	
					    }
					}
				}   
			}
	}
}

int main(int argc, char *argv[])
{
	
    if(argc<8)
	{
		cout<<"One of the argument is missing\n";
	}
	else
	{
		
	  serverProcessID=fork();
	  atexit(killProcesses);
	  //actingClient(argv);
	  if(serverProcessID==0)
	    actingServer(argv);
	  else
	  { 
		  //actingClient(argv);
		heartBeatID=fork();
		if(heartBeatID==0)
			actingHeartBeat(argv);
		else
			actingClient(argv);
	  }
	}


	//A Socket for the client side is created

return 0;
}
