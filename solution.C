#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include "node.h"
#include "file.h"

using namespace std;

template<class T>
bool descending( T const &a, T const &b){
	return b < a;
}

bool sortByID( DataFile &a, DataFile &b){
	return a.getID() < b.getID();
}

static void show_helpmess()
{
	std::cerr << "Options:\n"
		<< "\t-h,--help\tShow this help message\n"
		<< "\t-f,\tSpecify the input file list\t(requied)\n"
		<< "\t-n,\tSpecify the input node list\t(requied)\n"
		<< "\t-o,\tSpecify the output file name\t(optional)\n"
		<< std::endl;
}

int main(int argc, char* argv[])
{
	if(argc < 5) {
		show_helpmess();
		return 1;
	}

	string inputfilename("");
	string inputnodename("");
	string outputname(""); 
	for(int i = 1; i < argc; ++i){
		string arg = argv[i];
		if((arg == "-h")||(arg == "--help")){
			show_helpmess();
			return 0;
		}
		else if((arg == "-f")) {
			if (i + 1 < argc){
				inputfilename = argv[i+1];
				i += 1;
			}
			else{
				std::cerr << "The input file list is required\n";
				return 1;
			}
		}
		else if((arg == "-n")) {
			if (i + 1 < argc){
				inputnodename = argv[i+1];
				i += 1;
			}
			else{
				std::cerr << "The input node list is required\n";
				return 1;
			}
		}
		else if((arg == "-o")) {
			if (i + 1 < argc){
				outputname = argv[i+1];
				i += 1;
			}
			else{
				std::cerr << "The output file name need to be specified\n";
				return 1;
			}
		}	
	}
	
	if( inputfilename == "" ){ 
		std::cerr << "The input file list is required\n";
		return 1;
	}
	if( inputnodename == "" ){
		std::cerr << "The input node list is required\n";
		return 1;
	}

	vector< Node > nodeList;
	vector< DataFile > fileList;
	nodeList.clear();
	fileList.clear();

	string  inputline;
	stringstream inputstream;
	string  linename;
	int     linesize;
	regex   commentline("[\\s]*[#][\\s\\S]*");
	regex   dataline("[\\s]*[\\S]+[\\s|\\t]+[0-9]+[\\s]*");
	
	long    totalspace(0);
		
	ifstream node_file(inputnodename);
	if( node_file.is_open()){
		while( getline( node_file, inputline)){
			if( regex_match( inputline, commentline) || inputline.empty())
				continue;
			else if( regex_match(inputline, dataline) ){
				inputstream.str("");
				inputstream.clear();
				linename = "";
				linesize = 0;
				
				inputstream << inputline;
				inputstream >> linename >> linesize;
				Node singlenode(linename, linesize);
				nodeList.push_back(singlenode);
				totalspace += linesize;
			}
			else continue;
		}
	}
	else{
		std::cerr << "Cannot open the node file: " << inputnodename << ". Please check if the file exists." << std::endl;
		return 1;
	}	 

	ifstream data_file(inputfilename);
	if( data_file.is_open()){
		int fileid(0);
		while( getline( data_file, inputline)){
			if( regex_match( inputline, commentline) || inputline.empty())
				continue;
			else if( regex_match(inputline, dataline) ){
				inputstream.str("");
				inputstream.clear();
				linename = "";
				linesize = 0;
				
				inputstream << inputline;
				inputstream >> linename >> linesize;
				DataFile singlefile(linename, linesize, fileid);
				fileList.push_back(singlefile);
				fileid += 1;
			}
			else continue;
		}
	} 
	else{
		std::cerr << "Cannot open the node file: " << inputfilename << ". Please check if the file exists." << std::endl;
		return 1;
	}	 

	if( nodeList.size() == 0){
		std::cerr << "No available node. Please check the node list" << std::endl;
		return 1;
	}
	
	sort( nodeList.begin(), nodeList.end(), descending<Node> );  // sort the nodes and files in descending order
	sort( fileList.begin(), fileList.end(), descending<DataFile> );

	for( vector< DataFile >::iterator itf = fileList.begin(); itf != fileList.end(); itf++){

		if( itf->getSize() > nodeList[0].getSize() ) continue;
		long minload = totalspace;
		int  nodeid(-1); 
		for( vector< Node >::iterator itnd = nodeList.begin(); itnd != nodeList.end(); itnd++){
			if( itf->getSize() <= itnd->getLeftSpace() && itnd->getLoad() < minload){
				minload = itnd->getLoad();  // fill the file to the least-occupied node
				nodeid = distance( nodeList.begin(), itnd);
			}
		}
		
		if( nodeid >= 0)
			itf->assignFileToNode(&nodeList[nodeid], nodeid);
		else{
		//  If no node can be assigned to the file, try to arrange the files in different nodes.
		//  Combine A and B, and switch with C.
			bool findbin(false);
			for(vector< DataFile >::iterator itcomb = fileList.begin(); itcomb < itf; itcomb++){
				if( findbin )break;
				if( itcomb->getNodeId() < 0 || itcomb->getNodeId() >= nodeList.size()) continue;

				for( vector< DataFile >::iterator itsw = fileList.begin(); itsw < itf; itsw++){
					int combid = itcomb->getNodeId();
					int switchid = itsw->getNodeId();
					if( switchid < 0 || switchid == combid || switchid > nodeList.size()) continue;
					int combsize = itcomb->getSize() + itf->getSize();
					int swsize = itsw->getSize();
					int targetspace = nodeList[switchid].getLeftSpace() + swsize;
					int switchspace = nodeList[combid].getLeftSpace() + itcomb->getSize();
					
					if( targetspace >= combsize && switchspace >= swsize ){
						itcomb->removeFileFromNode( &nodeList[combid] );
						itsw->removeFileFromNode( &nodeList[switchid] );
				
						itf->assignFileToNode( &nodeList[switchid], switchid );
						itcomb->assignFileToNode( &nodeList[switchid], switchid );
						itsw->assignFileToNode( &nodeList[combid], combid);
						findbin = true;
						break;
					}
				}
			}					
		}

	}

	sort( fileList.begin(), fileList.end(), sortByID);

	if(outputname != ""){
		ofstream outfile;
		outfile.open( outputname );
		
		for(unsigned i(0); i < fileList.size(); i++)
			outfile << fileList[i].getName() <<  " " << fileList[i].getNodeName() << std::endl;
		
		outfile.close();
	}
	else{
		for(unsigned i(0); i < fileList.size(); i++)
			cout << fileList[i].getName() <<  " " << fileList[i].getNodeName() << std::endl;
	}
		
	
	return 0;
}	
