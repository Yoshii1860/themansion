#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cs50.h>
#include <conio.h>
#include <ctype.h>

#ifdef _WIN32
    #include <Windows.h>
#else
    #include <unistd.h>
    #include <ncurses.h>
#endif

// ##################################################
// DATA STRUCTURES
// ##################################################
    
// Items with name, amount and bool if picked up or not
typedef struct item
{
    string name;
    int amount;
    bool picked_up;
}
item;

// Weapons
typedef struct weapon
{
    string name;
    int ammo;
    string ammo_name;
    int min_damage;
    int max_damage;
    bool picked_up;
}
weapon;

// Zombie setup
typedef struct zombie
{
    int min_attack;
    int max_attack;
    int life;
    bool is_dead;
}
zombie;

// Room setup
typedef struct room
{
    string name;
    string floor_name;
    bool visited;
    int zombie_amount;
    int item_amount;
    int weapon_amount;
    int searched;
    bool riddle;
    zombie *enemies[3];
    item *items[4];
    weapon *weapons[4];
    struct room *next_room[6];
    void (*func)(player); 
}
room;

// Player with inventory of items, health points and current equipped weapon and shells
typedef struct player
{
    item *inventory[9];
    weapon *weapons[3];
    weapon *active_weapon;
    room *current_room;
    int health;
    int lives;
}
player;

// Struct for Maze to run through it
typedef struct maze_path
{
    int num;
    struct maze_path *available_paths[3];
    int end_of_path;
}
maze_path;

// ##################################################
// RIDDLES
// ##################################################

// Picture Place Game
void GuestRoomRiddle(player *player, room *previous_room);
void check(player *player, int *row, int *col, room *previous_room);
char array[3][3] = {{'K', 'B', 'X'},
                    {'D', 'I', 'E'},
                    {' ', 'L', 'Y'}};
char sample[3][3] = {{'K', 'B', 'X'},
                    {'D', 'I', 'E'},
                    {' ', 'L', 'Y'}};

// Inspecting items
void DiningRoomRiddle(player *player, room *previous_room);

// Removing planes
void VestibuleRiddle(player *player, room *previous_room);

// Riddle converting letter to binary code
void LaundryRoomRiddle(player *player, room *previous_room);

// ##################################################
// GLOBAL VARIABLES
// ##################################################

    // array to store all zombies in the game
zombie zombieArray[20];
    // size of floors of the mansion
const int floor_size = 5;
    // size of rooms in the attic
const int attic_size = 3;
    // size of rooms on the second floor
const int second_size = 6;
    // size of rooms in the courtyard
const int outside_size = 2;
    // size of rooms on the first floor
const int first_size = 8;
    // size of rooms in the basement
const int basement_size = 4;
    // size of enemies array in each room
const int enemies_array_size = 3;
    // size of items array in each room
const int items_array_size = 4;
    // size of room array in each room
const int next_array_size = 6;
    // size of weapons array in each room
const int weapon_array_size = 4;
    // size of zombies in the game
const int zombie_amount = 10;
    // boolean to see if boss is dead or alive
bool boss_attic = true;
    // boolean to see if boss is dead or alive
bool boss_basement = true;
    // boolean to see if boss is dead or alive
bool boss_maze = true;
    // Path size for each point in the maze
const int available_paths_size = 3;
    // start of maze
maze_path *p;
    // boolean for printing choices
bool printed = false;

// ##################################################
// CONVERSATION ARRAYS
// ##################################################

    // Messages for appearing zombies
string appearing_messages[5] = {
    "\nYou're being attacked by rotting, flesh-hungry zombies, their decomposing limbs propelling them forward as they relentlessly chase after you with a ravenous hunger for your flesh.\n", 
    "\nThe undead are closing in on you, their decomposing bodies shuffling forward as they gnash their teeth, their ragged moans filling the air.\n", 
    "\nThe zombies are relentless, their hollow eyes fixated on you as they shamble closer and closer, their outstretched arms reaching for you.\n", 
    "\nThe zombies are on your tail, their putrid stench filling your nostrils as they stumble towards you, their once-human faces twisted in a grotesque snarl.\n", 
    "\nYou're surrounded by the walking dead, their rotting flesh and tattered clothing sending shivers down your spine as they close in, eager to sink their teeth into your flesh.\n"
    };
    // Messages when zombie is hit by a bullet
string hit_messages[5] = {
    "\nThe bullet whizzed past the zombie's ear, barely missing it as the monster continued to lurch forward, fueled by its insatiable hunger.\n", 
    "\nThe shot hit the zombie square in the shoulder, causing it to stumble but not fall, as its rotting flesh sizzled and smoked from the impact.\n", 
    "\nThe zombie's moans grew louder as the bullet tore through its flesh, splintering bones and shredding muscle, but still not putting it down.\n", 
    "\nThe sound of the gun echoed through the desolate room as the bullet penetrates the zombie's chest, but the monster refuses to die, continuing its relentless pursuit.\n", 
    "\nThe zombie reels from the impact of the bullet, its tattered clothes and mangled flesh adding to its already frightening appearance.\n"
    };
    // Messages when zombie dies by a bullet
string zombie_dies_messages[5] = {
    "\nThe shot rang out, echoed through the abandoned building as the bullet found its mark, penetrating the zombie's skull and putting an end to its relentless pursuit.\n", 
    "\nThe zombie's body convulsed as the bullet tore through its head, sending a spray of dark, putrid blood and chunks of brain matter flying through the air.\n", 
    "\nThe monster's moans turned to gurgles as the bullet entered its chest, causing its lifeless body to crumple to the ground in a heap of tangled limbs and decaying flesh.\n", 
    "\nThe shot echoed through the desolate room, accompanied by the sound of bones breaking and flesh tearing as the bullet destroys the zombie's head and sends it crashing to the ground.\n", 
    "\nThe zombie's eyes rolled back in its head as the bullet penetrated its brain, sending it crashing to the ground in a shower of black, thick blood and shattered bone.\n"
    };
    // Messages when zombie is hit by a knife
string hit_messages_knife[5] = {
    "\nThe knife whizzed past the zombie's face, barely missing the brain as the monster continued to lurch forward, fueled by its insatiable hunger.\n", 
    "\nThe shot hit the zombie square in the shoulder, causing it to stumble but not fall, as its rotting flesh sizzled and smoked from the impact.\n", 
    "\nThe zombie's moans grew louder as the knife tore through its flesh, splintering bones and shredding muscle, but still not putting it down.\n", 
    "\nThe sound of the gun echoed through the desolate room as the knife penetrates the zombie's chest, but the monster refuses to die, continuing its relentless pursuit.\n", 
    "\nThe zombie reels from the impact of the knife, its tattered clothes and mangled flesh adding to its already frightening appearance.\n"
    };
    // Messages when zombie dies by a knife
string zombie_dies_messages_knife[5] = {
    "\nThe hit rang out, echoed through the abandoned building as the knife found its mark, penetrating the zombie's skull and putting an end to its relentless pursuit.\n", 
    "\nThe zombie's body convulsed as the knife tore through its head, sending a spray of dark, putrid blood and chunks of brain matter flying through the air.\n", 
    "\nThe monster's moans turned to gurgles as the knife entered its chest, causing its lifeless body to crumple to the ground in a heap of tangled limbs and decaying flesh.\n", 
    "\nThe hit echoed through the desolate room, accompanied by the sound of bones breaking and flesh tearing as the knife destroys the zombie's head and sends it crashing to the ground.\n", 
    "\nThe zombie's eyes rolled back in its head as the knife penetrated its brain, sending it crashing to the ground in a shower of black, thick blood and shattered bone.\n"
    };
    // Messages when all zombies in this room are killed
string win_messages[5] = {
    "\nYou win! The zombie's final death throes echoed through the desolate room as the survivor watches, their heart pounding in their chest as the realization of what they've just done sets in.\n", 
    "\nYou win! The survivor's hands shake as they pull the weapon from the zombie's lifeless body, its dark, thick blood staining their hands and clothing.\n", 
    "\nYou win! The survivor's breath comes in short gasps as they back away from the fallen zombie, the sounds of its death rattles still ringing in their ears.\n", 
    "\nYou win! The zombie's body lies still on the ground, its eyes staring blankly into the darkness as the survivor's mind reels from the adrenaline-fueled battle.\n", 
    "\nYou win! The survivor's heart races as they stare down at the zombie they just killed, their thoughts consumed by the horror of what they've just been through and the fear of what may come next.\n"
    };
    // Messages when zombie misses his target
string zombie_missed_messages[5] = {
    "\nThe zombie lurched forward, its ragged claws just missing the survivor's skin as they stumbled back in fear.\n", 
    "\nThe zombie's arms flailed in the darkness, its decaying fingers grasping wildly as the survivor ducked and weaved to avoid its deadly embrace.\n", 
    "\nThe survivor's heart raced as the zombie's bony hand shot out, narrowly missing their face as they twisted away at the last moment.\n", 
    "\nThe zombie's fetid breath washed over the survivor as its claw-like fingers slashed through the air, barely missing their flesh.\n", 
    "\nThe survivor's eyes widened in fear as the zombie's mangled arm reached out, its fingers inches from their skin before missing its target in a shower of sparks.\n"
    };
    // Messages if player dies
string death_messages[5] = {
    "\nYour vision faded to black as the monster's jaws closed around your throat, its rancid breath the last thing you ever felt.\n", 
    "\nThe monster's claws dug deep into your flesh, tearing apart your body as you screamed in terror and pain, the game finally ending in death.\n", 
    "\nThe last thing you saw was the monster's rotting face, its eyes devoid of life and its mouth twisted into a grotesque snarl as it claimed your soul.\n", 
    "\nThe monster's hands tightened around your neck, choking the life from your body as the screen faded to black, signaling the end of the game and your life.\n", 
    "\nYour screams echoed in the darkness as the monster's teeth sank into your flesh, the screen turning red as your final moments were consumed by pain and terror.\n"
    };
    // Messages if zombie hits the player
string zombies_hit_messages[5] = {
    "\nThe zombie's teeth sank into the survivor's flesh, its razor-sharp incisors tearing through skin and muscle as they screamed in pain.\n", 
    "\nThe survivor stumbled back as the zombie's claws raked across their chest, leaving deep gashes in their flesh and a trail of blood in their wake.\n", 
    "\nThe zombie's snarling jaws closed around the survivor's arm, its teeth sinking deep into the flesh and leaving a jagged, bloody wound.\n", 
    "\nThe survivor fell to the ground as the zombie's claws tore into their flesh, leaving deep gouges in their skin and a searing pain in their bones.\n", 
    "\nThe zombie's hands closed around the survivor's neck, its fingers digging deep into their flesh as they gasped for breath and struggled to break free.\n"
    };
    // Messages when boss gets hit by the player
string boss_hit[5] = {
    "\nYour heart pounds as you aim for the creature's vulnerable spot.\nWith a fierce blow, you connect with the glistening green plate on its chest.\nA geyser of noxious green goo spurts forth from the monster's wound,\nfueling its rage even further as it roars in fury.\n",
    "\nYou take a deep breath, then target with deadly precision.\nThe green plate shatters a bit, and a spray of putrid goo shoots forth,\nsending the monster into a frenzy.\n",
    "\nYour weapon pierces the soft, vulnerable green plate with a satisfying crunch.\nA flood of sickly green ichor gushes from the wound, enraging the monster.\n",
    "\nWith a fierce cry, you target the monster's chest.\nThe green plate gives further way under the blow,\nand a torrent of green ooze pours forth, fueling the beast's anger.\n",
    "\nThe bullet connects with the weak point, causing the green plate to crack.\nA spray of noxious goo erupts from the wound, and the monster lets out a rage-filled roar.\n"
    };
    // Messages if player misses the boss
string boss_miss[5] = {
    "\nYour aim falters, and your weapon misses its mark.\nThe monster seems to sense your fear, and its eyes gleam with an unholy light.\n",
    "\nWith a heart-stopping moment, you realize your shot has gone wide.\nThe monster sneers, savoring your terror as it prepares to attack.\n",
    "\nYour hand trembles as you miss your shot, and the monster howls in triumph.\nIts eyes gleam with malicious glee, sensing its impending victory.\n",
    "\nYour weapon misses its target, and the monster lets out a mocking cackle.\nThe darkness around you deepens as the monster advances, hungering for your blood.\n",
    "\nYou miss your shot, and the monster cackles in derision.\nThe shadows around you seem to come alive as the monster closes in, ready to strike.\n"
    };
    // Messages if boss is dodged
string boss_dodge[5] = {
    "\nYou duck and roll, narrowly avoiding the monster's massive claws.\nIt snarls in frustration, its hot breath washing over you as it prepares to strike again.\n",
    "\nWith a burst of speed, you leap out of the monster's reach.\nIt hisses in anger, its eyes blazing as it searches for another opening.\n",
    "\nYou sidestep the monster's attack, dodging its deadly claws just in time.\nThe monster growls in frustration, its eyes fixated on you as it circles for another strike.\n",
    "\nYou duck and weave, using your agility to avoid the monster's blows.\nIts massive form crashes into walls and furniture as it tries to bring you down.\n",
    "\nYour instincts take over, and you dart out of the way just as the monster strikes.\nIt misses its mark, and you take advantage of the momentary respite to catch your breath.\n"
    };
    // Messages if boss hits player
string boss_damage[5] = {
    "\nWith a sickening thud, the monster's claws tear into your flesh.\n",
    "\nThe monster's attack connects, sending you flying across the room.\n",
    "\nPain lances through your body as the monster's attack lands.\n",
    "\nThe monster's strike hits its mark, and you feel your energy draining away.\n",
    "\nThe monster's blow lands, and you crumple to the ground, your vision blurring.\n"
    };

// ##################################################
// CREATE ITEMS
// ##################################################

    // create all items
item herbs, key_safe, key_second, key_court, key_art, key_laundry, cutter, emblem, fuse;

    // create all weapons
weapon pistol, shotgun, knife;

    // create all rooms
room vestibule, main_attic, storage_attic, walk_in_closet, bedroom, guest_room, interior_balcony, solar, storeroom, cy_entrance, cy_maze, main_hall, dining_room, corridor, kitchen, storage_first, art_room, mirror_room, storage_art, secret_passage, kitchen_basement, corridor_basement, laundry_room, main_basement, main_entrance;

    // Array of rooms for each floor
room attic[3];
room second_floor[6];
room outside[2];
room first_floor[8];
room basement[4];

    // Array of paths in the maze
maze_path paths_array[16];

// ##################################################
// CALL FUNCTIONS
// ##################################################

// Game Start
void Start(void);
void Intro(void);
player CreatePlayer(void);
room CreateMansion(void);
void CreateZombies(int zombie_amount);
void CreateMaze(void);

// Game Commands
void Continue(void);
void ContinueOne(void);
void ShowLocation(player *player);
void ShowInventory(player *player);
void ShowHealth(player *player);
void Help(player *player);
void Action(player *player);
void SearchRoom(player *player);
void ChangeWeapon(player *player);
void ZombieCheck(player *player, room *previous_room);
void ZombieFight(player *player, int *zombie_amount);
void PlayerAttack(player *player, int *zombie_amount);
void ZombieAttack(player *player, int *zombie_amount);
int ZombieAttackLine(int num);
void Death(player *player);
int TestDodge(int num);

// Boss Fights
void BossAttic(player *player, room *previous_room);
void BossBasement(player *player, room *previous_room);
void sleepMilliSecond(long milliSecondInput);
int Skull(int num);
void BossFight(player *player, int *boss_life, bool *boss_alive, int boss_life_display);
    // Maze
void MazeStart(player *player, maze_path *pointer);
void MazeDodge(player *player);
void MazeFinalFight(void);
void MazeStory(void);

// Story Line
void MainEntrance(player *player);
void MainHall(player *player);
void MainHallAction(player *player);
void Courtyard(player *player);
void CourtyardAction(player *player);
void InteriorBalcony(player *player);
void ArtRoom(player *player);
void MirrorRoom(player *player);
void Vestibule(player *player);
void MainAttic(player *player);
void StorageAttic(player *player);
void WalkInCloset(player *player);
void Bedroom(player *player);
void GuestRoom(player *player);
void Solar(player *player);
void Storeroom(player *player);
void Maze(player *player);
void LittleStorage(player *player);
void SecretPassage(player *player);
void DiningRoom(player *player);
void Corridor(player *player);
void Kitchen(player *player);
void KitchenBasement(player *player);
void CorridorBasement(player *player);
void MainBasement(player *player);
void LaundryRoom(player *player);
void StorageChamber(player *player);
void End(void);

// ##################################################
// MAIN GAME
// ##################################################

int main(void)
{
    Start();
    Intro();
    player main_player = CreatePlayer();
    CreateMaze();
    p = &paths_array[0];
    CreateMansion();
    CreateZombies(zombie_amount);
    MainHall(&main_player);
}

// ##################################################
// GAME START FUNCTIONS
// ##################################################

