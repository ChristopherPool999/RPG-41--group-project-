#include <string>
#include <random>
#include <time.h>
#include <ctime>
using namespace std;
#pragma once

// Base Class

class Actor {
    protected:
    int health = 10;
    int speed;
    int power;
    int damage = 1;
    int damageDealt; //baseDamage * special + power
	string name;
    bool isEnemy;

    public:
    Actor() {
        health = 10;
        speed = 0;
        power = 0;
        damage = 1;
    }
    Actor(int new_health, int new_speed, int new_power){
        health = new_health;
        speed = new_speed;
        power = new_power;
    }
    virtual void set_health(int new_health) {
        health = new_health;
		if (new_health < 0) health = 0;
    }
    virtual void set_speed(int new_speed) {
        speed = new_speed;
    }
    virtual void set_power(int new_power) {
        power = new_power;
    }
    virtual void set_damage(int new_damage) {
        damage = damage + new_damage;
    }
    virtual void apply_damage(int enemy_damage) {
        health -= enemy_damage;
		if (health < 0) health = 0;
    }

    int get_health() const {
		if (health < 0) return 0;
        return health;
    }
    int get_speed() const {
        return speed;
    }
    int get_power() const {
        return power;
    }
    int get_damage() const {
        return damage;
    }
    int get_damageDealt() const {
        return damageDealt;
    }
	string get_name() const {
		return name;
	}
    bool checkIfEnemy() const {
        return isEnemy;
    }
    virtual const char* get_attack() {
        return "The actor fights with a basic attack!";
    }
    friend bool operator<(const Actor& L, const Actor& R){
            return L.get_speed() < R.get_speed();
    }
};

//Hero Class

class Hero : public Actor {
    protected:
    int goodIntent = 1 + (rand() % 10); //Increases Health
    int adrenaline = 1 + (rand() % 4); //Increases Power

    public:
    Hero() {
        health = 10;
        speed = 3;
        power = 4;
        damage = 5;
        damageDealt = damage + power;
    }

    void set_power(int new_power) override {
        power = new_power + adrenaline;
    }
    void set_health(int new_health) override {
        health = 10 + new_health + goodIntent;
    }
    int get_intent() const {
        return goodIntent;
    }
    const char* get_attack() override {
        return "The hero fights with a heroic attack!";
    }

};

//Monster Class

class Monster : public Actor {
    protected:
    int badIntent = 1 + (rand() % 5); //Increases Power
    int recklessness = 1 + (rand() % 2); //Reduces Health

    public:
    Monster() {
        health = 10;
        speed = 1;
        power = 2;
        damage = 3;
        damageDealt = damage + power;
    }

    void set_health(int new_health) override {
        health = new_health - recklessness;
    }
    void set_power(int new_power) override {
        power = new_power + badIntent;
    }
    int get_intent() const {
        return badIntent;
    }
    int get_recklessness() const {
        return recklessness;
    }
    const char* get_attack() override {
        return "The monster attacks monstrously!";
    }

};

//Individual Monsters

class Watermelon : public Monster {
    protected:
    int thickRind = 2 + (rand() % 5); //Additional Defense
    int heavyWeight = rand() %5; //Additional Power

    public:
    Watermelon() {
        health = 5 + (rand() % 20) - recklessness;
        speed = 1 + (rand() % 10);
        power = (rand() % 5) + badIntent + heavyWeight;
        damage = rand() % 6;
        damageDealt = damage + power;
		name = "Watermelon";
        isEnemy = true;
    }

        void apply_damage(int enemy_damage) override {
                health = health - ceil(enemy_damage*(1.0/thickRind));
        }
        int get_defense() const {
        return thickRind;
    }
    const char* get_attack() override {
        return "The watermelon has thick skin!";
    }

};

class Lemon : public Monster {
    protected:
    int sourPower = 2 + (rand() % 5); //Special Attack Power

    public:
    Lemon() {
        health = 5 + (rand() % 20) - recklessness;
        speed = 1 + (rand() % 10);
        power = (rand() % 5) + badIntent;
        damage = rand() % 6;
        damageDealt = damage*sourPower + power;
		name = "Lemon";
        isEnemy = true;
    }

    void apply_damage(int enemy_damage) override {
        health -= enemy_damage;
    }
    int get_sour() const {
        return sourPower;
    }
    const char* get_attack() override {
        return "The lemon attacked you with extra sour power!";
    }

};

class Banana : public Monster {
    protected:
    int makeEmSlip = 1 + (rand() % 5); //Reduce Hero Accuracy
    int overRipe = 1 + (rand() & 3); //Reduce Power

