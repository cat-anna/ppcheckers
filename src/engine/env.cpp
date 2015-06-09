#include "env.h"

using namespace std;

cApplication *Application = 0;

int main(int argc, char** argv) {
	try {
		Application = new cApplication(argc, argv);	
		int res = 1;
		try {
			res = Application->Run();
		}
		catch(...){
		}		
		delete Application;
		return res;
	}
	catch(bool){
		cerr << "boolean value thrown!\n";
	}
	catch (const char * Msg){
		cerr << "FATAL ERROR! '" << Msg << "'\n";
	}
	catch (string & Msg){
		cerr << "FATAL ERROR! '" << Msg << "'\n";
	}
	catch (std::exception &E){
		cerr << "FATAL ERROR! '" << E.what() << "'\n";
	}
	catch (...){
		cerr << "UNKNOWN FATAL ERROR!";
	}
	delete Application;
	return 1;
}