// Start the Game
void Start(void)
{
    printf("\n##################################################\n");
    printf("\nWelcome to 'The Mansion', a thrilling text-based horror survival game.\n\n");
    printf("The game will give you different options to proceed.\n\n");
    printf("When indicated, you can always type '0' [help] to see all possible actions:\n");
    printf("'show inventory', 'show location', 'show health', 'change weapon' or 'use herbs'.\n");
    Continue();
    printf("\nTo read further instructions about the combat system, type '1'.\n");
    printf("Otherwise, type '0' to skip further instructions: ");
    int input = 2;
    while (input != 0 || input != 1)
    {
        if (input == 1)
        {
            printf("\nZOMBIE COMBAT INSTRUCTIONS\n\n");
            printf("When you fight one or more zombies, there's a 80%% chance to hit first.\n");
            printf("You will shoot automatically with your currently equipped weapon.\n");
            printf("When you're out of ammo, the knife will be used automatically.\n");
            Continue();
            printf("\nWhen the zombie attacks, you have to dodge.\n");
            printf("There is a timer of 3 seconds counting down before the event starts.\n");
            printf("Then you will see a print similar to this:\n");
            printf("MMMMMMMMMMMMMMMMMM\033[1;31mXXXX\033[0mMMMMMMMMMMMMMMMMMM\n");
            printf("MMMMMMMMMMMMMM\033[1;33mOOOO\033[0mMMMM\033[1;33mOOOO\033[0mMMMMMMMMMMMMMM\n");
            Continue();
            printf("\nThe red X's will show you the zombie's position.\n");
            printf("The yellow O's will indicate the area where you can dodge.\n");
            printf("A red line will rapidly load from left to right.\n");
            printf("You can press any key to stop the red line.\n");
            Continue();
            printf("\nIf the red line aligns with the dodge area, you dodge and won't take damage.\n");
            printf("Otherwise, you will take damage.\n");
            printf("Let's try it out to practice.\n");
            int test = TestDodge(1);
            if (test == 2)
            {
                TestDodge(2);
            }
            ContinueOne();
            printf("\nDuring a boss fight, the combat system is similar.\n");
            printf("The only difference is, that you have to shoot the boss as well.\n");
            printf("In this case, the area you have to hit is highlighted with green X's.\n");
            printf("END OF INSTRUCTIONS\n");
            break;
        }
        else if (input == 0)
        {
            break;
        }
        scanf("%d", &input);
    }
    string startstr = " ";
    string buff = "start";
    printed = false;
    while(strcmp(startstr, buff) != 0)
    {
        if (!printed)
        {
            printf("\n|| To start game type 'start':");
            printed = true;
        }
        startstr = get_string(" ");
    }
}

// Start the Intro
void Intro(void) 
{
    printf("\n#################### START ####################\n");
    printf("\nThe city of Chicago was in chaos. The zombie outbreak had spread like wildfire and the streets were filled with the walking dead.\nAs a soldier with the national guard, you were tasked with evacuating as many citizens as possible to safety.\nYou joined forces with a unit and headed out into the city, but it quickly became clear that the mission was far from easy.\n");
    Continue();
    printf("\nAt every turn, you were faced with new dangers and challenges. The zombies were relentless and seemed to be everywhere.\nThe unit fought bravely, but soon found themselves overwhelmed by the sheer numbers of the undead.\nIn the heat of the battle, you were separated from the rest of the group and found yourself on your own, trying to navigate the city's dark and dangerous streets.\n");
    Continue();
    printf("\nAs you ran for your life, you stumbled upon an abandoned mansion in the woods out in the countryside.\nYou cautiously approached the mansion, wary of any traps or dangers that might lie ahead.\nAs you stepped inside, you felt a sense of relief wash over you.\nThe mansion seemed to be the perfect place to rest, gather your thoughts, and plan your next move.\n");
    Continue();
    printf("\nBut as you explored the mansion's dark and eerie rooms, you began to realize that the zombie outbreak was not the only thing you needed to fear.\nThe mansion seemed to hold secrets and dangers of its own, and you soon found yourself fighting not just for your survival, but for your sanity as well.\n");
    Continue();
}

// ##################################################
// STORY LINE FUNCTIONS
// ##################################################

// start in main hall
void MainHall(player *player)
{
    (*player).current_room = &main_hall;
    if (main_hall.visited == false)
    {
        printf("\n##################################################\n");
        printf("\nYou find yourself in the heart of the mansion, the main hall, where shadows seem to dance along the wall.\n");
        printf("The dim light illuminates several doors that line the sides of the room, each one leading to an unknown destination.\n");
        printf("The grand staircase looms ahead, spiraling upwards towards the depths of the mansion, beckoning you to explore the mysteries that lie above.\n");
        printf("A door at the far end of the hall seems to lead outside, offering a glimpse of the moonlit courtyard beyond.\n");
        Continue();
    }

    if (main_hall.visited == false)
    {
        main_hall.visited = true;
    } 

    Action(player);
}

// Function for the main entrance, also the goal
void MainEntrance(player *player)
{
    if(player->inventory[7]->picked_up == true)
    {
        End();
    }
    else
    {
        if(main_entrance.visited == true)
        {
            printf("On the door you notice a big socket. It appears that an object is meant to be inserted there.\n");
            printf("\nYou can't escape through this door.\n");
        }
        else
        {
            printf("\nAs you attempt to make your escape from the mansion through the grand main entrance, you're met with resistance.\n");
            printf("Despite your efforts to push open the heavy doors, they remain firmly in place, as if held fast by an unseen force.\n");
            printf("Panic starts to set in as you realize that you may be trapped, unable to leave this place of darkness and decay.\n");
            printf("On the door you notice a big socket. It appears that an object is meant to be inserted there.\n");
            main_entrance.visited = true;
        }
        Continue();
        Action(player);
    }
}

// Function for the courtyard entrance
void Courtyard(player *player)
{

    if ((*player).inventory[3]->picked_up != true)
    {
        printf("\nAs you grasp the cold metal handle, you realize with a jolt that the door is firmly locked.\n");
        printf("Your eyes are drawn to the small, grime-covered window set into the door, and through it\n");
        printf("you catch a glimpse of a massive maze, its twisted paths disappearing into the shadows.\n");
        Continue();
        Action(player);
    }
    else
    {
    room *previous_room = (*player).current_room;
    (*player).current_room = &cy_entrance;
    ZombieCheck(player, previous_room);
    if (cy_entrance.visited == false)
    {
        printf("\n##################################################\n");
        printf("\nYou take a deep breath, your heart racing as you open the door and step out into the courtyard.\n");
        printf("Finally you breath fresh air again. You have never thought you would make it.\n");
        printf("But then you realize that the only path is leading to a huge maze, towering over you like an ominous presence.\n");
        printf("The sun is setting, casting a soft orange glow over the maze, and you can just make out the shadows of something lurking in there.\n");
        Continue();
    }

    if (cy_entrance.visited == false)
    {
        cy_entrance.visited = true;
    } 

    Action(player);
    }
}

// Function for the interior balcony
void InteriorBalcony(player *player)
{
    (*player).current_room = &interior_balcony;
    if (interior_balcony.visited == false)
    {
        printf("\n##################################################\n");
        printf("\nYour heart races as you slowly ascend the tall, creaky staircase.\n");
        printf("As you reach the balcony, you gulp at the sight of all the different doors that line the walls.\n");
        printf("The darkness that lies beyond each one feels like a deathly abyss, and you can't help but feel watched.\n");
        printf("You can only take one path, but which one will lead you to safety?\n");
        Continue();
    }

    if (interior_balcony.visited == false)
    {
        interior_balcony.visited = true;
    } 

    Action(player);
}

// Function for the art room
void ArtRoom(player *player)
{
    if ((*player).inventory[4]->picked_up != true)
    {
        printf("\nDespite your attempts to open it, the lock holds fast, the metal bolts firmly in place.\n");
        printf("But it's not just the door that's keeping you from what lies beyond.\n");
        printf("From behind it comes a chorus of low, creepy sounds, whispers and shuffles and the occasional muffled thud that send shivers down your spine.\n");
        Continue();
        Action(player);
    }
    else
    {
    room *previous_room = (*player).current_room;
    (*player).current_room = &art_room;
    ZombieCheck(player, previous_room);
    if (art_room.visited == false)
    {
        printf("\n##################################################\n");
        printf("\nAfter defeating the zombies you take a few steps into the room and your skin prickles with apprehension.\n");
        printf("The walls were lined with grotesque paintings, depicting mutilated bodies and experiments gone wrong.\n");
        printf("The stench of death and despair hung heavily in the air, making you struggle not to retch.\n");
        printf("At the far end of the room, you noticed a door, slightly ajar.\n");
        Continue();
    }

    if (art_room.visited == false)
    {
        art_room.visited = true;
    } 

    Action(player);
    }
}

// Function for the mirror room
void MirrorRoom(player *player)
{
    if ((*player).inventory[6]->picked_up != true)
    {
        printf("\nAs you approach the door, your heart begins to race at the sight of the massive chain that wraps around it.\n");
        printf("The metal gleams menacingly in the dim light, its presence a clear indication that something on the other side is meant to stay locked away.\n");
        printf("The chain is fastened securely, the padlock clicking shut with a finality that sends a shiver down your spine.\n");
        Continue();
        Action(player);
    }
    else
    {
    room *previous_room = (*player).current_room;
    (*player).current_room = &mirror_room;
    ZombieCheck(player, previous_room);
    if (mirror_room.visited == false)
    {
        printf("\n##################################################\n");
        printf("\nYour heart stops as you take in the sight before you.\n");
        printf("Every wall in the room is lined with mirrors, all of them stained with a deep, rusty red that could only be blood.\n");
        printf("You can't tell if the blood is fresh or from the battle you just fought against the zombies, but you feel a chill run down your spine all the same.\n");
        printf("In the corner, you spot a small crack in one of the mirrors, barely large enough for you to fit through.\n");
        Continue();
    }

    if (mirror_room.visited == false)
    {
        mirror_room.visited = true;
    } 

    Action(player);
    }
}

// Function for the Vestibule - Riddle inside!
void Vestibule(player *player)
{
    room *previous_room = (*player).current_room;
    (*player).current_room = &vestibule;
    ZombieCheck(player, previous_room);
    if (vestibule.visited == false)
    {
        printf("\n##################################################\n");
        printf("\nAs you ascend the ladder, your heart pounding in your chest, you find yourself in some kind of vestibule.\n");
        printf("The space is filled with boxes and old trunks, and a chill runs down your spine as you see the dolls scattered throughout the room.\n");
        printf("Their staring glass eyes seem to follow your every move, and you can't help but feel like they are watching you.\n");
        printf("As you look around the room, your eyes are drawn to a door that is boarded up with wooden planks.\n");
        Continue();
        printf("\nYour heart skips a beat as you see the words 'Don't go inside!' scrawled in bloody letters on the wall next to it.\n");
        printf("The handwriting is jagged and uneven, as if written in a fit of fear or madness.\n");
        printf("You can feel the weight of the warning bearing down on you, urging you to turn back before it's too late.\n");
        Continue();
    }

    if (vestibule.visited == false)
    {
        vestibule.visited = true;
    }
    
    if (vestibule.riddle == true)
    {
        VestibuleRiddle(player, previous_room);
    }

    Action(player);
}

// Function for the Main Attic (Laboratory) - Boss inside!
void MainAttic(player *player)
{
    room *previous_room = (*player).current_room;
    (*player).current_room = &main_attic;
    ZombieCheck(player, previous_room);
    if (main_attic.visited == false)
    {
        printf("\n##################################################\n");
        printf("\nYou finally killed that monster.. it looked like it was pieced together from the remains of multiple corpses, with an unnatural and nightmarish quality to it.\n");
        printf("It's almost as if it wasn't even a normal zombie, but some kind of grotesque twisted amalgamation of body parts.\n");
        printf("Blood and bits of flesh cling to its various features, and it looks like it could come alive again at any moment.\n");
        printf("You can't afford to rest now - you have to get out of this living hell before it's too late.\n");
        Continue();
    }

    if (main_attic.visited == false)
    {
        main_attic.visited = true;
    } 

    Action(player);
}

// Function for the Storage attic
void StorageAttic(player *player)
{
    room *previous_room = (*player).current_room;
    (*player).current_room = &storage_attic;
    ZombieCheck(player, previous_room);
    if ((*player).inventory[1]->picked_up != true)
    {
        if (storage_attic.visited == false)
        {
            printf("\n##################################################\n");
            printf("\nThis eerie storage is littered with debris, making it difficult to navigate.\n");
            printf("A massive safe sits in the corner, its towering presence intimidating and unyielding.\n");
            printf("The lock on its door is imposing and unbreakable, leaving you to wonder what secrets it may hold.\n");
            printf("Uncovering the contents of the safe might help you finding a way out, but without a key there is no chance.\n");
            Continue();

            storage_attic.visited = true;
            Action(player);
        }
        else
        {
            printf("\nCome back when you've found a key for the safe.\n");
            Continue();
            (*player).current_room = previous_room;
            (*player).current_room->func(player);
            Action(player);
        }
    }
    else
    {
        printf("\nWith the lion key tightly clenched between your fingers, you slowly and carefully approach the safe.\n");
        printf("You close your eyes, take a deep breath, and insert the key into the lock.\n");
        Continue();
        (*player).inventory[1]->amount ++;
        (*player).inventory[1]->picked_up = true;
        printf("You have found the Lion Key!\n");
        Continue();
    }
    Action(player);
}

// Function for the Walk-in closet
void WalkInCloset(player *player)
{
    room *previous_room = (*player).current_room;
    (*player).current_room = &walk_in_closet;
    ZombieCheck(player, previous_room);
    if (walk_in_closet.visited == false)
    {
        printf("\n##################################################\n");
        printf("\nThe zombie attack was too close for comfort. Its rancid breath and twisted snarls still ringing in your ears.\n");
        printf("As you catch your breath, you take a look around the cluttered walk-in closet.\n");
        printf("The air is thick with the musty smell of decaying fabric and forgotten memories.\n");
        printf("The old clothes are tattered and torn, as if a wild animal had ripped through them in a frenzied fit of rage.\n");
        Continue();
        printf("\nBut what draws your eye the most is the ladder leading up to the attic.\n");
        Continue();
    }

    if (walk_in_closet.visited == false)
    {
        walk_in_closet.visited = true;
    } 

    Action(player);
}

// Function for the master bedroom
void Bedroom(player *player)
{
    if ((*player).inventory[2]->picked_up != true)
    {
        printf("\nAs you approach the door, your eyes are immediately drawn to the snarling tiger that adorns its surface.\n");
        printf("Despite your best efforts, the door refuses to budge, its lock clicking firmly into place.\n");
        printf("What secrets is the door keeping locked away, and what dangers might lie beyond?\n");
        Continue();
        Action(player);
    }
    else
    {
        room *previous_room = (*player).current_room;
        (*player).current_room = &bedroom;
        ZombieCheck(player, previous_room);
        if (bedroom.visited == false)
        {
            printf("\n##################################################\n");
            printf("\nAs you finally turn the tiger key and enter the master bedroom, your heart sinks with terror.\n");
            printf("The once grand and luxurious space is now marred by a brutal scene.\n");
            printf("A torn and mutilated body lies shattered on the bed, its limbs scattered throughout the room.\n");
            Continue();
            printf("\nIn the corner, a gaping hole in the wall beckons you to venture further into the unknown,\n");
            printf("while a half-open door to a walk-in wardrobe taunts you with whispers of secrets and danger lurking within.\n");
            printf("You can't shake the feeling that you are being watched, and you begin to question if you should even be here at all.\n");
            Continue();
        }

        if (bedroom.visited == false)
        {
            bedroom.visited = true;
        } 

        Action(player);
    }
}

// Function for the guest room - Riddle inside!
void GuestRoom(player *player)
{
    if ((*player).current_room == &interior_balcony)
    {
        printf("\nYour eyes are drawn to the smears of dried blood that cover the surface of the door, a warning of the violence that's taken place within.\n");
        printf("The door itself is completely blocked, a barricade of garbage and debris piling high, effectively sealing you off from what lies beyond.\n");
        printf("The thought of what you might find if you could only get inside sends shivers down your spine.\n");
        Continue();
        Action(player);
    }
    room *previous_room = (*player).current_room;
    (*player).current_room = &guest_room;
    ZombieCheck(player, previous_room);
    if (guest_room.visited == false)
    {
        printf("\n##################################################\n");
        printf("\nAs you cautiously squeezed through the narrow gap in the wall of the master bedroom, you found yourself in the musty guest room.\n");
        printf("The eerie stillness was shattered by the sight of a torn and tattered dead body, laying on the bed with its limbs splayed out in a macabre display.\n");
        printf("Your gaze was drawn to a massive metal closet in the corner, with an ominous lock that seemed to defy explanation.\n");
        Continue();
        printf("\nThe door was adorned with an intricate puzzle, reminiscent of a classic game of moving tiles to create a picture.\n");
        printf("With a sense of dread, you wondered what secrets lay hidden within the depths of the strange metal closet.\n");
        printf("There is an inscription beside it:\n");
        Continue();
    }

    if (guest_room.visited == false)
    {
        guest_room.visited = true;
    } 

    if (guest_room.riddle == true)
    {
        GuestRoomRiddle(player, previous_room);
    }
    Action(player);
}

// Function for the solar
void Solar(player *player)
{
    if ((*player).inventory[2]->picked_up != true)
    {
        printf("\nAs you stand before the door, your eyes are immediately drawn to the snarling tiger that adorns its surface.\n");
        printf("Despite your best efforts, the door remains firmly locked, a barrier between you and whatever lies beyond.\n");
        printf("Yet, as you stand there in the stillness, you can't help but feel that something is lurking just on the other side of the door.\n");
        Continue();
        Action(player);
    }
    else
    {
    room *previous_room = (*player).current_room;
    (*player).current_room = &solar;
    ZombieCheck(player, previous_room);
    if (solar.visited == false)
    {
        printf("\n##################################################\n");
        printf("\nAfter dispatching the two zombies, you survey the massive solar, its ancient facade appearing to have stood for centuries.\n");
        printf("This once-grand solar, a symbol of luxury and exclusivity in castles of yesteryear, now looms ominously before you.\n");
        printf("Its age-worn stones seeming to conceal dark and sinister secrets from a bygone era.\n");
        printf("The musty air and palpable sense of neglect within the solar only serve to further heighten your unease.\n");
        Continue();
    }

    if (solar.visited == false)
    {
        solar.visited = true;
    } 

    Action(player);
    }
}