    public:
    Banana() {
        health = 5 + (rand() % 20) - recklessness;
        speed = 1 + (rand() % 10);
        power = (2 + rand() % 5) + badIntent - overRipe;
        damage = rand() % 6;
        damageDealt = damage + power;
		name = "Banana";
        isEnemy = true;
    }
        void apply_damage(int enemy_damage) override {
        int slip_test = 1 + (rand() % 10);
                int slip_factor = get_slippage();
                if (slip_test >= slip_factor){
                        health -= enemy_damage;
                } else {
                        health -= 0;
                }
    }
    int get_slippage() const {
        return makeEmSlip;
    }
    int get_ripeness() const {
        return overRipe;
    }
    const char* get_attack() override {
        return "The banana reduced your accuracy!";
    }

};

class Lychee : public Monster {
    protected:
    int sugarShock = 2 + (rand() % 5); //Stun Hero -- Reduce Hero Speed (Actually Increases Lychee's Speed)

    public:
    Lychee() {
        health = 5 + (rand() % 20) - recklessness;
        speed = 1 + (rand() % 10) + sugarShock;
        power = (rand() % 5) + badIntent;
        damage = rand() % 6;
        damageDealt = damage + power;
		name = "Lychee";
        isEnemy = true;
    }
    int get_sugar() const {
        return sugarShock;
    }
    void apply_damage(int enemy_damage) override {
        health -= enemy_damage;
    }
    const char* get_attack() override {
        return "You were slowed by lychee's sugar shock!";
    }
};

//Individual Heroes

class Ninja : public Hero {
    protected:
    int knifeSkills = 2 + (rand() % 5); //Increases Damage
    int quickness = 1 + (rand() % 5); //Increased Speed

    public:
    Ninja() {
        health = 5 + (rand() % 20) + goodIntent;
        speed = 1 + (rand() % 10) + quickness;
        power = (rand() % 5) + adrenaline;
        damage = rand() % 6;
        damageDealt = damage*knifeSkills + power;
		name = "Ninja";
        isEnemy = false;
    }

    void apply_damage(int enemy_damage) override {
        health -= enemy_damage;
    }
    int get_knife() const {
        return knifeSkills;
    }
    int get_quickness() const {
        return quickness;
    }
    const char* get_attack() override {
        return "The ninja sliced through the fruit with his katana.";
    }
};

class Nutritionist : public Hero {
    protected:
    int moderation = 1 + (rand() % 5); //Increased Defense
    int goodChoices = 1 + (rand() %3); //Increased Damage

    public:
    Nutritionist() {
        health = 5 + (rand() % 20) + goodIntent;
        speed = 1 + (rand() % 10);
        power = (rand() % 5) + adrenaline;
        damage = rand() % 6;
        damageDealt = damage*goodChoices + power;
		name = "Nutritionist";
        isEnemy = false;
    }

    void apply_damage(int enemy_damage) override {
        health = health - ceil(enemy_damage*(1.0/moderation));
    }
        int get_moderate() const {
        return moderation;
    }
    int get_choices() const {
        return goodChoices;
    }
    const char* get_attack() override {
        return "The nutritionist knows how to defend themselves against sugary fruits.";
    }
};

class Trainer : public Hero {
    protected:
    int muscles = 2 + (rand() % 5); //Increases Power
    int workout = 1 + (rand() % 3); //Increases Health

    public:
    Trainer() {
        health = 5 + (rand() % 20) + goodIntent + workout;
        speed = 1 + (rand() % 10);
        power = (rand() % 5) + adrenaline + muscles;
        damage = rand() % 6;
        damageDealt = damage + power;
		name = "Trainer";
        isEnemy = false;
    }

    void set_health(int new_health) override {
        health = 10 + new_health + goodIntent + workout;
    }
    void apply_damage(int enemy_damage) override {
        health -= enemy_damage;
    }
    int get_muscles() const {
        return muscles;
    }
    int get_workout() const {
        return workout;
    }
    const char* get_attack() override {
        return "The trainer works out too much to be affected by mere fruit.";
    }
};

class Chef : public Hero {
    protected:
    int knifeSkills = 2 + (rand() % 8); //Increases Damage
    int serverShield = 1 + (rand() % 10); //Server takes all the damage before it comes down on the chef

    public:
    Chef() {
        health = 5 + (rand() % 20) + goodIntent;
        speed = 1 + (rand() % 10);
        power = (rand() % 5) + adrenaline;
        damage = rand() % 6;
        damageDealt = damage*knifeSkills + power;
		name = "Chef";
        isEnemy = false;
    }

    void apply_damage(int enemy_damage) override {
        int damageDifference = enemy_damage - serverShield;
        if (damageDifference > 0 && serverShield > 0){
            serverShield -= enemy_damage;
            health -= damageDifference;
        } else if (damageDifference <= 0 && serverShield > 0) {
            serverShield -= enemy_damage;
        } else if (serverShield <= 0) {
            health -= enemy_damage;
        }
    }
    int get_knife() const {
        return knifeSkills;
    }
    int get_server() const {
        return serverShield;
    }
    const char* get_attack() override {
        return "The ninja sliced through the fruit with his katana.";
    }
};
