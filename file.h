#ifndef DATAFILE
#define DATAFILE

#include <iostream>

#ifndef NODE
#include "node.h"
#endif

using namespace std;

class DataFile{
	public:
		DataFile( string name = "", int totalsize = 0, int id = -1):
			filename_(name),filesize_(totalsize), fileid_(id)
			{
				nodename_ = "NULL";
				nodeid_ = -1;
			}
		~DataFile(){
		}
		
		inline int getSize() const{return filesize_;}
		inline int getID() const{return fileid_;}
		inline string getName(){return filename_;}
		inline string getNodeName(){return nodename_;}
		inline int getNodeId(){return nodeid_;}
		inline void setNode(string name){ nodename_ = name; }
		bool   assignFileToNode(Node *node, int nodeid);
		bool   removeFileFromNode(Node *node);

		bool operator<(const DataFile& obj) const {
			return this->getSize() < obj.getSize();
		}
	private:
		string filename_;
		int    filesize_;
		int    fileid_;
		string nodename_;
		int    nodeid_;
};

bool DataFile::assignFileToNode(Node *node, int nodeid){
	if(node == NULL){
		std::cout << "node is NULL\n";
		return false;
	}
	else{
		node->addFile(filename_, filesize_);
		nodename_ = node->getName();
		nodeid_ = nodeid;
		return true;
	}
}


bool DataFile::removeFileFromNode(Node *node){
	if(node == NULL){
		std::cout << "node is NULL\n";
		return false;
	}
	else{
		node->removeFile(filename_, filesize_);
		nodename_ = "NULL"; 
		nodeid_ = -1;
		return true;
	}
}

#endif