// Function for the storeroom
void Storeroom(player *player)
{
    if ((*player).inventory[8]->picked_up != true)
    {
        printf("\n##################################################\n");
        printf("\nAs you step into the storeroom, you are immediately struck by the musty, decaying atmosphere.\n");
        printf("The shelves lining the walls are filled with rusted cans and jars that seem to have been there for years, the labels faded and unreadable.\n");
        printf("A thick layer of dust covers everything, and cobwebs cling to the corners of the room.\n");
        printf("You can hear the sound of scurrying rats and insects, and the smell of rot and decay fills your nostrils.\n");
        Continue();
        printf("\nIn the far corner of the room, you spot an old food elevator.\n");
        printf("You approach it, but as you get closer, you realize that it doesn't seem to be working.\n");
        printf("The door is jammed open, and the machinery is covered in rust.\n");
        printf("A flickering red light catches your eye, and you realize that the consumer unit is missing a fuse.\n");
        Continue();
        printf("\nYou feel a growing sense of unease as you examine the rest of the room.\n");
        printf("You decide to leave the storeroom.\n");
        Continue();
        Action(player);
    }
    else
    {
        room *previous_room = (*player).current_room;
        (*player).current_room = &storeroom;
        ZombieCheck(player, previous_room);
        if (storeroom.visited == false)
        {
            printf("\n##################################################\n");
            printf("\nWith the missing fuse tightly gripped in your hand, you hesitate at the foot of the food elevator.\n");
            printf("The elevator looks old and decrepit, and the machinery groans and creaks as if it hasn't been used in years.\n");
            printf("You imagine what might be waiting for you in the basement, and the hairs on the back of your neck stand up.\n");
            Continue();
            printf("\nDo you take a chance and venture down to the basement, risking what might be waiting for you down there,\n");
            printf("or do you try your luck in the upper areas, hoping that you don't run into anything too dangerous?\n");
            Continue();
        }

        if (storeroom.visited == false)
        {
            storeroom.visited = true;
        } 

        Action(player);
    }
}

// Function for the maze - Riddle and Boss inside!
void Maze(player *player)
{
    room *previous_room = (*player).current_room;
    (*player).current_room = &cy_maze;
    ZombieCheck(player, previous_room);
    if (cy_maze.visited == false)
    {
        printf("\n##################################################\n");
        printf("\nAs the twisted and mutated body of the beast crumples to the ground, a feeling of shock and disbelief washes over you.");
        printf("\nYou had battled this monstrous entity, feared by all who dared to cross its path, and yet, through sheer determination and grit, you have emerged victorious.\n");
        printf("\nThe once powerful and deadly creature lies defeated at your feet, and you stand tall, triumphant in your hard-fought win.\n");
        printf("\nYou know that you have to get out of this place because you won't survive another encounter with one of these creatures.\n");
        Continue();
    }

    if (cy_maze.visited == false)
    {
        cy_maze.visited = true;
    } 

    Action(player);
}

// Function for the little storage of the art room
void LittleStorage(player *player)
{
    room *previous_room = (*player).current_room;
    (*player).current_room = &storage_art;
    ZombieCheck(player, previous_room);
    if (storage_art.visited == false)
    {
        printf("\n##################################################\n");
        printf("\nAs you step into the storage room, you realize that the space is far smaller than you anticipated.\n");
        printf("It's cramped and cluttered, with shelves lining the walls and stacks of boxes blocking your path.\n");
        printf("The musty air is thick with the scent of old paper and oil paints, and the dim light casts eerie shadows across the room.\n");
        printf("The paintings on the walls seem to shift and leer at you, their subjects twisting and contorting into grotesque shapes.\n");
        Continue();
    }

    if (storage_art.visited == false)
    {
        storage_art.visited = true;
    } 

    Action(player);
}

// Function for the secret passage
void SecretPassage(player *player)
{
    room *previous_room = (*player).current_room;
    (*player).current_room = &secret_passage;
    ZombieCheck(player, previous_room);
    if (secret_passage.visited == false)
    {
        printf("\n##################################################\n");
        printf("\nWith your heart pounding in your chest, you mustered all your courage and squeezed through the small opening in the wall, into the cramped and dark crawl space.\n");
        printf("The musty air clings to your skin and the sound of your own breathing echoes in the stillness.\n");
        printf("You pause for a moment, allowing your eyes to adjust to the darkness.\n");
        printf("The space is cramped and cramped, the low ceiling making you feel as though you're trapped in a coffin.\n");
        Continue();
        printf("\nThe walls are slick with dampness and you can feel the weight of the building pressing down upon you.\n");
        printf("You're suddenly struck by the thought that this might not be just another dead end, but a final resting place.\n");
        printf("You shake your head, trying to dispel the thought. You need to keep moving.\n");
        Continue();
    }

    if (secret_passage.visited == false)
    {
        secret_passage.visited = true;
    } 

    Action(player);
}

// Function for the dining room - Riddle inside!
void DiningRoom(player *player)
{
    room *previous_room = (*player).current_room;
    (*player).current_room = &dining_room;
    ZombieCheck(player, previous_room);
    if (dining_room.visited == false)
    {
        printf("\n##################################################\n");
        printf("\nThe heavy door creaks open, revealing a large dining room, with elegant furnishings and chandeliers that hang from the ceiling.\n");
        printf("Once a beacon of opulence, the room is now a macabre display of decadence gone wrong.\n");
        printf("The walls are stained with smears of crimson, the once-gleaming silverware now tarnished and caked with dried blood.\n");
        printf("The once-pristine tablecloth is now torn and covered in rust-colored splotches, and the sweet smell of rot permeates the air.\n");
        Continue();
        printf("\nYou quickly realize that there is no other exit to the room, and a chill runs down your spine as you notice\n");
        printf("that something has been scrawled upon the wall in an eerily familiar substance - it's written in blood.\n");
        Continue();
    }

    if (dining_room.visited == false)
    {
        dining_room.visited = true;
    }

    if (dining_room.riddle == true)
    {
        DiningRoomRiddle(player, previous_room);
    }

    Action(player);
}

// Function for the corridor
void Corridor(player *player)
{
    room *previous_room = (*player).current_room;
    (*player).current_room = &corridor;
    ZombieCheck(player, previous_room);
    if (corridor.visited == false)
    {
        printf("\n##################################################\n");
        printf("\nThe adrenaline from the fight with the zombie slowly subsides, leaving you with a bone-deep terror that threatens to overwhelm you.\n");
        printf("You can still hear the creature's guttural moans echoing in your ears, and you wonder what other horrors might be lurking in the shadows.\n");
        printf("You frantically scan your surroundings, searching for any sign of a way out, but the path ahead is obscured by darkness.\n");
        printf("You have no choice but to keep moving, your heart pounding in your chest as you try to keep your breathing steady.\n");
        Continue();
        printf("\nYou must find a way out, and fast, before it's too late..\n");
        Continue();
    }

    if (corridor.visited == false)
    {
        corridor.visited = true;
    } 

    Action(player);
}

// Function for the kitchen
void Kitchen(player *player)
{
    room *previous_room = (*player).current_room;
    (*player).current_room = &kitchen;
    ZombieCheck(player, previous_room);
    if (kitchen.visited == false)
    {
        printf("\n##################################################\n");
        printf("\nAs you step into the kitchen, the overpowering stench of decay and rot hits you like a physical blow.\n");
        printf("You instinctively cover your nose and mouth, trying to block out the foul odor, but it's no use.\n");
        printf("The smell of rotten flesh is everywhere, cloying and suffocating.\n");
        printf("Your gaze falls on the pots on the stove, and a feeling of dread builds within you.\n");
        Continue();
        printf("\nWhat kind of stew could they be cooking up?\n");
        printf("You take a step closer and peer inside, your eyes widening in horror.\n");
        printf("Amidst the chunks of unidentifiable meat, there it is - a small, gnarled finger.\n");
        printf("You quickly step back, your stomach turning at the thought of what other things might be in the boiling broth.\n");
        Continue();
    }

    if (kitchen.visited == false)
    {
        kitchen.visited = true;
    } 

    Action(player);
}

// Function for the kitchen in the basement
void KitchenBasement(player *player)
{
    room *previous_room = (*player).current_room;
    (*player).current_room = &kitchen_basement;
    ZombieCheck(player, previous_room);
    if (kitchen_basement.visited == false)
    {
        printf("\n##################################################\n");
        printf("\nWith a sense of unease, you set the fuse into the consumer unit, praying that it will work.\n");
        printf("To your relief, the food elevator rumbles to life.\n");
        printf("Though small, you reluctantly step inside, knowing that it's the only way to reach the basement.\n");
        printf("As the elevator descends, the feeling of dread in your gut grows stronger.\n");
        Continue();
        printf("\nThe doors finally grind open, revealing a kitchen that is surprisingly well-maintained.\n");
        printf("The counters are clean and gleaming, the pots and pans neatly stacked.\n");
        printf("But something about the eerie stillness sets you on edge.\n");
        printf("You can't shake the feeling that you're being watched, that you're not alone in this basement.\n");
        Continue();
    }

    if (kitchen_basement.visited == false)
    {
        kitchen_basement.visited = true;
    } 

    Action(player);
}

// Function for the corridor in the basement
void CorridorBasement(player *player)
{
    room *previous_room = (*player).current_room;
    (*player).current_room = &corridor_basement;
    ZombieCheck(player, previous_room);
    if (corridor_basement.visited == false)
    {
        printf("\n##################################################\n");
        printf("\nWith shaking hands, you brace yourself against the wall, gasping for breath after another close call with the undead.\n");
        printf("You close your eyes, trying to calm your racing heart, and for a moment you wonder if this is all just a nightmare.\n");
        printf("But the chill of the damp basement walls and the ache in your muscles tell you otherwise.\n");
        printf("You push yourself away from the wall and cautiously look around.\n");
        Continue();
        printf("\nThe basement is just as decrepit and creepy as the rest of the mansion, but there's a feeling of stillness here that is even more unsettling.\n");
        printf("You can't shake the feeling that you're being watched, that something is lurking in the shadows, waiting to pounce.\n");
        printf("You need to keep moving, to find a way out before it's too late.\n");
        printf("What horrors might you face in the depths of this basement, and will you make it out alive?\n");
        Continue();
    }

    if (corridor_basement.visited == false)
    {
        corridor_basement.visited = true;
    } 

    Action(player);
}

// Function for the main basement - Boss inside
void MainBasement(player *player)
{
    room *previous_room = (*player).current_room;
    (*player).current_room = &main_basement;
    ZombieCheck(player, previous_room);
    if (main_basement.visited == false)
    {
        printf("\n##################################################\n");
        printf("\nYou're paralyzed with fear, your mind struggling to process what you just witnessed.\n");
        printf("The monster you fought was beyond anything you could have imagined, even in your darkest nightmares.\n");
        printf("You stare at the remains of the beast, your body rigid with terror.\n");
        Continue();
        printf("\nAs your gaze drifts away from the grisly scene, you notice the staircase to the first floor, now nothing but a pile of rubble and debris.\n");
        printf("The collapse of the stairs is a sharp reminder that your options for escape are limited.\n");
        printf("You steel yourself for what lies ahead, not knowing if you'll ever see the light of day again.\n");
        printf("You take a deep breath, trying to steady your nerves, and turn your attention to the rest of the room.\n");
        Continue();
    }

    if (main_basement.visited == false)
    {
        main_basement.visited = true;
    } 

    Action(player);
}

// Function for the Laundry room - Riddle inside!
void LaundryRoom(player *player)
{
    if ((*player).inventory[5]->picked_up != true)
    {
        printf("\nAs you approach the door, labeled simply as 'laundry room', the stench of decay and the coppery tang of blood assault your senses.\n");
        printf("Your gaze is drawn to the smears of dried blood that seem to cover every surface.\n");
        printf("The door itself is firmly locked, the lock clicking ominously in the silence, sealing you off from the horrors that lie beyond.\n");
        Continue();
        Action(player);
    }
    else
    {
    room *previous_room = (*player).current_room;
    (*player).current_room = &laundry_room;
    ZombieCheck(player, previous_room);
    if (laundry_room.visited == false)
    {
        printf("\n##################################################\n");
        printf("\nYou push open the creaky door to the massive room, the musty smell of decay and rust assaulting your senses.\n");
        printf("It's clear that this place was once a bustling industrial laundry room, but now everything lies in ruins.\n");
        printf("Dust covers the machines and broken pipes hang from the ceiling like cobwebs.\n");
        Continue();
        printf("\nBut one machine in the corner catches your eye. Its red light blinks ominously in the otherwise dark and silent room.\n");
        printf("With trembling hands, you approach it, trying to make out what it could be.\n");
        printf("The closer you get, the more your sense of unease grows. What secrets does this machine hold?\n");
        Continue();
    }

    if (laundry_room.visited == false)
    {
        laundry_room.visited = true;
    } 

    if (laundry_room.riddle == true)
    {
        LaundryRoomRiddle(player, previous_room);
    }
    Action(player);
    }
}

// Function for the storage chamber first floor
void StorageChamber(player *player)
{
    room *previous_room = (*player).current_room;
    (*player).current_room = &storage_first;
    ZombieCheck(player, previous_room);
    if (storage_first.visited == false)
    {
        printf("\n##################################################\n");
        printf("\nAs you stand victorious over the lifeless, rotting body of the zombie, a cold shiver runs down your spine.\n");
        printf("Despite your triumph, you can't shake the gnawing sensation that your worst fears have not only become a reality,\n");
        printf("but that the truth is even more grotesque than you could have imagined.\n");
        Continue();
        printf("\nThe air in the storage room is thick with the smell of decay, and the flickering light of a lamp casts grotesque shadows on the walls.\n");
        printf("Rows upon rows of shelves line the walls, each one overflowing with supplies.\n");
        printf("The light of the catches on something in the far corner, and your heart almost stops at the sight of it.\n");
        printf("There, in the corner, is a figure shrouded in darkness, its form seemingly shifting and changing as you approach.\n");
        Continue();
        printf("\nYou raise your weapon, ready for whatever horror is about to reveal itself, but as you get closer,\n");
        printf("you realize with a sickening feeling that the figure is not a monster, but something far more sinister.\n");
        printf("It's a pile of discarded body parts, human remains that have been left to rot in this chamber of horrors.\n");
        printf("And as the truth dawns on you, you can't help but wonder if your nightmares are just beginning.\n");
        Continue();
    }

    if (storage_first.visited == false)
    {
        storage_first.visited = true;
    } 

    Action(player);
}

// Function before the final fight after the maze
void MazeFinalFight(void)
{
    printf("\nAs you sprint through the twisting passages of the maze, the sound of the beast's");
    printf("\nsnarling breaths echoing hot on your heels, you suddenly spot a glimmer of light ahead.");
    printf("\nPanting and gasping for breath, you pour on the speed, driven by a desperate hope.\n");
    Continue();
    printf("\nBreaking through into a seemingly idyllic garden, your hopes are quickly dashed as you realize with a sinking feeling that you've reached another dead end.");
    printf("\nTowering walls loom high on every side, trapping you within the garden with no escape.");
    printf("\nAs you spin around in terror, you see the beast looming closer, its eyes blazing with a savage hunger.\n");
    Continue();
    printf("\nYou know in that moment that there is only one way out.");
    printf("\nYou must face the beast head on, steeling yourself for the fight to the death that lies ahead.");
    printf("\nThe stench of the creature's hot breath washes over you as you prepare to do battle, your heart pounding in your chest and your every muscle tensed and ready.");
    printf("\nThis is it. There is no turning back. You must fight or die trying.\n");
    Continue();
}

// Function when entering the maze
void MazeStory(void)
{
    printf("\nAs you cautiously approached the entrance of the maze, you couldn't help but feel a growing sense of unease.");
    printf("\nSuddenly, the tranquil atmosphere was shattered as a grotesque, mutated beast burst through the crumbling walls of the mansion, its beady eyes fixated on you.");
    printf("\nYou felt your heart race as it sprinted towards you, its massive form filling your field of vision.\n");
    Continue();
    printf("\nYou turned on your heels and darted into the maze, not daring to look back.");
    printf("\nYou felt the breath of the monster on the back of your neck as you ran blindly, desperation driving you forward.");
    printf("\nEvery step felt like you were running through quicksand, the beast getting closer and closer with each passing moment.");
    printf("\nYour heart was pounding so loud you couldn't hear anything else.\n");
    Continue();
    printf("\nAs you ran through the labyrinthine passages, you realized that this was a deadly game of cat and mouse.");
    printf("\nYou had to find a way out before the beast caught up to you, or face certain death.");
    printf("\nYou pushed yourself harder, running as fast as you could through the twists and turns, praying that you wouldn't stumble or take a wrong turn.\n");
    Continue();
    printf("\nType the number of the path you want to escape to.\n");
}

// Function for the Boss fight in the attic
void BossAttic(player *player, room *previous_room)
{
    printf("\nLast chance. Do you really want to enter?\n");
    printf("Be ready for whatever nightmare lies beyond this door.\n");
    printf("\n## Type '0' to go back or '1' to proceed: ");
    int input;
    while (input != 0 || input != 1)
    {
        if (input == 1)
        {
            break;
        }
        else if (input == 0)
        {
            previous_room->func(player);
        }
        scanf("%d", &input);
    }
    int boss_attic_life = 200;
    int boss_attic_life_display = 200;
    bool boss_attic_alive = true;

    printf("\nAs you cautiously push open the creaky wooden door, a shiver runs down your spine.");
    printf("\nThe room you've entered looks like a mad scientist's laboratory, complete with bubbling beakers and strange instruments scattered about.");
    printf("\nYour attention is drawn to a massive container in the center of the room.\n");
    Continue();
    printf("\nAs you approach the container, you can hear a pounding noise coming from within.");
    printf("\nThe hairs on the back of your neck stand on end as you suddenly realize what's making the noise.");
    printf("\nWith a deafening roar, a monster emerges from the container.\n");
    Continue();
    printf("\nIt's unlike anything you've ever seen before.");
    printf("\nThe beast towers over you, twice the height of a man and with an array of twisted, gnarled limbs.");
    printf("\nIts skin is an sickly green hue, slick with some sort of slimy substance.");
    printf("\nDespite your fear, you steel yourself for the fight ahead.\n");
    Continue();
    printf("\nYou quickly assess the monster's anatomy, looking for any weaknesses you can exploit.");
    printf("\nYour eyes settle on a large, glistening green plate located on the creature's chest.");
    printf("\nIt appears to be the source of the monster's power, and possibly its only weakness.");
    printf("\nIt's now up to you to overcome the monster and destroy this evil being once and for all.\n");
    Continue();
    printf("\n################## BOSS FIGHT #######################\n");
    Continue();
    printf("\nINSTRUCTIONS:\n\n");
    printf("You'll see the monster and have to shoot at his weak point,\n");
    printf("highlighted with six green X's. As soon as the red line\n");
    printf("loading from the left aligns with the weak point, press any key.\n");
    printf("Attention! You have to be very quick!\n");
    Continue();
    printf("\nWhen the monster attacks, you have to dodge.\n");
    printf("This time, you see four red X's. They can appear on different spots of the last line.\n");
    printf("Again, you have to align the red line with the row of X's to dodge.\n");

    while(boss_attic_alive)
    {
        BossFight(player, &boss_attic_life, &boss_attic_alive, boss_attic_life_display);
    }

    printf("\n################## YOU WON! #######################\n");
    ContinueOne();
}

