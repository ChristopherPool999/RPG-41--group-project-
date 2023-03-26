//Meagan Eggert, Cassie Valdez, Christopher Pool
//Eggert: heros & monsters, proper class design, inheritence, speed, pick up
//Valdez: data structures, load, save
//Pool: functional world map, walk, bounce obastcles, fight, beat game 
#include "map.h"
#include <unistd.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <time.h>
#include <CImg.h>
#include <cstdio>
#include <termios.h>
#include "fruits.h"
#include <memory>

using namespace cimg_library;
using namespace std;
#include <sstream>
#pragma once

const int MAX_FPS = 90; //Cap frame rate
const unsigned int TIMEOUT = 10; //Milliseconds to wait for a getch to finish
const int UP = 65; //Key code for up arrow
const int DOWN = 66;
const int LEFT = 68;
const int RIGHT = 67;

//Turns on full screen text mode
void turn_on_ncurses() {
        initscr();//Start curses mode
        start_color(); //Enable Colors if possible
        init_pair(1,COLOR_WHITE,COLOR_BLACK); //Set up some color pairs
        init_pair(2,COLOR_CYAN,COLOR_BLACK);
        init_pair(3,COLOR_GREEN,COLOR_BLACK);
        init_pair(4,COLOR_YELLOW,COLOR_BLACK);
        init_pair(5,COLOR_RED,COLOR_BLACK);
        init_pair(6,COLOR_MAGENTA,COLOR_BLACK);
        clear();
        noecho();
        cbreak();
        timeout(TIMEOUT); //Set a max delay for key entry
}

//Exit full screen mode - also do this if you ever want to use cout or gtest or                                                                                         something
void turn_off_ncurses() {
        clear();
        endwin(); // End curses mode
        if (system("clear")) {}
}

void displayMessage(auto message, float duration) {
    turn_off_ncurses();
	cout << message << endl;
	usleep(duration);
    turn_on_ncurses();
}

bool speed_sort(const shared_ptr<Actor>& lhs, const shared_ptr<Actor>& rhs) {
	return lhs->get_speed() >  rhs->get_speed();
}

string fightVisuals(vector<shared_ptr<Actor>> &party, vector<shared_ptr<Actor>> &enemies,
			int target, int partyMember, string message = "") {
	ostringstream visuals;
    for (int i = 0; i < enemies.size(); i++) {
		string hp = to_string(enemies[i]->get_health());
		if (hp.length() <= 1) hp = "0" + hp;
		if (i == target) visuals << "\033[91m" << hp << "hp " << enemies[i]->get_name() << "\n";
		else visuals << "\033[0m" << hp << "hp " << enemies[i]->get_name() << "\n";
	}
	visuals << "\n";
	for (int i = 0; i < party.size(); i++) {
        string hp = to_string(party[i]->get_health());
        if (hp.length() <= 1) hp = "0" + hp;
        if (i == partyMember) visuals << "\033[92m" << hp << "hp " << party[i]->get_name() << "\n";
        else visuals << "\033[0m" << hp << "hp " << party[i]->get_name() << "\n";
	}
	visuals << "\033[0m" << "\n" << "Choose a target to attack (use arrow keys and enter)\n" << message;
	return visuals.str();
}

bool checkIfFightWon(vector<shared_ptr<Actor>> &party, vector<shared_ptr<Actor>> &enemies, string visuals) {
    bool hasWon = true;
    bool isPartyDead = true;
    for (auto enemy : enemies) if (enemy->get_health() > 0) hasWon = false;
    for (auto hero : party) if (hero->get_health() > 0) isPartyDead = false;
    if (isPartyDead) {
        displayMessage("The party was wiped out.... (no autosave because this is 1999)", 2'000'000);
        exit(1);
    }
    return hasWon;
}

