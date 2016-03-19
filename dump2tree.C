#include <stdio.h>
#include <string.h>
#include <TTree.h>

const int MAX_N = 500;
typedef struct event_t
{
	int type = -1; // -1 empty 0 thsrc 1 tra
	int train_id;
	// int trainclass; //1ths
	int order;
	int station_id[MAX_N];
	int array_time[MAX_N];
	int departure_time[MAX_N];
	
}event_t;
TTree tree_muon("tree_muon", "apple tree");
event_t timetable;

int Norder;

// split string
void tsplit(int output[], char str[]) {
	char * pch;
	pch = strtok (str,":");
	int i = 0;
	while (pch != NULL){
		pch = strtok (NULL, ":");
		string temp(pch);
		output[i] = atoi(temp.c_str()) ; // convert to int
		i++;
	}
}

void savefromtra(TXMLEngine* xml, XMLNodePointer_t node){
	// cout << "gogogo" << endl;
	// cout << strcmp(xml->GetNodeName(node), "TrainInfo") << " " << xml->GetNodeName(node) << endl;
	if (strcmp(xml->GetNodeName(node), "TrainInfo")==0)
	{
		if (timetable.type!= -1){
			tree_muon.Fill();
			timetable = event_t();
		}
		Norder = 0;
		timetable.train_id = 100000 + xml->GetIntAttr(node, "Train value");
		timetable.type = 1; //tra
	}
	if (strcmp(xml->GetNodeName(node), "TimeInfo")==0){
		int temp[3];
		
		const char * arrival_time = xml->GetAttr(node, "ARRTime");
		tsplit(temp, arrival_time);
		
		cout << temp[0] << endl;
		const char * departure_time = xml->GetAttr(node, "DEPTime");
		int station_id = xml->GetIntAttr(node, "DEPTime");

	}


	// for searching all data
	XMLNodePointer_t child = xml->GetChild(node);
	while (child!=0) {
		savefromtra(xml, child);
		child = xml->GetNext(child);
		// cout << "hahaha" << endl;
	}
	//last one cannot save by the loop
	tree_muon.Fill();
	
}
void savefromthsrc(TXMLEngine* xml, XMLNodePointer_t node){

}
void xmlreadfile(const char* filename = "example.xml", int type = 0){
	TXMLEngine* xml = new TXMLEngine;
	// Now try to parse xml file
	// Only file with restricted xml syntax are supported
	XMLDocPointer_t xmldoc = xml->ParseFile(filename);
	if (xmldoc==0) {
		delete xml;
		return;
	}
	// take access to main node
	XMLNodePointer_t mainnode = xml->DocGetRootElement(xmldoc);
	// save data 2 tree
	if (type == 1)	{
		savefromtra(xml, mainnode);
	}else{
		savefromthsrc(xml, mainnode);
	}
	// Release memory
	xml->FreeDoc(xmldoc);
	delete xml;
}
void dump2tree(){
	TFile *f1 = new TFile("muon.root", "recreate");
	// construct a tree
	tree_muon.Branch("type", &timetable.type, "type/I");
	tree_muon.Branch("train_id", &timetable.train_id, "train_id/I");
	tree_muon.Branch("order", &timetable.order, "order/I");
	tree_muon.Branch("station_id", timetable.station_id, "station_id[order]/I");
	tree_muon.Branch("array_time", timetable.array_time, "array_time[order]/I");
	tree_muon.Branch("departure_time", timetable.departure_time, "departure_time[order]/I");
	//readfile
	xmlreadfile("20160116.xml", 1);

	f1 ->cd();
	tree_muon.Write();
	f1 ->Close();
}