// Function for the boss fight in the basement
void BossBasement(player *player, room *previous_room)
{
    printf("\nLast chance. Do you really want to enter?\n");
    printf("Be ready for whatever nightmare lies beyond this door.\n");
    printf("\n## Type '0' to go back or '1' to proceed: ");
    int input;
    while (input != 0 || input != 1)
    {
        if (input == 1)
        {
            break;
        }
        else if (input == 0)
        {
            previous_room->func(player);
        }
        scanf("%d", &input);
    }
    int boss_basement_life = 300;
    int boss_basement_life_display = 300;
    bool boss_basement_alive = true;

    printf("\nYou cautiously make your way through the door, the darkness shrouding your every step.");
    printf("\nThe basement is in shambles, as if a beast had wreaked havoc through the room.");
    printf("\nDebris is scattered everywhere, broken glass and splintered wood crunching underfoot.\n");
    Continue();
    printf("\nSuddenly, your eyes catch a glint of metal amidst the wreckage.");
    printf("\nCuriosity piqued, you cautiously make your way towards the source of the shine,");
    printf("\nonly to find a large, ancient-looking key lying amidst the chaos.\n");
    Continue();
    printf("\nAs you reach out to pick up the key, the debris around you begins to stir and shake,");
    printf("\nas if something were stirring beneath the rubble.");
    printf("\nSuddenly, with a deafening roar, a monster rises from the wreckage.");
    printf("\nIts eyes blazing, its fangs bared, it towers over you, a beast like no other.\n");
    Continue();
    printf("\nThis monster is not the one from the laboratory, no,");
    printf("\nthis beast is far larger and far more sinister.");
    printf("\nIts fur is matted, its skin torn and scarred.");
    printf("\nIt looks like it has been through a war, and it is now prepared to wage another.\n");
    Continue();
    printf("\n################## BOSS FIGHT #######################\n");
    printf("\n");
    Continue();
    printf("\nINSTRUCTIONS:\n\n");
    printf("You'll see the monster and have to shoot at his weak point,\n");
    printf("highlighted with six green X's. As soon as the red line\n");
    printf("loading from the left aligns with the weak point, press any key.\n");
    printf("Attention! You have to be very quick!\n");
    Continue();
    printf("\nWhen the monster attacks, you have to dodge.\n");
    printf("This time, you see four red X's. They can appear on different spots of the last line.\n");
    printf("Again, you have to align the red line with the row of X's to dodge.\n");

    while(boss_basement_alive)
    {
        BossFight(player, &boss_basement_life, &boss_basement_alive, boss_basement_life_display);
    }

    printf("\n################## YOU WON! #######################\n");
    ContinueOne();
}

// Function for the end of the game
void End(void)
{
    printf("\nAs you return to the main entrance, the ominous aura of the mansion looms over you.\n");
    printf("Your hand trembles as you reach for the emblem, the twisted symbol of your harrowing journey.\n");
    printf("You place it into the door's socket, a grotesquely carved indentation shaped like a snarling beast.\n");
    printf("The sound of grinding gears and clanging metal fills the air as the door slowly unlocks with a resounding thud.\n");
    Continue();
    printf("\nYou push it open, heart pounding with a mixture of triumph and fear..\n");
    Continue();
    printf("\nBut as you step out into the open air, you're met with a sobering realization.\n");
    printf("The world outside the mansion is just as dangerous as the horrors you faced within its walls.\n");
    printf("The zombie outbreak has ravaged the once-thriving cities and towns, leaving a desolate wasteland in its wake.\n");
    printf("You're free from the mansion, but now you must face an even greater challenge:\n");
    Continue();
    printf("\nwhere to go next..");
    printf("\nand how to survive in a world overrun by the undead..\n");
    printf("\n#################### THE END ####################\n");
    Continue();
    exit(0);
}


// ##################################################
// CREATE MANSION
// ##################################################

// Create all rooms, name them, fill them with items, weapons, zombies and possible pathes
room CreateMansion(void)
{
    int counter_zombies = 0;
    int counter = 0;

    // Main Entrance
    main_entrance.name = "main entrance";
    main_entrance.floor_name = "1st floor";
    main_entrance.visited = false;
    main_entrance.func = &MainEntrance;

    // 1. Attic
    attic[0] = vestibule;
    vestibule.name = "vestibule";
    vestibule.floor_name = "attic";
    vestibule.zombie_amount = 0;
    vestibule.item_amount = 0;
    vestibule.weapon_amount = 0;
    vestibule.func = &Vestibule;
    vestibule.riddle = true;
    vestibule.next_room[0] = &main_attic;
    vestibule.next_room[1] = &walk_in_closet;
    for (int i = 2; i < next_array_size; i++)
    {
        vestibule.next_room[i] = NULL;
    }
    for (int i = 0; i < items_array_size; i++)
    {
        vestibule.items[i] = NULL;
    }
    for (int i = 0; i < weapon_array_size; i++)
    {
        vestibule.weapons[i] = NULL;
    }
    for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < vestibule.zombie_amount)
        {
            vestibule.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            vestibule.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;

    // 2. Attic
    attic[1] = main_attic;
    main_attic.name = "laboratory";
    main_attic.floor_name = "attic";
    main_attic.zombie_amount = 0;
    main_attic.item_amount = 0;
    main_attic.weapon_amount = 0;
    main_attic.func = &MainAttic;
    main_attic.riddle = false;
    main_attic.next_room[0] = &vestibule;
    main_attic.next_room[1] = &storage_attic;
    for (int i = 2; i < next_array_size; i++)
    {
        main_attic.next_room[i] = NULL;
    }
    for (int i = 0; i < items_array_size; i++)
    {
        main_attic.items[i] = NULL;
    }
    for (int i = 0; i < weapon_array_size; i++)
    {
        main_attic.weapons[i] = NULL;
    }
    for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < main_attic.zombie_amount)
        {
            main_attic.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            main_attic.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;

    // 3. Attic
    attic[2] = storage_attic;
    storage_attic.name = "attic storage room";
    storage_attic.floor_name = "attic";
    storage_attic.zombie_amount = 0;
    storage_attic.item_amount = 2;
    storage_attic.weapon_amount = 1;
    storage_attic.func = &StorageAttic;
    storage_attic.riddle = false;
    storage_attic.weapons[0] = &pistol;
    storage_attic.items[0] = &herbs;
    storage_attic.items[1] = &key_art;
    storage_attic.next_room[0] = &main_attic;
    for (int i = 1; i < next_array_size; i++)
    {
        storage_attic.next_room[i] = NULL;
    }
    for (int i = 2; i < items_array_size; i++)
    {
        storage_attic.items[i] = NULL;
    }
    for (int i = 1; i < weapon_array_size; i++)
    {
        storage_attic.weapons[i] = NULL;
    }
    for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < storage_attic.zombie_amount)
        {
            storage_attic.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            storage_attic.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;

    // set all "visited" to false and add zombies to room
    for (int i = 0; i < attic_size; i++)
    {
        attic[i].visited = false;
        attic[i].searched = 0;
    }

    // 1. 2nd Floor
    second_floor[0] = walk_in_closet;
    walk_in_closet.name = "walk-in closet";
    walk_in_closet.floor_name = "2nd floor";
    walk_in_closet.zombie_amount = 1;
    walk_in_closet.item_amount = 0;
    walk_in_closet.weapon_amount = 0;
    walk_in_closet.func = &WalkInCloset;
    walk_in_closet.riddle = false;
    walk_in_closet.next_room[0] = &vestibule;
    walk_in_closet.next_room[1] = &bedroom;
    for (int i = 2; i < next_array_size; i++)
    {
        walk_in_closet.next_room[i] = NULL;
    }
    for (int i = 0; i < items_array_size; i++)
    {
        walk_in_closet.items[i] = NULL;
    }
    for (int i = 0; i < weapon_array_size; i++)
    {
        walk_in_closet.weapons[i] = NULL;
    }
        for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < walk_in_closet.zombie_amount)
        {
            walk_in_closet.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            walk_in_closet.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;

    // 2. 2nd Floor
    second_floor[1] = bedroom;
    bedroom.name = "master bedroom";
    bedroom.floor_name = "2nd floor";
    bedroom.zombie_amount = 0;
    bedroom.item_amount = 0;
    bedroom.weapon_amount = 0;
    bedroom.func = &Bedroom;
    bedroom.riddle = false;
    bedroom.next_room[0] = &interior_balcony;
    bedroom.next_room[1] = &walk_in_closet;
    bedroom.next_room[2] = &guest_room;
    for (int i = 3; i < next_array_size; i++)
    {
        bedroom.next_room[i] = NULL;
    }
    for (int i = 0; i < items_array_size; i++)
    {
        bedroom.items[i] = NULL;
    }
    for (int i = 0; i < weapon_array_size; i++)
    {
        bedroom.weapons[i] = NULL;
    }
    for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < bedroom.zombie_amount)
        {
            bedroom.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            bedroom.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;

    // 3. 2nd Floor
    second_floor[2] = guest_room;
    guest_room.name = "guest room";
    guest_room.floor_name = "2nd floor";
    guest_room.zombie_amount = 0;
    guest_room.item_amount = 1;
    guest_room.weapon_amount = 3;
    guest_room.func = &GuestRoom;
    guest_room.riddle = true;
    guest_room.weapons[0] = &pistol;
    guest_room.weapons[1] = &shotgun;
    guest_room.weapons[2] = &shotgun;
    guest_room.items[0] = &herbs;
    guest_room.next_room[0] = &bedroom;
    for (int i = 1; i < next_array_size; i++)
    {
        guest_room.next_room[i] = NULL;
    }
    for (int i = 1; i < items_array_size; i++)
    {
        guest_room.items[i] = NULL;
    }
    for (int i = 3; i < weapon_array_size; i++)
    {
        guest_room.weapons[i] = NULL;
    }
    for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < guest_room.zombie_amount)
        {
            guest_room.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            guest_room.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;
    
    // 4. 2nd Floor
    second_floor[3] = interior_balcony;
    interior_balcony.name = "interior balcony";
    interior_balcony.floor_name = "2nd floor";
    interior_balcony.zombie_amount = 0;
    interior_balcony.item_amount = 1;
    interior_balcony.weapon_amount = 0;
    interior_balcony.func = &InteriorBalcony;
    interior_balcony.riddle = false;
    interior_balcony.items[0] = &herbs;
    interior_balcony.next_room[0] = &bedroom;
    interior_balcony.next_room[1] = &guest_room;
    interior_balcony.next_room[2] = &solar;
    interior_balcony.next_room[3] = &main_hall;
    for (int i = 4; i < next_array_size; i++)
    {
        interior_balcony.next_room[i] = NULL;
    }
    for (int i = 1; i < items_array_size; i++)
    {
        interior_balcony.items[i] = NULL;
    }
    for (int i = 0; i < weapon_array_size; i++)
    {
        interior_balcony.weapons[i] = NULL;
    }
    for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < interior_balcony.zombie_amount)
        {
            interior_balcony.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            interior_balcony.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;
    
    // 5. 2nd Floor
    second_floor[4] = solar;
    solar.name = "solar";
    solar.floor_name = "2nd floor";
    solar.zombie_amount = 2;
    solar.item_amount = 0;
    solar.weapon_amount = 0;
    solar.func = &Solar;
    solar.riddle = false;
    solar.next_room[0] = &interior_balcony;
    solar.next_room[1] = &storeroom;
    for (int i = 2; i < next_array_size; i++)
    {
        solar.next_room[i] = NULL;
    }
    for (int i = 0; i < items_array_size; i++)
    {
        solar.items[i] = NULL;
    }
    for (int i = 0; i < weapon_array_size; i++)
    {
        solar.weapons[i] = NULL;
    }
    for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < solar.zombie_amount)
        {
            solar.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            solar.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;

    // 6. 2nd Floor
    second_floor[5] = storeroom;
    storeroom.name = "storeroom";
    storeroom.floor_name = "2nd floor";
    storeroom.zombie_amount = 0;
    storeroom.item_amount = 1;
    storeroom.weapon_amount = 1;
    storeroom.func = &Storeroom;
    storeroom.riddle = false;
    storeroom.weapons[0] = &pistol;
    storeroom.items[0] = &cutter;
    storeroom.next_room[0] = &solar;
    storeroom.next_room[1] = &kitchen_basement;
    for (int i = 2; i < next_array_size; i++)
    {
        storeroom.next_room[i] = NULL;
    }
    for (int i = 1; i < items_array_size; i++)
    {
        storeroom.items[i] = NULL;
    }
    for (int i = 1; i < weapon_array_size; i++)
    {
        storeroom.weapons[i] = NULL;
    }
    for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < storeroom.zombie_amount)
        {
            storeroom.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            storeroom.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;

    // set all "visited" to false and add zombies to room
    for (int i = 0; i < second_size; i++)
    {
        second_floor[i].visited = false;
        second_floor[i].searched = 0;
    }

    // 1. Courtyard
    outside[0] = cy_entrance;
    cy_entrance.name = "courtyard";
    cy_entrance.floor_name = "outside";
    cy_entrance.zombie_amount = 0;
    cy_entrance.item_amount = 0;
    cy_entrance.weapon_amount = 1;
    cy_entrance.func = &Courtyard;
    cy_entrance.riddle = false;
    cy_entrance.weapons[0] = &shotgun;
    cy_entrance.next_room[0] = &main_hall;
    cy_entrance.next_room[1] = &cy_maze;
    for (int i = 2; i < next_array_size; i++)
    {
        cy_entrance.next_room[i] = NULL;
    }
    for (int i = 0; i < items_array_size; i++)
    {
        cy_entrance.items[i] = NULL;
    }
    for (int i = 1; i < weapon_array_size; i++)
    {
        cy_entrance.weapons[i] = NULL;
    }
    for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < cy_entrance.zombie_amount)
        {
            cy_entrance.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            cy_entrance.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;
    
    // 2. Courtyard
    outside[1] = cy_maze;
    cy_maze.name = "maze";
    cy_maze.floor_name = "outside";
    cy_maze.zombie_amount = 0;
    cy_maze.item_amount = 1;
    cy_maze.weapon_amount = 0;
    cy_maze.func = &Maze;
    cy_maze.riddle = false;
    cy_maze.items[0] = &emblem;
    cy_maze.next_room[0] = &cy_entrance;
    for (int i = 1; i < next_array_size; i++)
    {
        cy_maze.next_room[i] = NULL;
    }
    for (int i = 1; i < items_array_size; i++)
    {
        cy_maze.items[i] = NULL;
    }
    for (int i = 0; i < weapon_array_size; i++)
    {
        cy_maze.weapons[i] = NULL;
    }
    for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < cy_maze.zombie_amount)
        {
            cy_maze.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            cy_maze.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;    

    // set all "visited" to false and add zombies to room
    for (int i = 0; i < outside_size; i++)
    {
        outside[i].visited = false;
        outside[i].searched = 0;
    }
    
    // 1. 1st Floor
    first_floor[0] = main_hall;
    main_hall.name = "main hall";
    main_hall.floor_name = "1st floor";
    main_hall.zombie_amount = 0;
    main_hall.item_amount = 0;
    main_hall.weapon_amount = 0;
    main_hall.func = &MainHall;
    main_hall.riddle = false;
    main_hall.next_room[0] = &art_room;
    main_hall.next_room[1] = &mirror_room;
    main_hall.next_room[2] = &interior_balcony;
    main_hall.next_room[3] = &cy_entrance;
    main_hall.next_room[4] = &dining_room;
    main_hall.next_room[5] = &main_entrance;
    for (int i = 6; i < next_array_size; i++)
    {
        main_hall.next_room[i] = NULL;
    }
    for (int i = 0; i < items_array_size; i++)
    {
        main_hall.items[i] = NULL;
    }
    for (int i = 0; i < weapon_array_size; i++)
    {
        main_hall.weapons[i] = NULL;
    }
    for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < main_hall.zombie_amount)
        {
            main_hall.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            main_hall.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;    
    
    // 2. 1st Floor
    first_floor[1] = art_room;
    art_room.name = "art room";
    art_room.floor_name = "1st floor";
    art_room.zombie_amount = 2;
    art_room.item_amount = 0;
    art_room.weapon_amount = 0;
    art_room.func = &ArtRoom;
    art_room.riddle = false;
    art_room.next_room[0] = &storage_art;
    art_room.next_room[1] = &main_hall;
    for (int i = 2; i < next_array_size; i++)
    {
        art_room.next_room[i] = NULL;
    }
    for (int i = 0; i < items_array_size; i++)
    {
        art_room.items[i] = NULL;
    }
    for (int i = 0; i < weapon_array_size; i++)
    {
        art_room.weapons[i] = NULL;
    }
    for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < art_room.zombie_amount)
        {
            art_room.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            art_room.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;  
    
    // 3. 1st Floor
    first_floor[2] = storage_art;
    storage_art.name = "a little storage room";
    storage_art.floor_name = "1st floor";
    storage_art.zombie_amount = 0;
    storage_art.item_amount = 1;
    storage_art.weapon_amount = 1;
    storage_art.func = &LittleStorage;
    storage_art.riddle = false;
    storage_art.weapons[0] = &pistol;
    storage_art.items[0] = &fuse;
    storage_art.next_room[0] = &art_room;
    for (int i = 1; i < next_array_size; i++)
    {
        storage_art.next_room[i] = NULL;
    }
    for (int i = 1; i < items_array_size; i++)
    {
        storage_art.items[i] = NULL;
    }
    for (int i = 1; i < weapon_array_size; i++)
    {
        storage_art.weapons[i] = NULL;
    }
    for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < storage_art.zombie_amount)
        {
            storage_art.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            storage_art.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;  
    
    // 4. 1st Floor
    first_floor[3] = mirror_room;
    mirror_room.name = "mirror room";
    mirror_room.floor_name = "1st floor";
    mirror_room.zombie_amount = 2;
    mirror_room.item_amount = 0;
    mirror_room.weapon_amount = 0;
    mirror_room.func = &MirrorRoom;
    mirror_room.riddle = false;
    mirror_room.next_room[0] = &main_hall;
    mirror_room.next_room[1] = &secret_passage;
    for (int i = 2; i < next_array_size; i++)
    {
        mirror_room.next_room[i] = NULL;
    }
    for (int i = 0; i < items_array_size; i++)
    {
        mirror_room.items[i] = NULL;
    }
    for (int i = 0; i < weapon_array_size; i++)
    {
        mirror_room.weapons[i] = NULL;
    }
    for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < mirror_room.zombie_amount)
        {
            mirror_room.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            mirror_room.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;  
    
    // 5. 1st Floor
    first_floor[4] = secret_passage;
    secret_passage.name = "secret passage";
    secret_passage.floor_name = "1st floor";
    secret_passage.zombie_amount = 0;
    secret_passage.item_amount = 2;
    secret_passage.weapon_amount = 1;
    secret_passage.func = &SecretPassage;
    secret_passage.riddle = false;
    secret_passage.weapons[0] = &pistol;
    secret_passage.items[0] = &key_safe;
    secret_passage.items[1] = &herbs;
    secret_passage.next_room[0] = &mirror_room;
    for (int i = 1; i < next_array_size; i++)
    {
        secret_passage.next_room[i] = NULL;
    }
    for (int i = 2; i < items_array_size; i++)
    {
        secret_passage.items[i] = NULL;
    }
    for (int i = 1; i < weapon_array_size; i++)
    {
        secret_passage.weapons[i] = NULL;
    }
    for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < secret_passage.zombie_amount)
        {
            secret_passage.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            secret_passage.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;  
    
    // 6. 1st Floor
    first_floor[5] = dining_room;
    dining_room.name = "dining room";
    dining_room.floor_name = "1st floor";
    dining_room.zombie_amount = 0;
    dining_room.item_amount = 0;
    dining_room.weapon_amount = 1;
    dining_room.func = &DiningRoom;
    dining_room.riddle = true;
    dining_room.weapons[0] = &pistol;
    dining_room.next_room[0] = &main_hall;
    dining_room.next_room[1] = &corridor;
    for (int i = 2; i < next_array_size; i++)
    {
        dining_room.next_room[i] = NULL;
    }
    for (int i = 0; i < items_array_size; i++)
    {
        dining_room.items[i] = NULL;
    }
    for (int i = 1; i < weapon_array_size; i++)
    {
        dining_room.weapons[i] = NULL;
    }
    for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < dining_room.zombie_amount)
        {
            dining_room.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            dining_room.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;  

    // 7. 1st Floor
    first_floor[6] = corridor;
    corridor.name = "corridor";
    corridor.floor_name = "1st floor";
    corridor.zombie_amount = 1;
    corridor.item_amount = 0;
    corridor.weapon_amount = 0;
    corridor.func = &Corridor;
    corridor.riddle = false;
    corridor.next_room[0] = &kitchen;
    corridor.next_room[1] = &dining_room;
    for (int i = 2; i < next_array_size; i++)
    {
        corridor.next_room[i] = NULL;
    }
    for (int i = 0; i < items_array_size; i++)
    {
        corridor.items[i] = NULL;
    }
    for (int i = 0; i < weapon_array_size; i++)
    {
        corridor.weapons[i] = NULL;
    }
    for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < corridor.zombie_amount)
        {
            corridor.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            corridor.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;  
    
    // 8. 1st Floor
    first_floor[7] = kitchen;
    kitchen.name = "kitchen";
    kitchen.floor_name = "1st floor";
    kitchen.zombie_amount = 0;
    kitchen.item_amount = 0;
    kitchen.weapon_amount = 1;
    kitchen.func = &Kitchen;
    kitchen.riddle = false;
    kitchen.weapons[0] = &pistol;
    kitchen.next_room[0] = &storage_first;
    kitchen.next_room[1] = &corridor;
    for (int i = 2; i < next_array_size; i++)
    {
        kitchen.next_room[i] = NULL;
    }
    for (int i = 0; i < items_array_size; i++)
    {
        kitchen.items[i] = NULL;
    }
    for (int i = 1; i < weapon_array_size; i++)
    {
        kitchen.weapons[i] = NULL;
    }
    for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < kitchen.zombie_amount)
        {
            kitchen.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            kitchen.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;    

    // 9. 1st Floor
    first_floor[8] = storage_first;
    storage_first.name = "storage chamber";
    storage_first.floor_name = "1st floor";
    storage_first.zombie_amount = 1;
    storage_first.item_amount = 2;
    storage_first.weapon_amount = 0;
    storage_first.func = &StorageChamber;
    storage_first.riddle = false;
    storage_first.items[0] = &herbs;
    storage_first.items[1] = &key_second;
    storage_first.next_room[0] = &kitchen;
    for (int i = 1; i < next_array_size; i++)
    {
        storage_first.next_room[i] = NULL;
    }
    for (int i = 2; i < items_array_size; i++)
    {
        storage_first.items[i] = NULL;
    }
    for (int i = 0; i < weapon_array_size; i++)
    {
        storage_first.weapons[i] = NULL;
    }
    for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < storage_first.zombie_amount)
        {
            storage_first.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            storage_first.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;    

    // set all "visited" to false and add zombies to room
    for (int i = 0; i < first_size; i++)
    {
        first_floor[i].visited = false;
        first_floor[i].searched = 0;
    }

    // 1. Basement
    basement[0] = kitchen_basement;
    kitchen_basement.name = "kitchen";
    kitchen_basement.floor_name = "basement";
    kitchen_basement.zombie_amount = 0;
    kitchen_basement.item_amount = 1;
    kitchen_basement.weapon_amount = 0;
    kitchen_basement.func = &KitchenBasement;
    kitchen_basement.riddle = false;
    kitchen_basement.items[0] = &herbs;
    kitchen_basement.next_room[0] = &storeroom;
    kitchen_basement.next_room[1] = &corridor_basement;
    for (int i = 2; i < next_array_size; i++)
    {
        kitchen_basement.next_room[i] = NULL;
    }
    for (int i = 1; i < items_array_size; i++)
    {
        kitchen_basement.items[i] = NULL;
    }
    for (int i = 0; i < weapon_array_size; i++)
    {
        kitchen_basement.weapons[i] = NULL;
    }
    for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < kitchen_basement.zombie_amount)
        {
            kitchen_basement.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            kitchen_basement.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;
    
    // 2. Basement
    basement[1] = corridor_basement;
    corridor_basement.name = "corridor";
    corridor_basement.floor_name = "basement";
    corridor_basement.zombie_amount = 1;
    corridor_basement.item_amount = 0;
    corridor_basement.weapon_amount = 1;
    corridor_basement.func = &CorridorBasement;
    corridor_basement.riddle = false;
    corridor_basement.weapons[0] = &shotgun;
    corridor_basement.next_room[0] = &kitchen_basement;
    corridor_basement.next_room[1] = &main_basement;
    corridor_basement.next_room[2] = &laundry_room;
    for (int i = 3; i < next_array_size; i++)
    {
        corridor_basement.next_room[i] = NULL;
    }
    for (int i = 0; i < items_array_size; i++)
    {
        corridor_basement.items[i] = NULL;
    }
    for (int i = 1; i < weapon_array_size; i++)
    {
        corridor_basement.weapons[i] = NULL;
    }
    for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < corridor_basement.zombie_amount)
        {
            corridor_basement.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            corridor_basement.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;
    
    // 3. Basement
    basement[2] = main_basement;
    main_basement.name = "staircase hall";
    main_basement.floor_name = "basement";
    main_basement.zombie_amount = 0;
    main_basement.item_amount = 1;
    main_basement.weapon_amount = 0;
    main_basement.func = &MainBasement;
    main_basement.riddle = false;
    main_basement.items[0] = &key_laundry;
    main_basement.next_room[0] = &corridor_basement;
    for (int i = 1; i < next_array_size; i++)
    {
        main_basement.next_room[i] = NULL;
    }
    for (int i = 1; i < items_array_size; i++)
    {
        main_basement.items[i] = NULL;
    }
    for (int i = 0; i < weapon_array_size; i++)
    {
        main_basement.weapons[i] = NULL;
    }
    for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < main_basement.zombie_amount)
        {
            main_basement.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            main_basement.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;
    
    // 4. Basement
    basement[3] = laundry_room;
    laundry_room.name = "laundry room";
    laundry_room.floor_name = "basement";
    laundry_room.zombie_amount = 0;
    laundry_room.item_amount = 1;
    laundry_room.weapon_amount = 0;
    laundry_room.func = &LaundryRoom;
    laundry_room.riddle = true;
    laundry_room.items[0] = &key_court;
    laundry_room.next_room[0] = &corridor_basement;
    for (int i = 1; i < next_array_size; i++)
    {
        laundry_room.next_room[i] = NULL;
    }
    for (int i = 1; i < items_array_size; i++)
    {
        laundry_room.items[i] = NULL;
    }
    for (int i = 0; i < weapon_array_size; i++)
    {
        laundry_room.weapons[i] = NULL;
    }
    for (int i = 1; i < enemies_array_size; i++)
    {
        if (counter < laundry_room.zombie_amount)
        {
            laundry_room.enemies[counter] = &zombieArray[counter_zombies];
            counter_zombies ++;
            counter ++;
        }
        else
        {
            laundry_room.enemies[counter] = NULL;
            counter ++;
        }
    }
    counter = 0;

    // set all "visited" to false and add zombies to room
    for (int i = 0; i < basement_size; i++)
    {
        basement[i].visited = false;
        basement[i].searched = 0;
    }
}

