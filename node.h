#ifndef NODE
#define NODE

#include <iostream>
using namespace std;

class Node{
	public:
		Node( string name = "", int totalsize = 0):
			nodename_(name),nodesize_(totalsize),nodeleftspace_(totalsize)
			{
				nodefilelist_.clear();
				nodeload_ = 0;
			}
		~Node(){
		}
		
		inline int getSize() const {return nodesize_;}
		inline int getLoad(){return nodeload_;}
		inline int getLeftSpace() {return nodeleftspace_;}
		inline string getName(){return nodename_;}
		inline bool checkFileInNode(string filename){
			std::vector<string>::iterator position = std::find(nodefilelist_.begin(), nodefilelist_.end(), filename);
			if(position != nodefilelist_.end())return true;
			else return false;
		}

		inline void addFile(string filename, int filesize){
			nodefilelist_.push_back(filename);
			nodeload_ += filesize;
			nodeleftspace_ = nodesize_ - nodeload_;
		}

		inline void removeFile(string filename, int filesize){
			if(checkFileInNode(filename)){
				std::vector<string>::iterator position = std::find(nodefilelist_.begin(), nodefilelist_.end(), filename);
				nodefilelist_.erase(position);
				nodeload_ -= filesize;
				nodeleftspace_ = nodesize_ - nodeload_;
			}
			else std::cout << "cannot remove file: file is not assigned to this node" << std::endl;
		}

		bool operator<(const Node& obj) const {
			return this->getSize() < obj.getSize();
		}
		
	private:
		string nodename_;
		int    nodesize_;
		int    nodeload_;
		int    nodeleftspace_;
		vector<string> nodefilelist_;

};

#endif