void fightMonster(vector<shared_ptr<Actor>> &party, int killsNeeded) {
	displayMessage("A group of vegatables appeared!", 1'000'000);
    srand(time(NULL));
	int amountOfMonsters = 1 + (rand() % 4);

    vector<shared_ptr<Actor>> enemies;
	for (int i = 0; i < amountOfMonsters; i++) { // generates random encounter w/ random # of mobs
		int monsterType = 1 + (rand() % 4);
		if (monsterType == 1) enemies.push_back(make_shared<Watermelon>());
		else if (monsterType == 2) enemies.push_back(make_shared<Lemon>());
		else if (monsterType == 3) enemies.push_back(make_shared<Banana>());
		else enemies.push_back(make_shared<Lychee>());
	}
	vector<shared_ptr<Actor>> turnOrder;
	turnOrder.insert(turnOrder.end(), enemies.begin(), enemies.end());
	turnOrder.insert(turnOrder.end(), party.begin(), party.end());
	sort(turnOrder.begin(), turnOrder.end(), speed_sort);

	int turn = 0;
	int ally = 0;
	int target = 0;
	string battleMessage = "";
	srand(time(NULL));
	while (true) { // turn base loop
		if (enemies[target]->get_health() <= 0) {
			while (true) {
				if (++target == enemies.size()) target = 0; 
				if (enemies[target]->get_health() != 0) break;
			}
		}
		if (turn == turnOrder.size()) turn = 0;
		if (!turnOrder[turn]->get_health()) {
			turn++;
			continue;
		}
		if (turnOrder[turn]->checkIfEnemy()) { // enemy turn
        	int damage = turnOrder[turn]->get_damageDealt();
			int attackAlly = 0;
			while (true) { // attack rand alive ally
            	attackAlly = rand() % 4;
				if (party[attackAlly]->get_health() > 0) break;
			}
            party[attackAlly]->apply_damage(damage);
			battleMessage = (turnOrder[turn++]->get_name() + " dealt " + 
					to_string(damage) + " to " + party[attackAlly]->get_name() + ".");
			string visuals =  fightVisuals(party, enemies, target, ally, battleMessage);
			displayMessage(visuals, 2'000'000);
			flushinp();
			battleMessage = "";
        }
		else {
			int input = getch();
        	if (input == 65) { // move target cursor up
            	while (true) {
                	if (--target < 0) target = enemies.size() - 1; // move target cursor down
					if (enemies[target]->get_health() != 0) break;
				}            	
        	}
        	if (input == 66) { // move target cursor down
                while (true) {
                	if (++target >= enemies.size()) target = 0 ;
					if(enemies[target]->get_health() > 0) break;
				}
        	}
        	if (input == (int)'\n') { // enter key
				int predamageHP = enemies[target]->get_health();
                int damage = party[ally]->get_damageDealt();
                enemies[target]->apply_damage(damage);
				int damageDone = enemies[target]->get_health() - predamageHP; // hidden info changes dmg taken
                while (true) {
					if (++ally >= party.size()) ally = 0;
					if (party[ally]->get_health() > 0) break;
				}
            	battleMessage = (turnOrder[turn++]->get_name() + " dealt " + 
                	    to_string(abs(damageDone)) + " to " + enemies[target]->get_name() + ".");
            	string visuals = fightVisuals(party, enemies, target, ally, battleMessage);
            	displayMessage(visuals, 2'000'000);
				flushinp();
            	battleMessage = "";
			}
		}
        string visuals = fightVisuals(party, enemies, target, ally, battleMessage);
        displayMessage(visuals, 80'000);
		if (checkIfFightWon(party, enemies, visuals)) {
			displayMessage(visuals, 1'000'000);
			string victoryMessage = ("Victory! We need to win " + to_string(killsNeeded) + " more battles!");
			displayMessage(victoryMessage, 1'000'000);
			flushinp();
			break;
		}
	}
}

void add_health(shared_ptr<Actor> a) {
	int currentHealth = a->get_health(); 
	int boost = 1;                     
    int new_health = currentHealth + boost;
    a->set_health(new_health);
}

void add_power(shared_ptr<Actor> a) {
   	int currentPower = a->get_power();
    int boost = 1;
    int new_power = currentPower + boost;
    a->set_power(new_power);
}

void add_speed(shared_ptr<Actor> a) {
    int currentSpeed = a->get_speed();
    int boost = 1;
    int new_speed = currentSpeed + boost;
    a->set_speed(new_speed);
}

void pick_up_treasure(vector<shared_ptr<Actor>> &vec) {
	srand(time(NULL));
    int whichTreasure = 1 + (rand() % 3);
    for (const auto &actor : vec) { 
        if (whichTreasure == 1) add_health(actor);
		else if (whichTreasure == 2) add_power(actor);
		else add_power(actor);
	}
    string potionType = "health";
    if (whichTreasure > 1) potionType = (whichTreasure == 2 ? "power" : "speed");
    displayMessage(("The party found a " + potionType + " potion!"), 1'000'000);
	flushinp();
}

void reverseDirInput(int ch, int &x, int &y) {
    if (ch == UP) y++;
    else if (ch == DOWN) y--;
    else if (ch == RIGHT) x--;
    else if (ch == LEFT) x++;
}

int main() {
//credit: got from  this SO post https://stackoverflow.com/questions/6899025/hide-user-input-on-password-prompt 
	   
		bool newGame = false;
		string input = "";
		cout << "new game? No save will result in new game either way.(y / n)";
		cin >> input;
		if (input == "y" or input == "Y") newGame = true;

		termios oldt; // removes echo even when ncurses is off for unix systems
    	tcgetattr(STDIN_FILENO, &oldt);
    	termios newt = oldt;
    	newt.c_lflag &= ~ECHO;
    	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

		// ncurses was here before
        Map map;
		int tempX = 0, tempY = 0;
        int x = Map::SIZE / 2, y = Map::SIZE / 2;
        int old_x = -1, old_y = -1;
		vector<shared_ptr<Actor>> party;
		party.push_back(make_shared<Chef>());
		party.push_back(make_shared<Trainer>());
		party.push_back(make_shared<Nutritionist>());
		party.push_back(make_shared<Ninja>());
		string data = map.loadFile();
		int mapSize = 100;
	    if (data.length() != 0 and !newGame) {
			int dataLoc = 0;
			int x = 0, y = 0; 
			while (y != mapSize) {
				map.setCoords(x++, y, data[dataLoc++]);
				if (x == mapSize) {
					x = 0;
					y++;
				}
			}
			vector<string> dataKeys;
			string temp = "";
			bool inWord = false;
			while (++dataLoc != data.length()) {
         		 if (data[dataLoc] == '\n' or data[dataLoc] == ' ') inWord = false;
				 else inWord = true;
				 if (inWord) temp.push_back(data[dataLoc]);
				 if (temp.length() and !inWord) {
					dataKeys.push_back(temp);
				 	temp = "";
				 }
			}
           	tempX = stoi(dataKeys[0]);
            tempY = stoi(dataKeys[1]);			
		}
		
      	sort(party.begin(), party.end(), speed_sort);
		bool isBounce = false;
		int bounceTimeout = 0;
		int bounceDir = 0;
		int monstersKilled = 0;
		const int killsNeeded = 5;
		if (!newGame) {
			x = tempX; y = tempY;
		}
		turn_on_ncurses();
        while (true) {
				if (monstersKilled == killsNeeded) {
					displayMessage("You have beaten the game!", 2'000'000);
					exit(1);
				}
                int ch = getch();
				if (isBounce and bounceTimeout != 25) {
					if (!bounceTimeout) {
						bounceTimeout = 50;
            			reverseDirInput(bounceDir, x, y);
					}
					--bounceTimeout;
					if (!bounceTimeout) isBounce = false;
					continue;
				}
				if (bounceTimeout == 25) { // will bounce player back + addition 25 wait after
					bounceTimeout--;
					ch = ERR;
				}
                if (ch == 'q' || ch == 'Q') break;
				else if (ch == RIGHT) {
					x++;
					if (x >= Map::SIZE) x = Map::SIZE - 1;
				} else if (ch == LEFT) {
					x--;
					if (x < 0) x = 0;
				} else if (ch == UP) {
					y--;
					if (y < 0) y = 0;
				} else if (ch == DOWN) {
					y++;
					if (y >= Map::SIZE) y = Map::SIZE - 1;
				} else if (ch == ERR) {
					; //Do nothing
				}
				char tile = map.seeAtCoords(x, y);
    			if (tile == '#' or tile == '~') {
    				isBounce = true;
    				bounceDir = ch;
				}
    			else if (tile == 'M') {
					map.setCoords(x, y, '.');
					fightMonster(party, killsNeeded - ++monstersKilled);
				}
    			else if (tile == '$') {
					map.setCoords(x, y, '.');
					pick_up_treasure(party);
				}
                if (x != old_x or y != old_y) {
                        map.draw(x,y);
                        mvprintw(Map::DISPLAY+1,0,"X: %i Y: %i\n",x,y);
                        refresh();
                }
                old_x = x;
                old_y = y;
                usleep(1'000'000/MAX_FPS);
        }
        turn_off_ncurses();
		map.saveFile(x, y, party);
}