// Create all the zombies in the game and fill the array
void CreateZombies(int zombie_amount)
{
    for (int i = 0; i < zombie_amount; i++)
    {
        zombieArray[i].min_attack = 10;
        zombieArray[i].max_attack = 30;
        zombieArray[i].life = 50;
        zombieArray[i].is_dead = false;
    }
}


// ##################################################
// CREATE PLAYER, ITEMS & WEAPONS
// ##################################################

// Create player, items and weapons and assign them to the player
player CreatePlayer(void)
{
    // set item stats
        // 1. Herbs to increase HP
    herbs.name = "Herbs";
    herbs.amount = 0;
    herbs.picked_up = false;
        // 2. Key for Safe (Attic)
    key_safe.name = "Lion Key";
    key_safe.amount = 0;
    key_safe.picked_up = false;
        // 3. Key for 2 doors (2nd Floor)
    key_second.name = "Tiger Key";
    key_second.amount = 0;
    key_second.picked_up = true;
        // 4. Key for Courtyard
    key_court.name = "Courtyard Key";
    key_court.amount = 0;
    key_court.picked_up = false;
        // 5. Key for Art Room (1st Floor)
    key_art.name = "Art Room Key";
    key_art.amount = 0;
    key_art.picked_up = false;
        // 6. Key for Laundry Room (basement)
    key_laundry.name = "Laundry Room Key";
    key_laundry.amount = 0;
    key_laundry.picked_up = false;
        // 7. Bolt Cutter
    cutter.name = "Bolt Cutter";
    cutter.amount = 0;
    cutter.picked_up = false;
        // 8. Emblem for main entrance (1st Floor)
    emblem.name = "Lion's Heart Emblem";
    emblem.amount = 0;
    emblem.picked_up = false;
        // 9. Fuse for kitchen food elevator (2nd Floor)
    fuse.name = "fuse";
    fuse.amount = 0;
    fuse.picked_up = false;
    // set weapon stats
        // 1. Pistol
        pistol.name = "Pistol";
        pistol.ammo = 4;
        pistol.ammo_name = "bullets";
        pistol.min_damage = 20;
        pistol.max_damage = 40;
        pistol.picked_up = true;
        // 2. Shotgun
        shotgun.name = "Shotgun";
        shotgun.ammo = 1;
        shotgun.ammo_name = "shells";
        shotgun.min_damage = 45;
        shotgun.max_damage = 100;
        shotgun.picked_up = false;
        // 3. Knife
        knife.name = "Combat Knife";
        knife.ammo = 1;
        knife.ammo_name = "";
        knife.min_damage = 10;
        knife.max_damage = 20;
        knife.picked_up = true;
    
    // Create Player
    player new_player;
    new_player.inventory[0] = &herbs;
    new_player.inventory[1] = &key_safe;
    new_player.inventory[2] = &key_second;
    new_player.inventory[3] = &key_court;
    new_player.inventory[4] = &key_art;
    new_player.inventory[5] = &key_laundry;
    new_player.inventory[6] = &cutter;
    new_player.inventory[7] = &emblem;
    new_player.inventory[8] = &fuse;
    new_player.weapons[0] = &pistol;
    new_player.weapons[1] = &shotgun;
    new_player.weapons[2] = &knife;
    new_player.active_weapon = &pistol;
    new_player.current_room = &main_hall;
    new_player.health = 100;
    new_player.lives = 3;

    return new_player;
}

// ##################################################
// GAME COMMAND FUNCTIONS
// ##################################################

// Gives the player the ability to move through the mansion and call for help functions
void Action(player *player)
{
    printf("\nWhat do you want to do?\n");
    int index = 0;
    printf("\n'0' - Help\n");
    for (int i = 0; i < 6; i++)
    {
        if ((*player).current_room->next_room[i] != NULL)
        {
            index ++;
            printf("'%i' - Go to: '%s', %s\n", index, (*player).current_room->next_room[i]->name, (*player).current_room->next_room[i]->floor_name);
        }
        else 
        {
            break;
        }
    }
    printf("'9' - search the %s\n", (*player).current_room->name);
    int input = 100;
    printed = false;
    while (input < 0 || input > 9)
    {
        if (!printed)
        {
            printf("\n## Your choice:");
            printed = true;
        }
        input = get_int(" ");
    }
    if (input == 0)
    {
        Help(player);
        (*player).current_room->func(player);
    }
    else if (input == 9)
    {
        SearchRoom(player);
    }
    else
    {
        (*player).current_room->next_room[input-1]->func(player);
    }
}

// Search the room for items
void SearchRoom(player *player)
{
    if ((*player).current_room->item_amount == 0 && (*player).current_room->weapon_amount == 0)
    {
        printf("\nYour search was met with a profound sense of emptiness and disappointment.\n");
        printf("Despite your efforts, you've come up with nothing.\n");
    }
    else if ((*player).current_room->item_amount != 0 || (*player).current_room->weapon_amount != 0)
    {
        if ((*player).current_room->searched < (*player).current_room->item_amount + (*player).current_room->weapon_amount)
        {
            printf("\nAmidst the shadows and stillness, you can see something.\n");
            if ((*player).current_room->searched < (*player).current_room->item_amount)
            {
                printf("You have found %s!\n", (*player).current_room->items[(*player).current_room->searched]->name);
                (*player).current_room->items[(*player).current_room->searched]->amount ++;
                (*player).current_room->items[(*player).current_room->searched]->picked_up = true;
                (*player).current_room->searched ++;
            }
            else
            {
                int num = (*player).current_room->searched - (*player).current_room->item_amount;
                if ((*player).current_room->weapons[num] == &pistol)
                {
                    printf("You have found %s!\n", (*player).current_room->weapons[num]->ammo_name);
                    (*player).current_room->weapons[num]->ammo += 3;
                }
                else
                {
                    if ((*player).current_room->weapons[num]->picked_up == false)
                    {
                        printf("You have found a %s!\n", (*player).current_room->weapons[num]->name);
                    }
                    else
                    {
                        printf("You have found %s!\n", (*player).current_room->weapons[num]->ammo_name);
                        (*player).current_room->weapons[num]->ammo += 2;
                    }
                }
                (*player).current_room->weapons[num]->picked_up = true;
                (*player).current_room->searched ++;
            }
        }
        else
        {
            printf("\nDespite your efforts, there is nothing left to discover, only remnants of what once was.\n");
        }
    }
    Continue();
    Action(player);
}

// Check if zombies are in the room
void ZombieCheck(player *player, room *previous_room)
{
    int zombie_amount = (*player).current_room->zombie_amount;
    int dead_zombies = 0;
    if (zombie_amount > 0)
    {   
        for (int i = 0; i < zombie_amount; i++)
        {
            if ((*player).current_room->enemies[i]->is_dead == true)
            {
                dead_zombies ++;
            }
        }
        if (zombie_amount - dead_zombies > 0)
        {
            ZombieFight(player, &zombie_amount);
        }
    }
    else if (player->current_room == &main_attic && boss_attic == true)
    {
        BossAttic(player, previous_room);
        boss_attic = false;
    }
    else if (player->current_room == &main_basement && boss_basement == true)
    {
        BossBasement(player, previous_room);
        boss_basement = false;
    }
    else if (player->current_room == &cy_maze && boss_maze == true)
    {
        printf("\nLast chance. Do you really want to enter?\n");
        printf("Be ready for whatever nightmare lies there.\n");
        printf("\n## Type '0' to go back or '1' to proceed: ");
        int input;
        while (input != 0 || input != 1)
        {
            if (input == 0)
            {
                previous_room->func(player);
            }
            else if (input == 1)
            {
                break;
            }
            scanf("%d", &input);
        }
        MazeStory();
        MazeStart(player, p);
    }
}

