//============================================================================
// Name        : MyApp.cpp
// Author      : Adriano
// Version     :
// Copyright   : Your copyright notice
//============================================================================

#include "Main.h"

void loadConfig(char* configFilename) {
	ifstream configFile(configFilename);

	if (!configFile.is_open() || !configFile.good()) {
		configFile.close();
		configFile.clear();

		//File given as parameter does not exist, or no parameter where given
		char defaultFilename[] = "default.conf";
		configFilename = defaultFilename;
		configFile.open(configFilename);

		//configFile.open(configFilename);
		if (!configFile.is_open() && !configFile.good()) {
			printf(
					"No file given as paramater, and no default file. Quitting\n");
			exit(1);
		}
	}
	printf("Loading configuration file: %s\n", configFilename);

	string scannedLine;
	char* linha = NULL;
	string key;
	string value;

	while (!configFile.eof()) {
		getline(configFile, scannedLine);
		if (scannedLine.size() == 0 || scannedLine[0] == '#'
				|| scannedLine[0] == '\r' || scannedLine[0] == '\n') {
			continue; //It's a comment or null line
		} else {
			linha = (char*) malloc(
					int(sizeof(char) * (scannedLine.size() + 1)));
			strcpy(linha, scannedLine.c_str());
			char* tok = strtok(linha, " =");
			key = string(tok);
			tok = strtok(NULL, " =\r");
			value = string(tok);
			Utils::configMap[key] = value;
		}
	}
	configFile.close();
	printf("Configuration file loaded\n");
}

int main(int argc, char **argv) {
	if (argc > 2) {
		printf("Invalid number of arguments.\n");
		return EXIT_FAILURE;
	} else {
		if (argc == 2)
			loadConfig(argv[1]);
		else
			loadConfig(NULL);

		// Detecting the algorithm to use
		string algorithm = Utils::configMap[string("algorithm")];

		if (!algorithm.compare("football")) {
			map<unsigned int, FootballLeague*> leagues =
					DataLoad::analyseFolder(
							Utils::configMap[string("dirCotacoes")]);
			printf("Executing Football\n");
			try {
				predictLeagues(leagues);
			} catch (exception& e) {
				cout << e.what() << '\n';
			}

		} else {
			printf("Unknown Algorithm:%s\n", algorithm.c_str());
		}

	}
	return EXIT_SUCCESS;
}

void predictLeagues(map<unsigned int, FootballLeague*> leagues) {
	map<unsigned int, FootballLeague*>::iterator i;
	for (i = leagues.begin(); i != leagues.end(); i++) {
		printf("League: %s\n", i->second->getName().c_str());
		RatingFactory *rf = new RatingFactory();
		PredictLeague *plp = new PredictLeague(i->second, rf);
		//i->second->debugPrint();
		//plp->predict(5, 6);
		//plp->printResultsHDL();
		//(*i).second->printStats(true);
		//for (int j = 15; j <= 25; j++) {
			//HDAStrategy* strat = new HDAStrategy(plp, 13);
			UOStrategy* strat = new UOStrategy(plp, 9);
			BackTrack* bt = new BackTrack(strat);
			bt->run();
			delete strat;
			delete bt;
		//}
		delete plp;
	}
	printf("Finished\n");
}