// Decision if zombies are fought or not / then starts the fight or goes back to the previous room
void ZombieFight(player *player, int *zombie_amount)
{
    printf("\n#################### ZOMBIES #########################\n");
    int random_int = rand() % 5;
    printf("%s", appearing_messages[random_int]);

    // Give choice if you want to fight or run away
    printf("\nWhat are you going to do?\n");
    printf("\n'0' - Change weapon, then fight.");
    if (player->current_room->zombie_amount > 1)
    {
        printf("\n'1' - Fight the [2] zombies.\n");
    }
    else
    {
        printf("\n'1' - Fight the [1] zombie.\n");
    }
    int counter = 0;
    while((*player).current_room->next_room[counter] != NULL)
    {
        printf("'%i' - Flee to %s!\n", counter+2, (*player).current_room->next_room[counter]->name);
        counter ++;
    }
    int answer = 100;
    printed = false;
    while (answer < 0 || answer > counter+2)
    {
        if (!printed)
        {
            printf("\n## Your choice:");
            printed = true;
        }
        answer = get_int(" ");
    }
    printf("\n");
    // FIGHT!
    if (answer == 0)
    {
        ChangeWeapon(player);
        answer = 1;
    }
    if (answer == 1)
    {
        printf("\n#################### FIGHT #########################\n");
        Continue();
        random_int = rand() % 100;
        // 80% chance of player starting
        if (random_int < 80)
        {
            while (*zombie_amount != 0)
            {
                PlayerAttack(player, zombie_amount);
                if (*zombie_amount <= 0)
                {
                    int random = rand() % 5;
                    printf("You shot with your %s || %s -1\n", (*player).active_weapon->name, (*player).active_weapon->ammo_name);
                    printf("%s", win_messages[random]);
                    ContinueOne();
                    return;
                }
                ZombieAttack(player, zombie_amount);
            }
        }
        // 20% chance of zombie starting
        else if (random_int >= 80)
        {
            printf("The zombie lunged forward, its decaying claws reaching out to tear into your flesh in a savage and unrelenting attack.\n");
            while (*zombie_amount != 0)
            {
                ZombieAttack(player, zombie_amount);
                PlayerAttack(player, zombie_amount);
                if (*zombie_amount <= 0)
                {
                    int random = rand() % 5;
                    printf("\nYou shot with your %s || %s -1\n", (*player).active_weapon->name, (*player).active_weapon->ammo_name);
                    printf("%s", win_messages[random]);
                    ContinueOne();
                    return;
                }
            }
        }
    }
    // RUN AWAY!
    else
    {
        printf("You ran away!\n" );
        (*player).current_room->next_room[answer-2]->func(player);
    }
}

// Player attack (no interaction)
void PlayerAttack(player *player, int *zombie_amount)
{
    int random_int = rand() % 100;
    // 95% chance of landing a shot
    if (random_int > 5)
    {
        // Calculate damage
        random_int = (((*player).active_weapon->max_damage) - ((*player).active_weapon->min_damage)) + 1;
        int attack_damage = (*player).active_weapon->min_damage + rand() % random_int;

        // If no ammo
        if ((*player).active_weapon->ammo <= 0)
        {   
            random_int = (((*player).weapons[2]->max_damage) - ((*player).weapons[2]->min_damage)) + 1;
            attack_damage = (*player).weapons[2]->min_damage + rand() % random_int;
            printf("\nYou have no ammo! There is no time to change your weapon!\n");
            printf("You hit the zombie with your knife.\n");
            Continue();
            (*player).current_room->enemies[(*zombie_amount)-1]->life -= attack_damage;
            // If zombie has no life left
            if ((*player).current_room->enemies[(*zombie_amount)-1]->life <= 0)
            {
                (*player).current_room->enemies[(*zombie_amount)-1]->is_dead = true;
                (*zombie_amount) = (*zombie_amount) - 1;
                // If all zombies dead
                if (*zombie_amount <= 0)
                {
                    return;
                }
                // If there are still zombies left
                else
                {
                    int random = rand() % 5;
                    printf("\n%s", zombie_dies_messages_knife[random]);
                    Continue();
                    return;
                }
            }
            else
            {
                int random = rand() % 5;
                printf("\n%s", hit_messages_knife[random]);
                Continue();
                return;
            }
        }
        // If ammo
        else 
        {
            if ((*player).active_weapon != &knife)
            {
                (*player).active_weapon->ammo --;
            }
            (*player).current_room->enemies[*zombie_amount-1]->life -= attack_damage;
            // If zombie has no life left
            if ((*player).current_room->enemies[*zombie_amount-1]->life <= 0)
            {
                (*player).current_room->enemies[*zombie_amount-1]->is_dead = true;
                (*zombie_amount) = (*zombie_amount) - 1;
                // If all zombies dead
                if (*zombie_amount <= 0)
                {
                    return;
                }
                // If there are still zombies left
                else
                {
                    int random = rand() % 5;
                    if ((*player).active_weapon != &knife)
                    {
                        printf("\nYou shot with your %s || %s -1", (*player).active_weapon->name, (*player).active_weapon->ammo_name);
                        printf("\n%s", zombie_dies_messages[random]);
                    }
                    else
                    {
                        printf("\n%s", zombie_dies_messages_knife[random]);
                    }
                    Continue();
                    return;
                }
            }
            // If it just deducted health of Zombie
            else
            {
                int random = rand() % 5;
                if ((*player).active_weapon != &knife)
                {
                    printf("\nYou shot with your %s || %s -1", (*player).active_weapon->name, (*player).active_weapon->ammo_name);
                    printf("\n%s", hit_messages[random]);
                }
                else
                {
                    printf("\n%s", hit_messages_knife[random]);
                }
                Continue();
                return;
            }
        }
    }
    // 5% chance of missing your shot
    else
    {
        printf("You shot with your %s || %s -1", (*player).active_weapon->name, (*player).active_weapon->ammo_name);
        printf("\n\nTrembling with terror, your hand wavered and the shot went wide.\n");
        Continue();
        return;
    }
}

// Zombie attack - interaction: Dodge
void ZombieAttack(player *player, int *zombie_amount)
{
    // Zombie ATTACK
    if (player->current_room->zombie_amount > 1)
    {
        // 2 Zombies attack
        printf("\nThe zombies are attacking you. Try to dodge!\n");
    }
    else
    {
        // 1 Zombie attacks
        printf("\nThe zombie is attacking you. Try to dodge!\n");
    }
    printf("\n## Type '1' when you're ready: ");
    int input_new = 100;
    while (input_new != 1)
    {
        if (input_new == 1)
        {
            break;
        }
        scanf("%d", &input_new);
    }
    int length = 0;
    printf("\n--- 3 ---\n");
    sleepMilliSecond(1000);
    printf("\n--- 2 ---\n");
    sleepMilliSecond(1000);
    printf("\n--- 1 ---\n");
    sleepMilliSecond(1000);
    int line;
    if (player->current_room->zombie_amount > 1)
    {
        // 2 Zombies attack
        line = ZombieAttackLine(2);
    }
    else
    {
        // 1 Zombie attacks
        line = ZombieAttackLine(1);
    }
    printf("\n");
    while (1)
    {
        if (kbhit())
        {
            // dodge
            break;
        }
        printf("\033[1;31m-\033[0m");
        sleepMilliSecond(15);
        length ++;
        if (length == 50)
        {
            // hit
            break;
        }
    }
    if (player->current_room->zombie_amount > 1)
    {
        // 2 Zombies attack
        line = ZombieAttackLine(2);
        if (length == line || length == line + 1 || length == line + 2 || length == line + 7 || length == line + 13 || length == line + 14 || length == line + 15)
        {
            int random = rand() % 5;
            printf("\n%s", zombie_missed_messages[random]);
            ContinueOne();
        }
        else
        {
            int random = rand() % 5;
            printf("\n%s", zombies_hit_messages[random]);
            int random_int = ((zombieArray[0].max_attack) - (zombieArray[0].min_attack)) + 1;
            int attack_damage = zombieArray[0].min_attack + (rand() % random_int);
            (*player).health -= attack_damage;
            printf("\nYour health is down to %i\n", (*player).health);
            // If player has no life left
            if ((*player).health <= 0)
            {
                Death(player);
            }
            ContinueOne();
        }
    }
    else
    {
        line = ZombieAttackLine(1);
        if (length == line || length == line + 1 || length == line + 2 || length == line + 3 || length == line + 8 || length == line + 9 || length == line + 10 || length == line + 11)
        {
            int random = rand() % 5;
            printf("\n%s", zombie_missed_messages[random]);
            ContinueOne();
        }
        else
        {
            int random = rand() % 5;
            printf("\n%s", zombies_hit_messages[random]);
            int random_int = ((zombieArray[0].max_attack) - (zombieArray[0].min_attack)) + 1;
            int attack_damage = zombieArray[0].min_attack + (rand() % random_int);
            (*player).health -= attack_damage;
            printf("\nYour health is down to %i\n", (*player).health);
            // If player has no life left
            if ((*player).health <= 0)
            {
                Death(player);
            }
            ContinueOne();
        }
    }
}

// Pause and wait for Player to continue with enter
void Continue(void)
{
    printf("\n|| Press any key to continue ||\n");
    sleepMilliSecond(100);
    while (1)
    {
        if (kbhit())
        {
            getch();
            break;
        }
    }
}

// Pause and wait for Player to continue with typing 1
void ContinueOne(void)
{
    int input = 0;
    printed = false;
    while (input != 1)
    {
        if (!printed)
        {
            printf("\n## Type '1' to continue:");
            printed = true;
        }
        input = get_int(" ");
    }
}

// Show current location
void ShowLocation(player *player)
{
    printf("\nYour location: %s, %s.\n", (*player).current_room->name, (*player).current_room->floor_name);
    Continue();
}

// Show current inventory
void ShowInventory(player *player)
{
    printf("\n# INVENTORY:\n");
    for (int i = 0; i < 3; i++)
    {
        if ((*player).weapons[i]->picked_up == true)
        {
            printf("\n%s: %i", (*player).weapons[i]->name, (*player).weapons[i]->ammo);
        }
    }
    for (int i = 0; i < 8; i++)
    {
        if ((*player).inventory[i]->picked_up == true)
        {
            printf("\n%s: %i", (*player).inventory[i]->name, (*player).inventory[i]->amount);
        }
    }
    printf("\n");
    Continue();
}

// Change weapon
void ChangeWeapon(player *player)
{
    printf("\nWhich weapon do you want to change?\n");
    for (int i = 0; i < 3; i++)
    {
        if ((*player).weapons[i]->picked_up == true)
        {
            printf("\n'%i': %s", i+1, (*player).weapons[i]->name);
        }
    }
    printf("\n");
    printf("\nType the number of the weapon or '0' if you don't wan't to change your weapon.\n");
    int input = 100;
    printed = false;
    while (input < 0 || input > 3)
    {   
        if (!printed)
        {
            printf("\n## Your choice:");
            printed = true;
        }
        input = get_int(" ");
    }
    if (input == 1)
    {
        (*player).active_weapon = (*player).weapons[0];
        printf("\n-- Pistol equipped --\n");
    }
    else if (input == 2)
    {
        if ((*player).weapons[1]->picked_up == false)
        {
            printf("\nYou don't have this weapon.\n");
            ChangeWeapon(player);
        }
        else if ((*player).weapons[1]->picked_up == true)
        {
            (*player).active_weapon = (*player).weapons[1];
            printf("\n-- Shotgun equipped --\n");
        }
    }
    if (input == 3)
    {
        (*player).active_weapon = (*player).weapons[2];
        printf("\n-- Combat Knife equipped --\n");
    }
    else if (input == 0)
    {
        goto end;
    }
    end:
    ContinueOne();
}

// Show health of the player
void ShowHealth(player *player)
{
    printf("\nYour health is %i of 100.\n", (*player).health);
    Continue();
}

// Use herbs to increase HP
void UseHerbs(player *player) 
{
    if ((*player).health == 100)
    {
        printf("\nYou already have full health.\n");
    }
    else if ((*player).inventory[0]->amount == 0)
    {
        printf("\nYou don't have any herbs.\n");
    }
    else
    {
        (*player).health += 40;
        if((*player).health > 100)
        {
            (*player).health = 100;
        }
        (*player).inventory[0]->amount -= 1;
        printf("\nYou'r health is %i of 100.\n", (*player).health);
    }
    Continue();
}

// get all action possibilities
void Help(player *player)
{
    printf("\nWhat do you want to do?\n");
    printf("\n'1' show inventory, '2' show location");
    printf("\n'3' change weapon, '4' show health");
    printf("\n'5' use herbs, '6' back\n");
    int input = 10;
    printed = false;
    while (input < 1 || input > 6)
    {
        if (!printed)
        {
            printf("\n## Your choice:");
            printed = true;
        }
        input = get_int(" ");
    }
    if (input == 1)
    {
        ShowInventory(player);
    }
    else if (input == 2)
    {
        ShowLocation(player);
    }
    else if (input == 3)
    {
        ChangeWeapon(player);
    }
    else if (input == 4)
    {
        ShowHealth(player);
    }
    else if (input == 5)
    {
        UseHerbs(player);
    }
    else if (input == 6)
    {
        return;
    }
}

// If player is out of HP he will die. Able to continue the game in this function if enough lives
void Death(player *player)
{
    Continue();
    (*player).lives -= 1;
    int random = rand() % 5;
    printf("\n%s\n", death_messages[random]);
    Continue();
    if ((*player).lives <= 0)
    {
        printf("\nYour journey is over, brave soldier. The mansion is your grave.\n");
        Continue();
        exit(0);
    }
    else
    {
        printf("\nYou have %i lives left. Do you want to continue?\n", (*player).lives);
        printf("## Type '0' to exit and '1' to continue\n");
        int input;
        while (input != 0 || input != 1)
        {
        if (input == 1)
        {
            printf("\nIt seems like you're still breathing..");
            printf("\nGet up and continue to fight!\n");
            (*player).health = 100;
            break;
        }
        else if (input == 0)
        {
            printf("\nYour journey is over, brave soldier. The mansion is your grave.\n");
            Continue();
            exit(0);
        }
        scanf("%d", &input);
    }
    }

}

// ##################################################
// RIDDLES
// ##################################################

// riddle to check different objects
void DiningRoomRiddle(player *player, room *previous_room)
{
    printf("\n'It hangs upon the wall, a silent work of art,'\n");
    printf("'But look beyond its beauty, and you'll make a start.'\n");
    printf("'A secret waits behind, that only few can find,'\n");
    printf("'A button, pressed with care, will unlock what's confined.'\n");
    Continue();

    printf("\nThere are some creepy things in this room.\n");
    printf("\nWhat do you want to do?\n");

    printf("\n'0' - Go back to main hall.\n");
    printf("'1' - Inspect the table.\n");
    printf("'2' - Inspect the painting.\n");
    printf("'3' - Inspect the dish cabinet.\n");
    printf("'4' - Inspect the blood on the wall.\n");

    int input = 100;
    printed = false;
    while(input < 0 || input > 4)
    {
        if (!printed)
        {
            printf("\n## Your choice:");
            printed = true;
        }
        input = get_int(" ");
    }

    if (input == 0)
    {
        printf("\nYou are leaving the dining room.\n");
        (*player).current_room = previous_room;
        Continue();
        (*player).current_room->func(player);
    }
    else if (input == 1)
    {
        printf("\nThe once-pristine tablecloth is now stained with large, dark splotches of dried blood.\n");
        printf("The silverware that was once polished to a shine is now caked with rust-colored residue.\n");
        printf("The sight alone is enough to make your skin crawl, and as you scan the table,\n");
        printf("you realize that apart from the blood-covered silverware, there is nothing unusual to be found.\n");
        Continue();
        printf("\nYou are leaving the dining room.\n");
        (*player).current_room = previous_room;
        Continue();
        (*player).current_room->func(player);
    }
    else if (input == 2)
    {
        printf("\nYou approach the painting, taking in the unsettling image of a distorted and twisted figure that is captured upon the canvas.\n");
        printf("And then, as you step closer, you notice that there is a small, almost imperceptible indentation behind the painting.\n");
        printf("Your heart beats faster as you realize that this indentation is in the shape of a button.\n");
        printf("You hesitate for a moment, your hand hovering above the button as you weigh the potential consequences of pressing it. \n");
        Continue();
        printf("\nThe air is thick with tension, and the silence of the room only amplifies the sound of your own racing heartbeat.\n");
        printf("Do you dare to press the button and unlock what lies hidden behind the painting?\n");
        printf("\n'0' - Don't.\n");
        printf("'1' - Press the button.\n");
        int new_input = 100;
        printed = false;
        while(new_input < 0 || new_input > 1)
        {
            if (!printed)
            {
                printf("\n## Your choice:");
                printed = true;
            }
            new_input = get_int(" ");
        }
        if (new_input == 0)
        {
            printf("\nThe silence of the room is deafening as you stand there, your hand still hovering above the button, as though waiting for a sign.\n");
            printf("But as the moments stretch into eternity, you realize that you've made the decision to not press it.\n");
            printf("\nYou are leaving the dining room.\n");
            (*player).current_room = previous_room;
            Continue();
            (*player).current_room->func(player);
        }
        else if (new_input == 1)
        {
            printf("\nWith a deep breath, you make the decision to press the button. The air is heavy with tension as your fingers make contact, and a low mechanical whirring fills the room.\n");
            printf("Suddenly, the sound of grinding gears and creaking wood fills the room, and the dish cabinet slowly begins to move, revealing a hidden door that was concealed behind it.\n");
            printf("The door groans as it opens, revealing a pitch-black void that seems to swallow the light.\n");
            printf("The hairs on the back of your neck stand on end as you realize that you have unlocked a secret that was meant to remain hidden.\n");
            dining_room.riddle = false;
            Continue();
            return;
        }
    }
    else if (input == 3)
    {
        printf("\nAs you inspect the dish cabinet, you can't shake the feeling that there is something off about it.\n");
        printf("You run your fingers along the smooth surface, searching for any indication of what might be hidden behind it.\n");
        printf("Suddenly, you hear a faint sound coming from within, like a faint whisper that you can't quite make out.\n");
        printf("You press your ear to the cabinet, straining to hear what might be lurking inside.\n");
        Continue();
        printf("\nBut as you try to move the cabinet, you realize that it's firmly anchored to the wall.\n");
        printf("The sense of unease grows within you as you realize that whatever is behind the cabinet is trapped,\n");
        printf("and now the only sound you can hear is the pounding of your own heart.\n");
        Continue();
        printf("\nYou are leaving the dining room.\n");
        (*player).current_room = previous_room;
        Continue();
        (*player).current_room->func(player);
    }
    else if (input == 4)
    {
        printf("\nAs you approach the blood-stained wall, the smell of iron fills your nostrils, and you realize that the blood is still fresh.\n");
        printf("The sight of the sticky, dark red liquid makes your stomach turn, and you can feel your knees starting to buckle.\n");
        printf("Despite your fear, you force yourself to inspect the blood more closely, searching for any signs of what might have caused this horrific scene.\n");
        printf("But as you look more closely, you see nothing out of the ordinary. The only evidence of what happened is the blood, which seems to be mocking you with its blank emptiness.\n");
        Continue();
        printf("\nYou are leaving the dining room.\n");
        (*player).current_room = previous_room;
        Continue();
        (*player).current_room->func(player);
    }
}

// riddle to open a door or leave it closed
void VestibuleRiddle (player *player, room *previous_room)
{
    printf("\n What do you want to do?\n");
    printf("\n'0' - Go back.\n");
    printf("'1' - Remove the wooden planks from the wall.\n");
    int input = 100;
    printed = false;
    while(input < 0 || input > 1)
    {
        if (!printed)
        {
            printf("\n## Your choice:");
            printed = true;
        }
        input = get_int(" ");
    }
    if (input == 0)
    {
        (*player).current_room = previous_room;
        printf("\nYou know that you need to get out of this place as soon as possible,\n");
        printf("so you rush towards the latter to go back down.\n");
        Continue();
        (*player).current_room->func(player);
    }
    else if (input == 1)
    {
        printf("\nWith trembling hands, you begin to remove the wooden planks that seal the door. The boards creak and groan as you pry them away from the door frame.\n");
        printf("The air around you seems to grow colder as you work, and a feeling of dread begins to build in the pit of your stomach.\n");
        printf("You can't shake the sense that you are making a terrible mistake, but your curiosity is stronger than your fearr, and you continue to work until the last board is removed.\n");
        printf("You hesitate, wondering if you should turn back, but the urge to know what is behind the door is too strong to ignore.\n");
        vestibule.riddle = false;
        Continue();        
    }
}

// riddle of sorting letters
void GuestRoomRiddle(player *player, room *previous_room)
{
    printf("\n'I unlock the doors, to secrets kept inside,'\n");
    printf("'I hold the power, to open and provide.'\n");
    printf("'In darkness I reside, in shadows I'm concealed,'\n");
    printf("'But with a single turn, I reveal what's been sealed.'\n");
    Continue();
    printf("\n");
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            printf("'%c' ", array[i][j]);
        }
        printf("\n");
    }
    printf("\nYou notice that the area where the three tiles spelling 'D-I-E' are placed is slightly illuminated.\n");

    int space_row, space_col;

    // Find the position of the empty character
    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            if (array[i][j] == ' ') 
            {
                space_row = i;
                space_col = j;
            }
        }
    }
    printf("\n## Type a letter to move or '0' to exit: ");
    check(player, &space_row, &space_col, previous_room);
}

// input check for GuestRoomRiddle
void check(player *player, int *row, int *col, room *previous_room)
{
    if (array[1][0] == 75 && array[1][1] == 69 && array[1][2] == 89)
    {
        printf("\n");
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                printf("'%c' ", array[i][j]);
            }
            printf("\n");
        }
        printf("\nWith bated breath, you meticulously worked through the complex puzzle, feeling a sense of mounting unease with every shift of the tiles.\n");
        printf("Suddenly, with a sharp click, the lock on the metal closet gave way and a musty, damp scent wafted out from the depths of the closet.\n");
        Continue();
        guest_room.riddle = false;
        guest_room.func(player);
        return;
    }
    else
    {
        int input = 100;
        while (input != 48 && input != 75 && input != 66 && input != 88 && input != 68 && input != 73 && input != 69 && input != 76 && input != 89)
        {
            input = getchar();
            if (isdigit(input) == 0) 
            {
                input = toupper(input);
            }
        }
        if (input == 48)
        {
            player->current_room = previous_room;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    array[i][j] = sample[i][j];
                }
            }
            printf("\nYou return to the master bedroom.\n");
            Continue();
            player->current_room->func(player);
        }
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                // Check if the character is above the empty character
                if (input == array[(*row) - 1][(*col)] && (*row) - 1 >= 0) 
                {
                    char buffer = array[(*row) - 1][(*col)];
                    array[(*row)][(*col)] = buffer;
                    array[(*row) - 1][(*col)] = ' ';
                    *row = (*row) - 1;
                    printf("\n");
                    for (int i = 0; i < 3; i++)
                    {
                        for (int j = 0; j < 3; j++)
                        {
                            printf("'%c' ", array[i][j]);
                        }
                        printf("\n");
                    }
                    printf("\nYour Move: \n");

                    check(player, row, col, previous_room);
                }
                // Check if the character is below the empty character
                else if (input == array[(*row) + 1][(*col)] && (*row) + 1 < 3) 
                {
                    char buffer = array[(*row) + 1][(*col)];
                    array[(*row)][(*col)] = buffer;
                    array[(*row) + 1][(*col)] = ' ';
                    *row = (*row) + 1;
                    printf("\n");
                    for (int i = 0; i < 3; i++)
                    {
                        for (int j = 0; j < 3; j++)
                        {
                            printf("'%c' ", array[i][j]);
                        }
                        printf("\n");
                    }
                    printf("\nYour Move: \n");

                    check(player, row, col, previous_room);
                }
                // Check if the character is to the left of the empty character
                else if (input == array[(*row)][(*col) - 1] && (*col) - 1 >= 0) 
                {
                    char buffer = array[(*row)][(*col) - 1];
                    array[(*row)][(*col)] = buffer;
                    array[(*row)][(*col) - 1] = ' ';
                    *col = (*col) - 1;
                    printf("\n");
                    for (int i = 0; i < 3; i++)
                    {
                        for (int j = 0; j < 3; j++)
                        {
                            printf("'%c' ", array[i][j]);
                        }
                        printf("\n");
                    }
                    printf("\nYour Move: \n");

                    check(player, row, col, previous_room);
                }
                // Check if the character is to the right of the empty character
                else if (input == array[(*row)][(*col) + 1] && (*col) + 1 < 3) 
                {
                    char buffer = array[(*row)][(*col) + 1];
                    array[(*row)][(*col)] = buffer;
                    array[(*row)][(*col) + 1] = ' ';
                    *col = (*col) + 1;
                    printf("\n");
                    for (int i = 0; i < 3; i++)
                    {
                        for (int j = 0; j < 3; j++)
                        {
                            printf("'%c' ", array[i][j]);
                        }
                        printf("\n");
                    }
                    printf("\nYour Move: \n");

                    check(player, row, col, previous_room);
                }
                else 
                {
                    check(player, row, col, previous_room);
                }
            }
        }
    }
}

// Riddle to convert a letter into binary code
void LaundryRoomRiddle(player *player, room *previous_room)
{
    printf("\nI am a simple letter, just one of many you see,\n");
    printf("But when I'm translated, to binary I'll be.\n");
    printf("With zeros and ones, I'll be represented with flair,\n");
    printf("So listen up closely, and see if you can guess where.\n");
    Continue();
    printf("\nYou see a metallic keypad that can accept 8 digits.\n");
    printf("\nUpon closer inspection, you notice that someone has delicately carved the letter 'o' into the casing of the device.\n");

    printf("\n## Type '0' if you want to return.\n");
    string code = "01101111";
    string back = "0";
    string input = get_string("Keypad: ");
    while(strcmp(input, code) != 0 || strcmp(input, back) != 0)
    {
        input = get_string("Keypad: ");
    }
    printf("\n");
    if (strcmp(input, back) == 0)
    {
        printf("\nYou know that you are not able to solve that riddle,\n");
        printf("so you leave the room and go back into the corridor.\n");
        Continue();
        player->current_room = previous_room;
        (*player).current_room->func(player);
    }
    else if (strcmp(input, code) == 0)
    {
        printf("\nAs you hesitantly typed in the series of numbers into the keypad, your heart raced with fear of the unknown.\n");
        printf("With a beep, the code was accepted and the lock on the door of the massive machine clicked open.\n");
        laundry_room.riddle = false;
        Continue();
        return;        
    }
}

// Function to use Windows and Unix-based sleep-function
void sleepMilliSecond(long milliSecondInput) 
{
#ifdef _WIN32
    Sleep(milliSecondInput);
#else
    usleep(pollingDelay * 1000);
#endif
}

// ##################################################
// BOSS FIGHTS
// ##################################################

// print-outs for boss fights
int Skull(int num)
{
    if (num == 1)
    {

        printf("\nMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMNKOdolcccclodOKNMMMMMMMMMMMM");
        printf("\nMMMMMMMMMXkc,.     \033[1;31m..\033[0m     .,ckXMMMMMMMMM");
        printf("\nMMMMMMMKo'  \033[1;31m.,cdkO0000Okdc,.\033[0m  'oKMMMMMMM");
        printf("\nMMMMMWk'  \033[1;31m'o0NMMMMMMMMMMMMN0o.\033[0m  'kWMMMMM");
        printf("\nMMMMWd. \033[1;31m.lKMMMMMMMMMMMMMMMMMMKl.\033[0m .dWMMMM");
        printf("\nMMMMO.  \033[1;31mlNMMMMMMMMMMMMMMMMMMMMNl\033[0m  .OMMMM");
        printf("\nMMMWo  \033[1;31m'0MMMMMMMMMMMMMMMMMMMMMM0'\033[0m  oWMMM");
        printf("\nMMMWo  \033[1;31m'0MMMWXKXWMMMMMMWXKXWMMM0'\033[0m  oWMMM");
        printf("\nMMMMk. \033[1;31m.dWWO;...;OWMMWO;...:OWWd.\033[0m .kMMMM");
        printf("\nMMMMXc  \033[1;31m,0X:     :XMMX:     :X0,\033[0m  cXMMMM");
        printf("\nMMMMMO'  \033[1;31moNx'   'xWMMWx'   'xNo\033[0m  'OMMMMM");
        printf("\nMMMMMX;  \033[1;31m;0XKOkOXNOooONXOkOKX0;\033[0m  ;XMMMMM");
        printf("\nMMMMMWk'  \033[1;31m..,ckNWx.  .xWNkc,..\033[0m  'kWMMMMM");
        printf("\nMMMMMMWKxc;'. \033[1;31m.lXOoddoOKl\033[0m  .';cxKWMMMMMM");
        printf("\nMMMMMMMMMMWNd. \033[1;31m.xWMMMMWx.\033[0m .dNWMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMX:  \033[1;31m.:cccc:.\033[0m  :XMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMMXd,........,dXMMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMMMMWXKOOOOKXWMMMMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMMMMMMM\033[1;31mXXXX\033[0mMMMMMMMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMMMMMMM");
        return 18;
    }
    else if (num == 2)
    {
        printf("\nMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMNKOdolcccclodOKNMMMMMMMMMMMM");
        printf("\nMMMMMMMMMXkc,.     \033[1;31m..\033[0m     .,ckXMMMMMMMMM");
        printf("\nMMMMMMMKo'  \033[1;31m.,cdkO0000Okdc,.\033[0m  'oKMMMMMMM");
        printf("\nMMMMMWk'  \033[1;31m'o0NMMMMMMMMMMMMN0o.\033[0m  'kWMMMMM");
        printf("\nMMMMWd. \033[1;31m.lKMMMMMMMMMMMMMMMMMMKl.\033[0m .dWMMMM");
        printf("\nMMMMO.  \033[1;31mlNMMMMMMMMMMMMMMMMMMMMNl\033[0m  .OMMMM");
        printf("\nMMMWo  \033[1;31m'0MMMMMMMMMMMMMMMMMMMMMM0'\033[0m  oWMMM");
        printf("\nMMMWo  \033[1;31m'0MMMWXKXWMMMMMMWXKXWMMM0'\033[0m  oWMMM");
        printf("\nMMMMk. \033[1;31m.dWWO;...;OWMMWO;...:OWWd.\033[0m .kMMMM");
        printf("\nMMMMXc  \033[1;31m,0X:     :XMMX:     :X0,\033[0m  cXMMMM");
        printf("\nMMMMMO'  \033[1;31moNx'   'xWMMWx'   'xNo\033[0m  'OMMMMM");
        printf("\nMMMMMX;  \033[1;31m;0XKOkOXNOooONXOkOKX0;\033[0m  ;XMMMMM");
        printf("\nMMMMMWk'  \033[1;31m..,ckNWx.  .xWNkc,..\033[0m  'kWMMMMM");
        printf("\nMMMMMMWKxc;'. \033[1;31m.lXOoddoOKl\033[0m  .';cxKWMMMMMM");
        printf("\nMMMMMMMMMMWNd. \033[1;31m.xWMMMMWx.\033[0m .dNWMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMX:  \033[1;31m.:cccc:.\033[0m  :XMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMMXd,........,dXMMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMMMMWXKOOOOKXWMMMMMMMMMMMMMMM");
        printf("\nMMMMMMMMM\033[1;31mXXXX\033[0mMMMMMMMMMMMMMMMMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMMMMMMM");
        return 9;
    }
    else if (num == 3)
    {
        printf("\nMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMNKOdolcccclodOKNMMMMMMMMMMMM");
        printf("\nMMMMMMMMMXkc,.     \033[1;31m..\033[0m     .,ckXMMMMMMMMM");
        printf("\nMMMMMMMKo'  \033[1;31m.,cdkO0000Okdc,.\033[0m  'oKMMMMMMM");
        printf("\nMMMMMWk'  \033[1;31m'o0NMMMMMMMMMMMMN0o.\033[0m  'kWMMMMM");
        printf("\nMMMMWd. \033[1;31m.lKMMMMMMMMMMMMMMMMMMKl.\033[0m .dWMMMM");
        printf("\nMMMMO.  \033[1;31mlNMMMMMMMMMMMMMMMMMMMMNl\033[0m  .OMMMM");
        printf("\nMMMWo  \033[1;31m'0MMMMMMMMMMMMMMMMMMMMMM0'\033[0m  oWMMM");
        printf("\nMMMWo  \033[1;31m'0MMMWXKXWMMMMMMWXKXWMMM0'\033[0m  oWMMM");
        printf("\nMMMMk. \033[1;31m.dWWO;...;OWMMWO;...:OWWd.\033[0m .kMMMM");
        printf("\nMMMMXc  \033[1;31m,0X:     :XMMX:     :X0,\033[0m  cXMMMM");
        printf("\nMMMMMO'  \033[1;31moNx'   'xWMMWx'   'xNo\033[0m  'OMMMMM");
        printf("\nMMMMMX;  \033[1;31m;0XKOkOXNOooONXOkOKX0;\033[0m  ;XMMMMM");
        printf("\nMMMMMWk'  \033[1;31m..,ckNWx.  .xWNkc,..\033[0m  'kWMMMMM");
        printf("\nMMMMMMWKxc;'. \033[1;31m.lXOoddoOKl\033[0m  .';cxKWMMMMMM");
        printf("\nMMMMMMMMMMWNd. \033[1;31m.xWMMMMWx.\033[0m .dNWMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMX:  \033[1;31m.:cccc:.\033[0m  :XMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMMXd,........,dXMMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMMMMWXKOOOOKXWMMMMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMMMMMMMMMMMMMM\033[1;31mXXXX\033[0mMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMMMMMMM");
        return 25;
    }
    else if (num == 4)
    {
        printf("\nMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMNKOdolcccclodOKNMMMMMMMMMMMM");
        printf("\nMMMMMMMMMXkc,.     \033[1;31m..\033[0m     .,ckXMMMMMMMMM");
        printf("\nMMMMMMMKo'  \033[1;31m.,cdkO0000Okdc,.\033[0m  'oKMMMMMMM");
        printf("\nMMMMMWk'  \033[1;31m'o0NMMMMMMMMMMMMN0o.\033[0m  'kWMMMMM");
        printf("\nMMMMWd. \033[1;31m.lKMMMMMMMMMMMMMMMMMMKl.\033[0m .dWMMMM");
        printf("\nMMMMO.  \033[1;31mlNMMMMMMMMMMMMMMMMMMMMNl\033[0m  .OMMMM");
        printf("\nMMMWo  \033[1;31m'0MMMMMMMMMMMMMMMMMMMMMM0'\033[0m  oWMMM");
        printf("\nMMMWo  \033[1;31m'0MMMWXKXWMMMMMMWXKXWMMM0'\033[0m  oWMMM");
        printf("\nMMMMk. \033[1;31m.dWWO;...;OWMMWO;...:OWWd.\033[0m .kMMMM");
        printf("\nMMMMXc  \033[1;31m,0X:     :XMMX:     :X0,\033[0m  cXMMMM");
        printf("\nMMMMMO'  \033[1;31moNx'   'xWMMWx'   'xNo\033[0m  'OMMMMM");
        printf("\nMMMMMX;  \033[1;31m;0XKOkOXNOooONXOkOKX0;\033[0m  ;XMMMMM");
        printf("\nMMMMMWk'  \033[1;31m..,ckNWx.  .xWNkc,..\033[0m  'kWMMMMM");
        printf("\nMMMMMMWKxc;'. \033[1;31m.lXOoddoOKl\033[0m  .';cxKWMMMMMM");
        printf("\nMMMMMMMMMMWNd. \033[1;31m.xWMMMMWx.\033[0m .dNWMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMX:  \033[1;31m.:cccc:.\033[0m  :XMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMMXd,........,dXMMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMMMMWXKOOOOKXWMMMMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\033[1;31mXXXX\033[0mMMMMMM");
        printf("\nMMMMMMMMMMMMMMMMMM");
        return 30;
    }
    // for Maze dodge
    else if (num == 9)
    {
        printf("\nMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMNKOdolcccclodOKNMMMMMMMMMMMM");
        printf("\nMMMMMMMMMXkc,.     \033[1;31m..\033[0m     .,ckXMMMMMMMMM");
        printf("\nMMMMMMMKo'  \033[1;31m.,cdkO0000Okdc,.\033[0m  'oKMMMMMMM");
        printf("\nMMMMMWk'  \033[1;31m'o0NMMMMMMMMMMMMN0o.\033[0m  'kWMMMMM");
        printf("\nMMMMWd. \033[1;31m.lKMMMMMMMMMMMMMMMMMMKl.\033[0m .dWMMMM");
        printf("\nMMMMO.  \033[1;31mlNMMMMMMMMMMMMMMMMMMMMNl\033[0m  .OMMMM");
        printf("\nMMMWo  \033[1;31m'0MMMMMMMMMMMMMMMMMMMMMM0'\033[0m  oWMMM");
        printf("\nMMMWo  \033[1;31m'0MMMWXKXWMMMMMMWXKXWMMM0'\033[0m  oWMMM");
        printf("\nMMMMk. \033[1;31m.dWWO;...;OWMMWO;...:OWWd.\033[0m .kMMMM");
        printf("\nMMMMXc  \033[1;31m,0X:     :XMMX:     :X0,\033[0m  cXMMMM");
        printf("\nMMMMMO'  \033[1;31moNx'   'xWMMWx'   'xNo\033[0m  'OMMMMM");
        printf("\nMMMMMX;  \033[1;31m;0XKOkOXNOooONXOkOKX0;\033[0m  ;XMMMMM");
        printf("\nMMMMMWk'  \033[1;31m..,ckNWx.  .xWNkc,..\033[0m  'kWMMMMM");
        printf("\nMMMMMMWKxc;'. \033[1;31m.lXOoddoOKl\033[0m  .';cxKWMMMMMM");
        printf("\nMMMMMMMMMMWNd. \033[1;31m.xWMMMMWx.\033[0m .dNWMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMX:  \033[1;31m.:cccc:.\033[0m  :XMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMMXd,........,dXMMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMMMMWK\033[1;31mXXXXXX\033[0mKWMMMMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMMMMMM\033[1;31mXXXXXX\033[0mMMMMMMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMMMMMMM");
        return 0;
    }
    else
    {
        printf("\nMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMNKOdolcccclodOKNMMMMMMMMMMMM");
        printf("\nMMMMMMMMMXkc,.     \033[1;31m..\033[0m     .,ckXMMMMMMMMM");
        printf("\nMMMMMMMKo'  \033[1;31m.,cdkO0000Okdc,.\033[0m  'oKMMMMMMM");
        printf("\nMMMMMWk'  \033[1;31m'o0NMMMMMMMMMMMMN0o.\033[0m  'kWMMMMM");
        printf("\nMMMMWd. \033[1;31m.lKMMMMMMMMMMMMMMMMMMKl.\033[0m .dWMMMM");
        printf("\nMMMMO.  \033[1;31mlNMMMMMMMMMMMMMMMMMMMMNl\033[0m  .OMMMM");
        printf("\nMMMWo  \033[1;31m'0MMMMMMMMMMMMMMMMMMMMMM0'\033[0m  oWMMM");
        printf("\nMMMWo  \033[1;31m'0MMMWXKXWMMMMMMWXKXWMMM0'\033[0m  oWMMM");
        printf("\nMMMMk. \033[1;31m.dWWO;...;OWMMWO;...:OWWd.\033[0m .kMMMM");
        printf("\nMMMMXc  \033[1;31m,0X:     :XMMX:     :X0,\033[0m  cXMMMM");
        printf("\nMMMMMO'  \033[1;31moNx'   'xWMMWx'   'xNo\033[0m  'OMMMMM");
        printf("\nMMMMMX;  \033[1;31m;0XKOkOXNOooONXOkOKX0;\033[0m  ;XMMMMM");
        printf("\nMMMMMWk'  \033[1;31m..,ckNWx.  .xWNkc,..\033[0m  'kWMMMMM");
        printf("\nMMMMMMWKxc;'. \033[1;31m.lXOoddoOKl\033[0m  .';cxKWMMMMMM");
        printf("\nMMMMMMMMMMWNd. \033[1;31m.xWMMMMWx.\033[0m .dNWMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMX:  \033[1;31m.:cccc:.\033[0m  :XMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMMXd,........,dXMMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMMMMWK\033[1;32mXXXXXX\033[0mKWMMMMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMMMMMM\033[1;32mXXXXXX\033[0mMMMMMMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMMMMMMM");
        return 0;
    }
}

// Combat system for Boss fights
void BossFight(player *player, int *boss_life, bool *boss_alive, int boss_life_display)
{
    int input = 100;
    printf("\n## Type '0' for help actions\n## or '1' when you're ready to try shooting the beast: ");
    
    while (input != 0 || input != 1)
    {
        if (input == 1)
        {
            break;
        }
        else if (input == 0)
        {
            Help(player);
            BossFight(player, boss_life, boss_alive, boss_life_display);
        }
        scanf("%d", &input);
    }
    int length = 0;
    printf("\n--- 3 ---\n");
    sleepMilliSecond(1000);
    printf("\n--- 2 ---\n");
    sleepMilliSecond(1000);
    printf("\n--- 1 ---\n");
    sleepMilliSecond(1000);
    Skull(0);
    printf("\n");
    while (1)
    {
        if (kbhit())
        {
            // aim and shoots
            break;
        }
        printf("\033[1;31m-\033[0m");
        sleepMilliSecond(15);
        length ++;
        if (length == 50)
        {
            // missed shot
            break;
        }
    }
    if (length == 18 || length == 19 || length == 20 || length == 21 || length == 22 || length == 23)
    {
        int random = rand() % 5;
        printf("\n%s\n", boss_hit[random]);
        if ((*player).active_weapon->ammo <= 0)
        {
            Continue();
            printf("Your %s is out of ammo, so you attacked with your knife. Change your weapon!\n", (*player).active_weapon->name);
            random = (((*player).weapons[2]->max_damage) - ((*player).weapons[2]->min_damage)) + 1;
            int attack_damage = (*player).weapons[2]->min_damage + rand() % random;
            (*boss_life) -= attack_damage;
        }
        else
        {
            random = (((*player).active_weapon->max_damage) - ((*player).active_weapon->min_damage)) + 1;
            int attack_damage = (*player).active_weapon->min_damage + rand() % random;
            if ((*player).active_weapon != &knife)
            {
                (*player).active_weapon->ammo --;
            }
            (*boss_life) -= attack_damage;
        }
        if((*boss_life) < 0)
        {
            (*boss_alive) = false;
            return;
        }
        else
        {
            printf("Monster's health: %i of %i.\n", *boss_life, boss_life_display);
        }
    }
    else
    {
        int random = rand() % 5;
        printf("\n%s\n", boss_miss[random]);
    }

    // BOSS ATTACK
    printf("\nThe beast is running towards you! Get ready to dodge.\n");
    printf("\n## Type '1' when you're ready: ");
    int input_new = 100;
    while (input_new != 1)
    {
        if (input_new == 1)
        {
            break;
        }
        scanf("%d", &input_new);
    }
    length = 0;
    printf("\n--- 3 ---\n");
    sleepMilliSecond(1000);
    printf("\n--- 2 ---\n");
    sleepMilliSecond(1000);
    printf("\n--- 1 ---\n");
    sleepMilliSecond(1000);
    int random_skull;
    int skull_result;
    random_skull = (rand() % 4) + 1;
    skull_result = Skull(random_skull);
    printf("\n");
    while (1)
    {

        if (kbhit())
        {
            // dodge
            break;
        }
        printf("\033[1;31m-\033[0m");
        sleepMilliSecond(15);
        length ++;
        if (length == 50)
        {
            // hit
            break;
        }
    }
    if (length == skull_result || length == skull_result + 1 || length == skull_result + 2 || length == skull_result + 3)
    {
        int random = rand() % 5;
        printf("\n%s", boss_dodge[random]);
        ContinueOne();
    }
    else
    {
        int random = rand() % 5;
        printf("\n%s", boss_damage[random]);
        // max dmg - min dmg
        random = (50 - 30);
        // min dmg + rand(range of max and min)
        int attack_damage = 30 + rand() % random;
        (*player).health -= attack_damage;
        printf("\nYour health is down to %i\n", (*player).health);
        if ((*player).health <= 0)
        {
            Death(player);
        }
    }
}


// Riddle of running through a maze and dodge the boss if a dead end is reached
void MazeStart(player *player, maze_path *pointer)
{    
    int input = -1;
    printf("\nCurrent Passage: %i\n", pointer->num);
    if (pointer->end_of_path == 1)
    {
        MazeDodge(player);
        printf("\nRUN BACK! ");
        for (int i = 0; i < available_paths_size; i++)
        {
            if (i == 0 && (*pointer).available_paths[i] != NULL)
            {
                printf("\n%i - Back", i);
            }
            else if (i == 1 && (*pointer).available_paths[i] != NULL)
            {
                printf("\n%i - Back", i);
            }
            else if (i == 2 && (*pointer).available_paths[i] != NULL)
            {
                printf("\n%i - Back ", i);
            }
        }
        printf("\n");
        while (input < 0 || input > available_paths_size)
        {
            scanf("%d", &input);
        }
        pointer = pointer->available_paths[input];
        MazeStart(player, pointer);
    }
    // 14 is the Exit
    else if (memcmp(pointer, &paths_array[14], sizeof(maze_path)) == 0)
    {
        MazeFinalFight();
        int boss_life = 300;
        int boss_life_display = 300;
        bool boss_alive = true;
        while (boss_alive == true)
        {
            BossFight(player, &boss_life, &boss_alive, boss_life_display);
        }
        boss_maze = false;
        player->current_room->func(player);
    }
    else
    {
        // Where do you want to go?
        printf("\nRUN! ");
        for (int i = 0; i < available_paths_size; i++)
        {
            if (i == 0 && (*pointer).available_paths[i] != NULL)
            {
                printf("\n%i - Back (Passage: %i)", i, pointer->available_paths[i]->num);
            }
            else if (i == 1 && (*pointer).available_paths[i] != NULL)
            {
                printf("\n%i - Passage %i", i, pointer->available_paths[i]->num);
            }
            else if (i == 2 && (*pointer).available_paths[i] != NULL)
            {
                printf("\n%i - Passage %i", i, pointer->available_paths[i]->num);
            }
        }
        printf("\n");
        while (input < 0 || input > available_paths_size)
        {
            scanf("%d", &input);
        }
        pointer = pointer->available_paths[input];
        MazeStart(player, pointer);
    }
}

// Dodge function for the Maze
void MazeDodge(player *player)
{
    printf("It's a dead end! The beast is running towards you. Get ready to dodge and flee!\n");
    printf("\n## Type '1' when you're ready: ");
    
    int input = 0;
    while (input != 1)
    {
        if (input == 1)
        {
            break;
        }
        scanf("%d", &input);
    }
    int length = 0;
    printf("\n--- 3 ---\n");
    sleepMilliSecond(1000);
    printf("\n--- 2 ---\n");
    sleepMilliSecond(1000);
    printf("\n--- 1 ---\n");
    sleepMilliSecond(1000);
    Skull(9);
    printf("\n");
    while (1)
    {
        if (kbhit())
        {
            // dodge
            break;
        }
        printf("\033[1;31m-\033[0m");
        sleepMilliSecond(15);
        length ++;
        if (length == 50)
        {
            // hit
            break;
        }
    }
    if (length == 18 || length == 19 || length == 20 || length == 21 || length == 22 || length == 23)
    {
        int random = rand() % 5;
        printf("\n%s", boss_dodge[random]);
        ContinueOne();
    }
    else
    {
        int random = rand() % 5;
        printf("\n%s", boss_damage[random]);
        // max dmg - min dmg
        random = (30 - 10);
        // min dmg + rand(range of max and min)
        int attack_damage = 10 + rand() % random;
        (*player).health -= attack_damage;
        printf("\nYour health is down to %i\n", (*player).health);
        if ((*player).health <= 0)
        {
            Death(player);
        }
    }
}

// Creates the maze and it's passages
void CreateMaze(void)
{
    for (int i = 0; i < 16; i++)
    {    
        paths_array[i].num = i;
        paths_array[i].end_of_path = 0;
        for (int j = 0; j < 3; j++)
        {
            paths_array[i].available_paths[j] = NULL;
        }
    }

    // Entrance
    // available_paths[0] == back
    // available_paths[1] == left
    // available_paths[2] == right
    paths_array[0].available_paths[1] = &paths_array[1];

    paths_array[1].available_paths[0] = &paths_array[0];
    paths_array[1].available_paths[1] = &paths_array[2];
    paths_array[1].available_paths[2] = &paths_array[5];

    paths_array[2].available_paths[0] = &paths_array[1];
    paths_array[2].available_paths[1] = &paths_array[3];
    paths_array[2].available_paths[2] = &paths_array[4];

    paths_array[3].available_paths[0] = &paths_array[2];
    paths_array[3].end_of_path = 1;

    paths_array[4].available_paths[0] = &paths_array[2];
    paths_array[4].end_of_path = 1;

    paths_array[5].available_paths[0] = &paths_array[1];
    paths_array[5].available_paths[1] = &paths_array[6];
    paths_array[5].available_paths[2] = &paths_array[7];

    paths_array[6].available_paths[0] = &paths_array[5];
    paths_array[6].end_of_path = 1;

    paths_array[7].available_paths[0] = &paths_array[5];
    paths_array[7].available_paths[1] = &paths_array[8];
    paths_array[7].available_paths[2] = &paths_array[11];

    paths_array[8].available_paths[0] = &paths_array[7];
    paths_array[8].available_paths[1] = &paths_array[9];
    paths_array[8].available_paths[2] = &paths_array[10];

    paths_array[9].available_paths[0] = &paths_array[8];
    paths_array[9].end_of_path = 1;

    paths_array[10].available_paths[0] = &paths_array[8];
    paths_array[10].end_of_path = 1;

    paths_array[11].available_paths[0] = &paths_array[7];
    paths_array[11].available_paths[1] = &paths_array[12];
    paths_array[11].available_paths[2] = &paths_array[13];

    paths_array[12].available_paths[0] = &paths_array[11];
    paths_array[12].end_of_path = 1;

    paths_array[13].available_paths[0] = &paths_array[11];
    paths_array[13].available_paths[1] = &paths_array[14];
    paths_array[13].available_paths[2] = &paths_array[15];

    paths_array[15].available_paths[0] = &paths_array[13];
    paths_array[15].end_of_path = 1;

    // paths_array[14] == exit
}

// print-outs for dodging zombie attacks
int ZombieAttackLine(int num)
{
    if (num == 2)
    {
        printf("\nMMMMMMMMMMMMMMM\033[1;31mMMMM\033[0mXXX\033[1;31mMMMM\033[0mMMMMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMM\033[1;33mOOO\033[0mMMMMM\033[1;33mO\033[0mMMMMM\033[1;33mOOO\033[0mMMMMMMMMMMMM");
        return 13;
    }
    else if (num == 1)
    {
        printf("\nMMMMMMMMMMMMMMMMMM\033[1;31mXXXX\033[0mMMMMMMMMMMMMMMMMMM");
        printf("\nMMMMMMMMMMMMMM\033[1;33mOOOO\033[0mMMMM\033[1;33mOOOO\033[0mMMMMMMMMMMMMMM");
        return 15;
    }
}

// function to test the dodge functions in the beginning of the game
int TestDodge(int num)
{
    // Zombie ATTACK
    printf("\n## Type '1' when you're ready: ");
    int input_new = 100;
    while (input_new != 1)
    {
        if (input_new == 1)
        {
            break;
        }
        scanf("%d", &input_new);
    }
    int length = 0;
    printf("\n--- 3 ---\n");
    sleepMilliSecond(1000);
    printf("\n--- 2 ---\n");
    sleepMilliSecond(1000);
    printf("\n--- 1 ---\n");
    sleepMilliSecond(1000);
    int line;
    line = ZombieAttackLine(1);
    printf("\n");
    while (1)
    {
        if (kbhit())
        {
            // dodge
            break;
        }
        printf("\033[1;31m-\033[0m");
        sleepMilliSecond(15);
        length ++;
        if (length == 50)
        {
            // hit
            break;
        }
    }
    if (length == line || length == line + 1 || length == line + 2 || length == line + 3 || length == line + 8 || length == line + 9 || length == line + 10 || length == line + 11)
    {
        printf("\n\nGood Job! You dodged!\n");
        return 1;
    }
    else
    {
        if (num == 1)
        {
            printf("\n\nYou got hit. Try it again!\n");
            return 2;
        }
        else if (num == 2)
        {
            printf("\n\nWell, let's just hope your skills increase during the game!\n");
            return 3;
        }
    }
}