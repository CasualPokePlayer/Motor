#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

typedef int8_t s8;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

/* Constants */
#define BLOCKS				(4)		// Number of blocks: A, B, C and D
#define BLOCK_PERMS			(24)	// Factorial of the number of blocks
#define BLOCK_SIZE			(16)	// Number of 16-bit words in 128 bytes, the size of Pkmn Block data
#define COND_SIZE			(50)	// Number of 16-bit words in 100 bytes, the size of Pkmn Condition data
#define COND_SIZE_S			(33)	// Utility: stop earlier to avoid needless encryption
#define LANGUAGES_MAX		(9)		// Number of languages: _0, Japanese, English, French, Italian, German, _6, Spanish and Korean
#define VERSIONS_MAX		(3)		// Number of versions: Diamond, Pearl and Platinum
#define ABILITIES_MAX		(124)	// Number of abilities
#define SPECIES_MAX			(494)	// Number of species
#define MOVES_MAX			(468)	// Number of moves
#define ITEMS_MAX			(468)	// Number of items (465 in Diamond and Pearl but it doesn't make a difference)
#define NATURES_MAX			(25)	// Number of natures
#define STATS_MAX			(6)		// Number of stats: Hit Points, Attack, Defense, Speed, Special Attack and Special Defense
#define OWN_MOVES_MAX		(4)		// Maximum number of moves a Pkmn can know at the same time
#define STRING_LENGTH_MAX	(16)	// Utility: 
#define STACK_OFFSET		(4)		// Misalignment between wild and seven
#define PIDS_MAX			(1060)	// Highest amount of frames possible between 2 duplicate PIDs with Method J
#define OG_WILDS_MAX		(9)		// OG_WILDS_MAX
#define ASLR_GROUPS_MAX		(12)	// Number of ASLR groups (mirrors)
#define KOREAN_OFFSET		(0x44)	// Korean RAM quirk
#define HEAPID_MAX			(0x5C)	// Hardcoded game constant. Apparently 0x5B would work too?
#define BALL_ID_MAX			(20)	//

/* Strings of the available game languages by index, 0 and 6 are dummies */
u8 Languages[LANGUAGES_MAX][3] = { "_0", "jp", "en", "fr", "it", "ge", "_6", "sp", "ko" };

/* Strings of the game versions */
u8 Versions[VERSIONS_MAX][9] = { "Diamond", "Pearl", "Platinum" };

/* Offset from the base pointer */
u32 OppPartyOffBeg[VERSIONS_MAX - 1] = { 0x4C7B0, 0x4B884 }; //DP, Pt
u32 OppPartyOffEnd[VERSIONS_MAX - 1] = { 0x4D310, 0x4C3E4 }; //DP, Pt

/* Base pointers depending on ASLR, Language and Version, 1 per group (ignores mirrors)  */
u32 aslr__0_dp[ASLR_GROUPS_MAX] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 }; //dummy
u32 aslr__0_pt[ASLR_GROUPS_MAX] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 }; //dummy
u32 aslr_jp_dp[ASLR_GROUPS_MAX] = { 0x02271940, 0x02271944, 0x02271948, 0x0227194C, 0x02271970, 0x02271974, 0x02271978, 0x0227197C, 0x022719F0, 0x022719F4, 0x022719F8, 0x022719FC };
u32 aslr_jp_pt[ASLR_GROUPS_MAX] = { 0x0227D4F0, 0x0227D4F4, 0x0227D4F8, 0x0227D4FC, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 }; //unavailable
u32 aslr_en_dp[ASLR_GROUPS_MAX] = { 0x0226D2F8, 0x0226D2FC, 0x0226D300, 0x0226D304, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
u32 aslr_en_pt[ASLR_GROUPS_MAX] = { 0x0227E140, 0x0227E144, 0x0227E148, 0x0227E14C, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
u32 aslr_fr_dp[ASLR_GROUPS_MAX] = { 0x0226D5F8, 0x0226D5FC, 0x0226D600, 0x0226D604, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
u32 aslr_fr_pt[ASLR_GROUPS_MAX] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 }; //unavailable
u32 aslr_it_dp[ASLR_GROUPS_MAX] = { 0x0226D500, 0x0226D504, 0x0226D508, 0x0226D50C, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
u32 aslr_it_pt[ASLR_GROUPS_MAX] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 }; //unavailable
u32 aslr_ge_dp[ASLR_GROUPS_MAX] = { 0x0226D500, 0x0226D504, 0x0226D508, 0x0226D50C, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
u32 aslr_ge_pt[ASLR_GROUPS_MAX] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 }; //unavailable
u32 aslr__6_dp[ASLR_GROUPS_MAX] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 }; //dummy
u32 aslr__6_pt[ASLR_GROUPS_MAX] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 }; //dummy
u32 aslr_sp_dp[ASLR_GROUPS_MAX] = { 0x0226D60C, 0x0226D600, 0x0226D604, 0x0226D608, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
u32 aslr_sp_pt[ASLR_GROUPS_MAX] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 }; //unavailable
u32 aslr_ko_dp[ASLR_GROUPS_MAX] = { 0x02274B9C, 0x02274BA0, 0x02274BA4, 0x02274BA8, 0x02274BAC, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
u32 aslr_ko_pt[ASLR_GROUPS_MAX] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 }; //unavailable

/* ASLR table using the grouped version format */
u32* Aslrs[LANGUAGES_MAX][VERSIONS_MAX - 1] = {
  { aslr__0_dp, aslr__0_pt },
  { aslr_jp_dp, aslr_jp_pt },
  { aslr_en_dp, aslr_en_pt },
  { aslr_fr_dp, aslr_fr_pt },
  { aslr_it_dp, aslr_it_pt },
  { aslr_ge_dp, aslr_ge_pt },
  { aslr__6_dp, aslr__6_pt },
  { aslr_sp_dp, aslr_sp_pt },
  { aslr_ko_dp, aslr_ko_pt }
};

/* The 24 ABCD Block permutations (inverse) */
u16 Perms[BLOCK_PERMS] = { 0x0123, 0x0132, 0x0213, 0x0312, 0x0231, 0x0321, 0x1023, 0x1032, 0x2013, 0x3012, 0x2031, 0x3021, 0x1203, 0x1302, 0x2103, 0x3102, 0x2301, 0x3201, 0x1230, 0x1320, 0x2130, 0x3120, 0x2310, 0x3210 };

/* Indices of each stat */
enum { hp = 0, at, df, sp, sa, sd };

/* Table for bonus/malus in each stat depending on Nature, HP omitted */
s8 NatureStatModifiers[NATURES_MAX][STATS_MAX - 1] = { {0, 0, 0, 0, 0}, {1, -1, 0, 0, 0}, {1, 0, -1, 0, 0}, {1, 0, 0, -1, 0}, {1, 0, 0, 0, -1}, {-1, 1, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 1, -1, 0, 0}, {0, 1, 0, -1, 0}, {0, 1, 0, 0, -1}, {-1, 0, 1, 0, 0}, {0, -1, 1, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 1, -1, 0}, {0, 0, 1, 0, -1}, {-1, 0, 0, 1, 0}, {0, -1, 0, 1, 0}, {0, 0, -1, 1, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 1, -1}, {-1, 0, 0, 0, 1}, {0, -1, 0, 0, 1}, {0, 0, -1, 0, 1}, {0, 0, 0, -1, 1}, {0, 0, 0, 0, 0} };

/* Ordered strings of all abilities */
u8 Abilities[ABILITIES_MAX][STRING_LENGTH_MAX] = { "None", "Stench", "Drizzle", "Speed Boost", "Battle Armor", "Sturdy", "Damp", "Limber", "Sand Veil", "Static", "Volt Absorb", "Water Absorb", "Oblivious", "Cloud Nine", "Compound Eyes", "Insomnia", "Color Change", "Immunity", "Flash Fire", "Shield Dust", "Own Tempo", "Suction Cups", "Intimidate", "Shadow Tag", "Rough Skin", "Wonder Guard", "Levitate", "Effect Spore", "Synchronize", "Clear Body", "Natural Cure", "Lightning Rod", "Serene Grace", "Swift Swim", "Chlorophyll", "Illuminate", "Trace", "Huge Power", "Poison Point", "Inner Focus", "Magma Armor", "Water Veil", "Magnet Pull", "Soundproof", "Rain Dish", "Sand Stream", "Pressure", "Thick Fat", "Early Bird", "Flame Body", "Run Away", "Keen Eye", "Hyper Cutter", "Pickup", "Truant", "Hustle", "Cute Charm", "Plus", "Minus", "Forecast", "Sticky Hold", "Shed Skin", "Guts", "Marvel Scale", "Liquid Ooze", "Overgrow", "Blaze", "Torrent", "Swarm", "Rock Head", "Drought", "Arena Trap", "Vital Spirit", "White Smoke", "Pure Power", "Shell Armor", "Air Lock", "Tangled Feet", "Motor Drive", "Rivalry", "Steadfast", "Snow Cloak", "Gluttony", "Anger Point", "Unburden", "Heatproof", "Simple", "Dry Skin", "Download", "Iron Fist", "Poison Heal", "Adaptability", "Skill Link", "Hydration", "Solar Power", "Quick Feet", "Normalize", "Sniper", "Magic Guard", "No Guard", "Stall", "Technician", "Leaf Guard", "Klutz", "Mold Breaker", "Super Luck", "Aftermath", "Anticipation", "Forewarn", "Unaware", "Tinted Lens", "Filter", "Slow Start", "Scrappy", "Storm Drain", "Ice Body", "Solid Rock", "Snow Warning", "Honey Gather", "Frisk", "Reckless", "Multitype", "Flower Gift", "Bad Dreams" };

/* Ordered strings of all species */
u8 Pokelist[SPECIES_MAX][STRING_LENGTH_MAX] = { "anything", "Bulbasaur", "Ivysaur", "Venusaur", "Charmander", "Charmeleon", "Charizard", "Squirtle", "Wartortle", "Blastoise", "Caterpie", "Metapod", "Butterfree", "Weedle", "Kakuna", "Beedrill", "Pidgey", "Pidgeotto", "Pidgeot", "Rattata", "Raticate", "Spearow", "Fearow", "Ekans", "Arbok", "Pikachu", "Raichu", "Sandshrew","Sandslash", "Nidoran-F", "Nidorina", "Nidoqueen", "Nidoran-M", "Nidorino", "Nidoking", "Clefairy", "Clefable", "Vulpix", "Ninetales", "Jigglypuff", "Wigglytuff", "Zubat", "Golbat", "Oddish", "Gloom", "Vileplume", "Paras", "Parasect", "Venonat", "Venomoth", "Diglett", "Dugtrio", "Meowth", "Persian", "Psyduck", "Golduck", "Mankey", "Primeape", "Growlithe", "Arcanine", "Poliwag", "Poliwhirl", "Poliwrath", "Abra", "Kadabra", "Alakazam", "Machop", "Machoke", "Machamp", "Bellsprout", "Weepinbell", "Victreebel", "Tentacool", "Tentacruel", "Geodude", "Graveler", "Golem", "Ponyta", "Rapidash", "Slowpoke", "Slowbro", "Magnemite", "Magneton", "Farfetch'd", "Doduo", "Dodrio", "Seel", "Dewgong", "Grimer", "Muk", "Shellder", "Cloyster", "Gastly", "Haunter", "Gengar", "Onix", "Drowzee", "Hypno", "Krabby", "Kingler", "Voltorb", "Electrode", "Exeggcute", "Exeggutor", "Cubone", "Marowak", "Hitmonlee", "Hitmonchan", "Lickitung", "Koffing", "Weezing", "Rhyhorn", "Rhydon", "Chansey", "Tangela", "Kangaskhan", "Horsea", "Seadra", "Goldeen", "Seaking", "Staryu", "Starmie", "Mr.Mime", "Scyther", "Jynx", "Electabuzz", "Magmar", "Pinsir", "Tauros", "Magikarp", "Gyarados", "Lapras", "Ditto", "Eevee", "Vaporeon", "Jolteon", "Flareon", "Porygon", "Omanyte", "Omastar", "Kabuto", "Kabutops", "Aerodactyl", "Snorlax", "Articuno", "Zapdos", "Moltres", "Dratini", "Dragonair", "Dragonite", "Mewtwo", "Mew", "Chikorita", "Bayleef", "Meganium", "Cyndaquil", "Quilava", "Typhlosion", "Totodile", "Croconaw", "Feraligatr", "Sentret", "Furret", "Hoothoot", "Noctowl", "Ledyba", "Ledian", "Spinarak", "Ariados", "Crobat", "Chinchou", "Lanturn", "Pichu", "Cleffa", "Igglybuff", "Togepi", "Togetic", "Natu", "Xatu", "Mareep", "Flaaffy", "Ampharos", "Bellossom", "Marill", "Azumarill", "Sudowoodo", "Politoed", "Hoppip", "Skiploom", "Jumpluff", "Aipom", "Sunkern", "Sunflora", "Yanma", "Wooper", "Quagsire", "Espeon", "Umbreon", "Murkrow", "Slowking", "Misdreavus", "Unown", "Wobbuffet", "Girafarig", "Pineco", "Forretress", "Dunsparce", "Gligar", "Steelix", "Snubbull", "Granbull", "Qwilfish", "Scizor", "Shuckle", "Heracross", "Sneasel", "Teddiursa", "Ursaring", "Slugma", "Magcargo", "Swinub", "Piloswine", "Corsola", "Remoraid", "Octillery", "Delibird", "Mantine", "Skarmory", "Houndour", "Houndoom", "Kingdra", "Phanpy", "Donphan", "Porygon-2", "Stantler", "Smeargle", "Tyrogue", "Hitmontop", "Smoochum", "Elekid", "Magby", "Miltank", "Blissey", "Raikou", "Entei", "Suicune", "Larvitar", "Pupitar", "Tyranitar", "Lugia", "Ho-Oh", "Celebi", "Treecko", "Grovyle", "Sceptile", "Torchic", "Combusken", "Blaziken", "Mudkip", "Marshtomp", "Swampert", "Poochyena", "Mightyena", "Zigzagoon", "Linoone", "Wurmple", "Silcoon", "Beautifly", "Cascoon", "Dustox", "Lotad", "Lombre", "Ludicolo", "Seedot", "Nuzleaf", "Shiftry", "Taillow", "Swellow", "Wingull", "Pelipper", "Ralts", "Kirlia", "Gardevoir", "Surskit", "Masquerain", "Shroomish", "Breloom", "Slakoth", "Vigoroth", "Slaking", "Nincada", "Ninjask", "Shedinja", "Whismur", "Loudred", "Exploud", "Makuhita", "Hariyama", "Azurill", "Nosepass", "Skitty", "Delcatty", "Sableye", "Mawile", "Aron", "Lairon", "Aggron", "Meditite", "Medicham", "Electrike", "Manectric", "Plusle", "Minun", "Volbeat", "Illumise", "Roselia", "Gulpin", "Swalot", "Carvanha", "Sharpedo", "Wailmer", "Wailord", "Numel", "Camerupt", "Torkoal", "Spoink", "Grumpig", "Spinda", "Trapinch", "Vibrava", "Flygon", "Cacnea", "Cacturne", "Swablu", "Altaria", "Zangoose", "Seviper", "Lunatone", "Solrock", "Barboach", "Whiscash", "Corphish", "Crawdaunt", "Baltoy", "Claydol", "Lileep", "Cradily", "Anorith", "Armaldo", "Feebas", "Milotic", "Castform", "Kecleon", "Shuppet", "Banette", "Duskull", "Dusclops", "Tropius", "Chimecho", "Absol", "Wynaut", "Snorunt", "Glalie", "Spheal", "Sealeo", "Walrein", "Clamperl", "Huntail", "Gorebyss", "Relicanth", "Luvdisc", "Bagon", "Shelgon", "Salamence", "Beldum", "Metang", "Metagross", "Regirock", "Regice", "Registeel", "Latias", "Latios", "Kyogre", "Groudon", "Rayquaza", "Jirachi", "Deoxys", "Turtwig", "Grotle", "Torterra", "Chimchar", "Monferno", "Infernape", "Piplup", "Prinplup", "Empoleon", "Starly", "Staravia", "Staraptor", "Bidoof", "Bibarel", "Kricketot", "Kricketune", "Shinx", "Luxio", "Luxray", "Budew", "Roserade", "Cranidos", "Rampardos", "Shieldon", "Bastiodon", "Burmy", "Wormadam", "Mothim", "Combee", "Vespiquen", "Pachirisu", "Buizel", "Floatzel", "Cherubi", "Cherrim", "Shellos", "Gastrodon", "Ambipom", "Drifloon", "Drifblim", "Buneary", "Lopunny", "Mismagius", "Honchkrow", "Glameow", "Purugly", "Chingling", "Stunky", "Skuntank", "Bronzor", "Bronzong", "Bonsly", "MimeJr.", "Happiny", "Chatot", "Spiritomb", "Gible", "Gabite", "Garchomp", "Munchlax", "Riolu", "Lucario", "Hippopotas", "Hippowdon", "Skorupi", "Drapion", "Croagunk", "Toxicroak", "Carnivine", "Finneon", "Lumineon", "Mantyke", "Snover", "Abomasnow", "Weavile", "Magnezone", "Lickilicky", "Rhyperior", "Tangrowth", "Electivire", "Magmortar", "Togekiss", "Yanmega", "Leafeon", "Glaceon", "Gliscor", "Mamoswine", "Porygon-Z", "Gallade", "Probopass", "Dusknoir", "Froslass", "Rotom", "Uxie", "Mesprit", "Azelf", "Dialga", "Palkia", "Heatran", "Regigigas", "Giratina", "Cresselia", "Phione", "Manaphy", "Darkrai", "Shaymin", "Arceus" };

/* Ordered strings of all items */
u8 Items[ITEMS_MAX][STRING_LENGTH_MAX] = { "anything", "Master Ball", "Ultra Ball", "Great Ball", "Poké Ball", "Safari Ball", "Net Ball", "Dive Ball", "Nest Ball", "Repeat Ball", "Timer Ball", "Luxury Ball", "Premier Ball", "Dusk Ball", "Heal Ball", "Quick Ball", "Cherish Ball", "Potion", "Antidote", "Burn Heal", "Ice Heal", "Awakening", "Paralyze Heal", "Full Restore", "Max Potion", "Hyper Potion", "Super Potion", "Full Heal", "Revive", "Max Revive", "Fresh Water", "Soda Pop", "Lemonade", "Moomoo Milk", "Energy Powder", "Energy Root", "Heal Powder", "Revival Herb", "Ether", "Max Ether", "Elixir", "Max Elixir", "Lava Cookie", "Berry Juice", "Sacred Ash", "HP Up", "Protein", "Iron", "Carbos", "Calcium", "Rare Candy", "PP Up", "Zinc", "PP Max", "Old Gateau", "Guard Spec", "Dire Hit", "X Attack", "X Defense", "X Speed", "X Accuracy", "X SpeAtk", "X SpeDef", "Poké Doll", "Fluffy Tail", "Blue Flute", "Yellow Flute", "Red Flute", "Black Flute", "White Flute", "Shoal Salt", "Shoal Shell", "Red Shard", "Blue Shard", "Yellow Shard", "Green Shard", "Super Repel", "Max Repel", "Escape Rope", "Repel", "Sun Stone", "Moon Stone", "Fire Stone", "Thunder Stone", "Water Stone", "Leaf Stone", "Tiny Mushroom", "Big Mushroom", "Pearl", "Big Pearl", "Stardust", "Star Piece", "Nugget", "Heart Scale", "Honey", "Growth Mulch", "Damp Mulch", "Stable Mulch", "Gooey Mulch", "Root Fossil", "Claw Fossil", "Helix Fossil", "Dome Fossil", "Old Amber", "Armor Fossil", "Skull Fossil", "Rare Bone", "Shiny Stone", "Dusk Stone", "Dawn Stone", "Oval Stone", "Odd Keystone", "Griseous Orb", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Unknown Item", "Adamant Orb", "Lustrous Orb", "Grass Mail", "Flame Mail", "Bubble Mail", "Bloom Mail", "Tunnel Mail", "Steel Mail", "Heart Mail", "Snow Mail", "Space Mail", "Air Mail", "Mosaic Mail", "Brick Mail", "Cheri Berry", "Chesto Berry", "Pecha Berry", "Rawst Berry", "Aspear Berry", "Leppa Berry", "Oran Berry", "Persim Berry", "Lum Berry", "Sitrus Berry", "Figy Berry", "Wiki Berry", "Mago Berry", "Aguav Berry", "Iapapa Berry", "Razz Berry", "Bluk Berry", "Nanab Berry", "Wepear Berry", "Pinap Berry", "Pomeg Berry", "Kelpsy Berry", "Qualot Berry", "Hondew Berry", "Grepa Berry", "Tamato Berry", "Cornn Berry", "Magost Berry", "Rabuta Berry", "Nomel Berry", "Spelon Berry", "Pamtre Berry", "Watmel Berry", "Durin Berry", "Belue Berry", "Occa Berry", "Passho Berry", "Wacan Berry", "Rindo Berry", "Yache Berry", "Chople Berry", "Kebia Berry", "Shuca Berry", "Coba Berry", "Payapa Berry", "Tanga Berry", "Charti Berry", "Kasib Berry", "Haban Berry", "Colbur Berry", "Babiri Berry", "Chilan Berry", "Liechi Berry", "Ganlon Berry", "Salac Berry", "Petaya Berry", "Apicot Berry", "Lansat Berry", "Starf Berry", "Enigma Berry", "Micle Berry", "Custap Berry", "Jaboca Berry", "Rowap Berry", "Bright Powder", "White Herb", "Macho Brace", "Exp Share", "Quick Claw", "Soothe Bell", "Mental Herb", "Choice Band", "King's Rock", "Silver Powder", "Amulet Coin", "Cleanse Tag", "Soul Dew", "Deep Sea Tooth", "Deep Sea Scale", "Smoke Ball", "Everstone", "Focus Band", "Lucky Egg", "Scope Lens", "Metal Coat", "Leftovers", "Dragon Scale", "Light Ball", "Soft Sand", "Hard Stone", "Miracle Seed", "Black Glasses", "Black Belt", "Magnet", "Mystic Water", "Sharp Beak", "Poison Barb", "Never-Melt Ice", "Spell Tag", "Twisted Spoon", "Charcoal", "Dragon Fang", "Silk Scarf", "Up-Grade", "Shell Bell", "Sea Incense", "Lax Incense", "Lucky Punch", "Metal Powder", "Thick Club", "Stick", "Red Scarf", "Blue Scarf", "Pink Scarf", "Green Scarf", "Yellow Scarf", "Wide Lens", "Muscle Band", "Wise Glasses", "Expert Belt", "Light Clay", "Life Orb", "Power Herb", "Toxic Orb", "Flame Orb", "Quick Powder", "Focus Sash", "Zoom Lens", "Metronome", "Iron Ball", "Lagging Tail", "Destiny Knot", "Black Sludge", "Icy Rock", "Smooth Rock", "Heat Rock", "Damp Rock", "Grip Claw", "Choice Scarf", "Sticky Barb", "Power Bracer", "Power Belt", "Power Lens", "Power Band", "Power Anklet", "Power Weight", "Shed Shell", "Big Root", "Choice Specs", "Flame Plate", "Splash Plate", "Zap Plate", "Meadow Plate", "Icicle Plate", "Fist Plate", "Toxic Plate", "Earth Plate", "Sky Plate", "Mind Plate", "Insect Plate", "Stone Plate", "Spooky Plate", "Draco Plate", "Dread Plate", "Iron Plate", "Odd Incense", "Rock Incense", "Full Incense", "Wave Incense", "Rose Incense", "Luck Incense", "Pure Incense", "Protector", "Electirizer", "Magmarizer", "Dubious Disc", "Reaper Cloth", "Razor Claw", "Razor Fang", "TM01", "TM02", "TM03", "TM04", "TM05", "TM06", "TM07", "TM08", "TM09", "TM10", "TM11", "TM12", "TM13", "TM14", "TM15", "TM16", "TM17", "TM18", "TM19", "TM20", "TM21", "TM22", "TM23", "TM24", "TM25", "TM26", "TM27", "TM28", "TM29", "TM30", "TM31", "TM32", "TM33", "TM34", "TM35", "TM36", "TM37", "TM38", "TM39", "TM40", "TM41", "TM42", "TM43", "TM44", "TM45", "TM46", "TM47", "TM48", "TM49", "TM50", "TM51", "TM52", "TM53", "TM54", "TM55", "TM56", "TM57", "TM58", "TM59", "TM60", "TM61", "TM62", "TM63", "TM64", "TM65", "TM66", "TM67", "TM68", "TM69", "TM70", "TM71", "TM72", "TM73", "TM74", "TM75", "TM76", "TM77", "TM78", "TM79", "TM80", "TM81", "TM82", "TM83", "TM84", "TM85", "TM86", "TM87", "TM88", "TM89", "TM90", "TM91", "TM92", "HM01", "HM02", "HM03", "HM04", "HM05", "HM06", "HM07", "HM08", "Explorer Kit", "Loot Sack", "Rule Book", "Poké Radar", "Point Card", "Journal", "Seal Case", "Fashion Case", "Seal", "Pal Pad", "Works Key", "Old Charm", "Galactic Key", "Red Chain", "Town Map", "VS Seeker", "Coin Case", "Old Rod", "Good Rod", "Super Rod", "Sprayduck", "Poffin Case", "Bicycle", "Suite Key", "Oak's Letter", "Lunar Wing", "Member Card", "Azure Flute", "S.S. Ticket", "Contest Pass", "Magma Stone", "Parcel", "Coupon 1", "Coupon 2", "Coupon 3", "Storage Key", "Secret Potion", "VS Recorder", "Gracidea", "Secret Key" };

/* Ordered strings of all moves */
u8 Moves[MOVES_MAX][STRING_LENGTH_MAX] = { "None" , "Pound", "Karate Chop", "Double Slap", "Comet Punch", "Mega Punch", "Pay Day", "Fire Punch", "Ice Punch", "Thunder Punch", "Scratch", "Vice Grip", "Guillotine", "Razor Wind", "Swords Dance", "Cut", "Gust", "Wing Attack", "Whirlwind", "Fly", "Bind", "Slam", "Vine Whip", "Stomp", "Double Kick", "Mega Kick", "Jump Kick", "Rolling Kick", "Sand Attack", "Headbutt", "Horn Attack", "Fury Attack", "Horn Drill", "Tackle", "Body Slam", "Wrap", "Take Down", "Thrash", "Double-Edge", "Tail Whip", "Poison Sting", "Twineedle", "Pin Missile", "Leer", "Bite", "Growl", "Roar", "Sing", "Supersonic", "Sonic Boom", "Disable", "Acid", "Ember", "Flamethrower", "Mist", "Water Gun", "Hydro Pump", "Surf", "Ice Beam", "Blizzard", "Psybeam", "Bubble Beam", "Aurora Beam", "Hyper Beam", "Peck", "Drill Peck", "Submission", "Low Kick", "Counter", "Seismic Toss", "Strength", "Absorb", "Mega Drain", "Leech Seed", "Growth", "Razor Leaf", "Solar Beam", "Poison Powder", "Stun Spore", "Sleep Powder", "Petal Dance", "String Shot", "Dragon Rage", "Fire Spin", "Thunder Shock", "Thunderbolt", "Thunder Wave", "Thunder", "Rock Throw", "Earthquake", "Fissure", "Dig", "Toxic", "Confusion", "Psychic", "Hypnosis", "Meditate", "Agility", "Quick Attack", "Rage", "Teleport", "Night Shade", "Mimic", "Screech", "Double Team", "Recover", "Harden", "Minimize", "Smokescreen", "Confuse Ray", "Withdraw", "Defense Curl", "Barrier", "Light Screen", "Haze", "Reflect", "Focus Energy", "Bide", "Metronome", "Mirror Move", "Self-Destruct", "Egg Bomb", "Lick", "Smog", "Sludge", "Bone Club", "Fire Blast", "Waterfall", "Clamp", "Swift", "Skull Bash", "Spike Cannon", "Constrict", "Amnesia", "Kinesis", "Soft-Boiled", "High Jump Kick", "Glare", "Dream Eater", "Poison Gas", "Barrage", "Leech Life", "Lovely Kiss", "Sky Attack", "Transform", "Bubble", "Dizzy Punch", "Spore", "Flash", "Psywave", "Splash", "Acid Armor", "Crabhammer", "Explosion", "Fury Swipes", "Bonemerang", "Rest", "Rock Slide", "Hyper Fang", "Sharpen", "Conversion", "Tri Attack", "Super Fang", "Slash", "Substitute", "Struggle", "Sketch", "Triple Kick", "Thief", "Spider Web", "Mind Reader", "Nightmare", "Flame Wheel", "Snore", "Curse", "Flail", "Conversion 2", "Aeroblast", "Cotton Spore", "Reversal", "Spite", "Powder Snow", "Protect", "Mach Punch", "Scary Face", "Feint Attack", "Sweet Kiss", "Belly Drum", "Sludge Bomb", "Mud-Slap", "Octazooka", "Spikes", "Zap Cannon", "Foresight", "Destiny Bond", "Perish Song", "Icy Wind", "Detect", "Bone Rush", "Lock-On", "Outrage", "Sandstorm", "Giga Drain", "Endure", "Charm", "Rollout", "False Swipe", "Swagger", "Milk Drink", "Spark", "Fury Cutter", "Steel Wing", "Mean Look", "Attract", "Sleep Talk", "Heal Bell", "Return", "Present", "Frustration", "Safeguard", "Pain Split", "Sacred Fire", "Magnitude", "Dynamic Punch", "Megahorn", "Dragon Breath", "Baton Pass", "Encore", "Pursuit", "Rapid Spin", "Sweet Scent", "Iron Tail", "Metal Claw", "Vital Throw", "Morning Sun", "Synthesis", "Moonlight", "Hidden Power", "Cross Chop", "Twister", "Rain Dance", "Sunny Day", "Crunch", "Mirror Coat", "Psych Up", "Extreme Speed", "Ancient Power", "Shadow Ball", "Future Sight", "Rock Smash", "Whirlpool", "Beat Up", "Fake Out", "Uproar", "Stockpile", "Spit Up", "Swallow", "Heat Wave", "Hail", "Torment", "Flatter", "Will-O-Wisp", "Memento", "Facade", "Focus Punch", "Smelling Salts", "Follow Me", "Nature Power", "Charge", "Taunt", "Helping Hand", "Trick", "Role Play", "Wish", "Assist", "Ingrain", "Superpower", "Magic Coat", "Recycle", "Revenge", "Brick Break", "Yawn", "Knock Off", "Endeavor", "Eruption", "Skill Swap", "Imprison", "Refresh", "Grudge", "Snatch", "Secret Power", "Dive", "Arm Thrust", "Camouflage", "Tail Glow", "Luster Purge", "Mist Ball", "Feather Dance", "Teeter Dance", "Blaze Kick", "Mud Sport", "Ice Ball", "Needle Arm", "Slack Off", "Hyper Voice", "Poison Fang", "Crush Claw", "Blast Burn", "Hydro Cannon", "Meteor Mash", "Astonish", "Weather Ball", "Aromatherapy", "Fake Tears", "Air Cutter", "Overheat", "Odor Sleuth", "Rock Tomb", "Silver Wind", "Metal Sound", "Grass Whistle", "Tickle", "Cosmic Power", "Water Spout", "Signal Beam", "Shadow Punch", "Extrasensory", "Sky Uppercut", "Sand Tomb", "Sheer Cold", "Muddy Water", "Bullet Seed", "Aerial Ace", "Icicle Spear", "Iron Defense", "Block", "Howl", "Dragon Claw", "Frenzy Plant", "Bulk Up", "Bounce", "Mud Shot", "Poison Tail", "Covet", "Volt Tackle", "Magical Leaf", "Water Sport", "Calm Mind", "Leaf Blade", "Dragon Dance", "Rock Blast", "Shock Wave", "Water Pulse", "Doom Desire", "Psycho Boost", "Roost", "Gravity", "Miracle Eye", "Wake-Up Slap", "Hammer Arm", "Gyro Ball", "Healing Wish", "Brine", "Natural Gift", "Feint", "Pluck", "Tailwind", "Acupressure", "Metal Burst", "U-turn", "Close Combat", "Payback", "Assurance", "Embargo", "Fling", "Psycho Shift", "Trump Card", "Heal Block", "Wring Out", "Power Trick", "Gastro Acid", "Lucky Chant", "Me First", "Copycat", "Power Swap", "Guard Swap", "Punishment", "Last Resort", "Worry Seed", "Sucker Punch", "Toxic Spikes", "Heart Swap", "Aqua Ring", "Magnet Rise", "Flare Blitz", "Force Palm", "Aura Sphere", "Rock Polish", "Poison Jab", "Dark Pulse", "Night Slash", "Aqua Tail", "Seed Bomb", "Air Slash", "X-Scissor", "Bug Buzz", "Dragon Pulse", "Dragon Rush", "Power Gem", "Drain Punch", "Vacuum Wave", "Focus Blast", "Energy Ball", "Brave Bird", "Earth Power", "Switcheroo", "Giga Impact", "Nasty Plot", "Bullet Punch", "Avalanche", "Ice Shard", "Shadow Claw", "Thunder Fang", "Ice Fang", "Fire Fang", "Shadow Sneak", "Mud Bomb", "Psycho Cut", "Zen Headbutt", "Mirror Shot", "Flash Cannon", "Rock Climb", "Defog", "Trick Room", "Draco Meteor", "Discharge", "Lava Plume", "Leaf Storm", "Power Whip", "Rock Wrecker", "Cross Poison", "Gunk Shot", "Iron Head", "Magnet Bomb", "Stone Edge", "Captivate", "Stealth Rock", "Grass Knot", "Chatter", "Judgment", "Bug Bite", "Charge Beam", "Wood Hammer", "Aqua Jet", "Attack Order", "Defend Order", "Heal Order", "Head Smash", "Double Hit", "Roar of Time", "Spacial Rend", "Lunar Dance", "Crush Grip", "Magma Storm", "Dark Void", "Seed Flare", "Ominous Wind", "Shadow Force" };

/* Strings used for appending to Results.txt's file name */
u8 OgWilds[VERSIONS_MAX - 1][OG_WILDS_MAX][STRING_LENGTH_MAX] = {
	{ "Giratina", "Arceus", "Dialga", "Palkia", "Shaymin", "Darkrai", "Uxie", "Azelf", "Rotom" }, //Diamond and Pearl
	{ "Giratina-O", "Giratina-A", "Dialga", "Palkia", "Uxie", "Azelf", "Rotom", "OGW_PT_7", "OGW_PT_8" } //Platinum
};

typedef struct {
	u8 order;
	u8 pos_a;
	u8 pos_b;
	u8 pos_c;
	u8 pos_d;
	u8 nature;
	u8 ivs[STATS_MAX];
	u16 iv1;
	u16 iv2;
	//Main pkmn data; ordered the same as in game to simulate the buffer overflow
	u32 pid;
	u16 bef;
	u16 checksum;
	u16 data[BLOCKS][BLOCK_SIZE];
	u16 cond[COND_SIZE];
	//Size: 252 bytes
} Pkmn;

typedef struct {
	u32 version;
	u32 language;
	u32 tid;
	u32 sid;
	u32 seed;
	u32 frames;
	u32 species;
	u32 item;
	u32 move;
	u32 aslr;
	u32 dupe;
	//Size: 44 bytes
} User;

/* The original wild Pkmn's unique characteristics */
typedef struct {
	u16 species;
	u16 item;
	u16 level;
	u16 bstats[STATS_MAX];
	u16 xp1;
	u16 xp2;
	u16 frab; //frienship and ability concatenated
	u16 moves[OWN_MOVES_MAX];
	u16 pp1and2;
	u16 pp3and4;
	u16 name[11];
	u16 gfx[8]; //remains of graphical data from the current map
	//Size: 74 bytes
} Original;

/* Every static encounters */
Original dp_giratina = { 0x01E7, 0, 70, {150, 100, 120, 90, 100, 120}, 0x8ACE, 0x0006, 0x2E00, {0x01D3, 0x0179, 0x019E, 0x00A3}, 0x0F05, 0x140A, {0x0131, 0x0133, 0x013C, 0x012B, 0x013E, 0x0133, 0x0138, 0x012B, 0xffff, 0, 0}, {0x3377, 0x1463, 0x9631, 0x7779, 0x3377, 0x1463, 0x7605, 0x7777} };
Original dp_arceus = { 0x01ED, 0, 80, {120, 120, 120, 120, 120, 120}, 0xC400, 0x0009, 0x7900, {0x011F, 0x00F8, 0x0069, 0x003F}, 0x0F14, 0x050A, {0x012B, 0x013C, 0x012D, 0x012F, 0x013F, 0x013D, 0xFFFF, 0, 0, 0, 0}, {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7205, 0x6565} };
Original dp_shaymin = { 0x01EC, 0x009D, 30, {100, 100, 100, 100, 100, 100}, 0x5500, 0, 0x1E64, {0x006F, 0x0159, 0x0049, 0x00EB}, 0x1428, 0x050A, {0x013D, 0x0132, 0x012B, 0x0143, 0x0137, 0x0133, 0x0138, 0xFFFF, 0, 0, 0}, {0x6872, 0x6E61, 0x0061, 0x0000, 0x0000, 0x0000, 0x0005, 0x0000} };
Original dp_darkrai = { 0x01EB, 0, 40, {70, 90, 90, 125, 90, 135}, 0x3880, 0x0001, 0x7B00, {0x0062, 0x005F, 0x00E4, 0x00AB}, 0x141E, 0x0F14, {0x012E, 0x012B, 0x013C, 0x0135, 0x013C, 0x012B, 0x0133, 0xFFFF, 0, 0, 0}, {0x0000, 0x0000, 0x0000, 0x0000, 0x3364, 0x735F, 0x3105, 0x625F} };
Original dp_uxie = { 0x01E0, 0, 50, {75, 75, 130, 95, 75, 130}, 0x625A, 0x0002, 0x1A8C, {0x005D, 0x0119, 0x00F8, 0x0085}, 0x0A19, 0x140F, {0x013F, 0x0142, 0x0133, 0x012F, 0xFFFF, 0, 0, 0, 0, 0, 0}, {0x0015, 0x3000, 0x1000, 0x0000, 0x0015, 0x3000, 0x1005, 0x0000} };
Original dp_azelf = { 0x01E2, 0, 50, {75, 125, 70, 115, 70, 125}, 0x625A, 0x0002, 0x1A8C, {0x005D, 0x00FD, 0x00F8, 0x01A1}, 0x0A19, 0x140F, {0x012B, 0x0144, 0x012F, 0x0136, 0x0130, 0xFFFF, 0, 0, 0, 0, 0}, {0x0015, 0x3000, 0x1000, 0x0000, 0x0015, 0x3000, 0x1005, 0x0000} };
Original dp_rotom = { 0x01DF, 0, 15, {50, 50, 77, 91, 95, 77}, 0x0D2F, 0x0000, 0x1A46, {0x0054, 0x006D, 0x00FD, 0x0068}, 0x0A1E, 0x0F0A, {0x013C, 0x0139, 0x013E, 0x0139, 0x0137, 0xFFFF, 0, 0, 0, 0, 0}, {0x0000, 0x0000, 0x0000, 0x0000, 0x0100, 0x0100, 0x0005, 0x002C} };
Original dp_crehelf = { 0x01E0, 0, 50, {75, 75, 130, 95, 75, 130}, 0x625A, 0x0002, 0x1A8C, {0x005D, 0x0119, 0x00F8, 0x0085}, 0x0A19, 0x140F, {0x012D, 0x013C, 0x012F, 0x0132, 0x012F, 0x0136, 0x0130, 0xFFFF, 0, 0, 0}, {0x0015, 0x3000, 0x1000, 0x0000, 0x0015, 0x3000, 0x1005, 0x0000} };
Original dp_crefadet = { 0x01E2, 0, 50, {75, 125, 70, 115, 70, 125}, 0x625A, 0x0002, 0x1A8C, {0x005D, 0x00FD, 0x00F8, 0x01A1}, 0x0A19, 0x140F, {0x012D, 0x013C, 0x012F, 0x0130, 0x012B, 0x012E, 0x012F, 0x013E, 0xFFFF, 0, 0}, {0x0015, 0x3000, 0x1000, 0x0000, 0x0015, 0x3000, 0x1005, 0x0000} };
Original dp_motisma = { 0x01DF, 0, 15, {50, 50, 77, 91, 95, 77}, 0x0D2F, 0x0000, 0x1A46, {0x0054, 0x006D, 0x00FD, 0x0068}, 0x0A1E, 0x0F0A, {0x0137, 0x0139, 0x013E, 0x0133, 0x013D, 0x0137, 0x012B, 0xFFFF, 0, 0, 0}, {0x0000, 0x0000, 0x0000, 0x0000, 0x0100, 0x0100, 0x0005, 0x002C} };
Original dp_selfe = { 0x01E0, 0, 50, {75, 75, 130, 95, 75, 130}, 0x625A, 0x0002, 0x1A8C, {0x005D, 0x0119, 0x00F8, 0x0085}, 0x0A19, 0x140F, {0x013D, 0x012F, 0x0136, 0x0130, 0x012F, 0xFFFF, 0, 0, 0, 0, 0}, {0x0015, 0x3000, 0x1000, 0x0000, 0x0015, 0x3000, 0x1005, 0x0000} };
Original dp_tobutz = { 0x01E2, 0, 50, {75, 125, 70, 115, 70, 125}, 0x625A, 0x0002, 0x1A8C, {0x005D, 0x00FD, 0x00F8, 0x01A1}, 0x0A19, 0x140F, {0x013E, 0x0139, 0x012C, 0x013F, 0x013E, 0x0144, 0xFFFF, 0, 0, 0, 0}, {0x0015, 0x3000, 0x1000, 0x0000, 0x0015, 0x3000, 0x1005, 0x0000} };
Original dp_GIRATEiNA = { 0x01E7, 0, 70, {150, 100, 120, 90, 100, 120}, 0x8ACE, 0x0006, 0x2E00, {0x01D3, 0x0179, 0x019E, 0x00A3}, 0x0F05, 0x140A, {0x005F, 0x009A, 0x0077, 0x0054, 0x007B, 0xFFFF, 0, 0, 0, 0, 0}, {0x3377, 0x1463, 0x9631, 0x7779, 0x3377, 0x1463, 0x7605, 0x7777} };
Original dp_ARUSEUSU = { 0x01ED, 0, 80, {120, 120, 120, 120, 120, 120}, 0xC400, 0x0009, 0x7900, {0x011F, 0x00F8, 0x0069, 0x003F}, 0x0F14, 0x050A, {0x0053, 0x009C, 0x006C, 0x0057, 0x006A, 0xFFFF, 0, 0, 0, 0, 0}, {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7205, 0x6565} };
Original dp_SHIeIMI = { 0x01EC, 0x009D, 30, {100, 100, 100, 100, 100, 100}, 0x5500, 0, 0x1E64, {0x006F, 0x0159, 0x0049, 0x00EB}, 0x1428, 0x050A, {0x0068, 0x0058, 0x0055, 0x0090, 0xFFFF, 0, 0, 0, 0, 0, 0}, {0x6872, 0x6E61, 0x0061, 0x0000, 0x0000, 0x0000, 0x0005, 0x0000} };
Original dp_DAAKURAI = { 0x01EB, 0, 40, {70, 90, 90, 125, 90, 135}, 0x3880, 0x0001, 0x7B00, {0x0062, 0x005F, 0x00E4, 0x00AB}, 0x141E, 0x0F14, {0x0071, 0x00F1, 0x0060, 0x009A, 0x0055, 0xFFFF, 0, 0, 0, 0, 0}, {0x0000, 0x0000, 0x0000, 0x0000, 0x3364, 0x735F, 0x3105, 0x625F} };
Original dp_YUKUSHII = { 0x01E0, 0, 50, {75, 75, 130, 95, 75, 130}, 0x625A, 0x0002, 0x1A8C, {0x005D, 0x0119, 0x00F8, 0x0085}, 0x0A19, 0x140F, {0x0097, 0x0060, 0x0068, 0x00F1, 0xFFFF, 0, 0, 0, 0, 0, 0}, {0x0015, 0x3000, 0x1000, 0x0000, 0x0015, 0x3000, 0x1005, 0x0000} };
Original dp_AGUNOMU = { 0x01E2, 0, 50, {75, 125, 70, 115, 70, 125}, 0x625A, 0x0002, 0x1A8C, {0x005D, 0x00FD, 0x00F8, 0x01A1}, 0x0A19, 0x140F, {0x0053, 0x0061, 0x007F, 0x0091, 0xFFFF, 0, 0, 0, 0, 0, 0}, {0x0015, 0x3000, 0x1000, 0x0000, 0x0015, 0x3000, 0x1005, 0x0000} };
Original dp_ROTOMU = { 0x01DF, 0, 15, {50, 50, 77, 91, 95, 77}, 0x0D2F, 0x0000, 0x1A46, {0x0054, 0x006D, 0x00FD, 0x0068}, 0x0A1E, 0x0F0A, {0x013C, 0x0139, 0x013E, 0x0139, 0x0137, 0xFFFF, 0, 0, 0, 0, 0}, {0x0000, 0x0000, 0x0000, 0x0000, 0x0100, 0x0100, 0x0005, 0x002C} };
Original dp_giratina_ko = { 0x01E7, 0, 70, {150, 100, 120, 90, 100, 120}, 0x8ACE, 0x0006, 0x2E00, {0x01D3, 0x0179, 0x019E, 0x00A3}, 0x0F05, 0x140A, {0x04A0, 0x0687, 0x0C30, 0x0524, 0xFFFF, 0, 0, 0, 0, 0, 0}, {0x3377, 0x1463, 0x9631, 0x7779, 0x3377, 0x1463, 0x7605, 0x7777} };
Original dp_arceus_ko = { 0x01ED, 0, 80, {120, 120, 120, 120, 120, 120}, 0xC400, 0x0009, 0x7900, {0x011F, 0x00F8, 0x0069, 0x003F}, 0x0F14, 0x050A, {0x012B, 0x013C, 0x012D, 0x012F, 0x013F, 0x013D, 0xFFFF, 0, 0, 0, 0}, {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7205, 0x6565} };
Original dp_shaymin_ko = { 0x01EC, 0x009D, 30, {100, 100, 100, 100, 100, 100}, 0x5500, 0, 0x1E64, {0x006F, 0x0159, 0x0049, 0x00EB}, 0x1428, 0x050A, {0x013D, 0x0132, 0x012B, 0x0143, 0x0137, 0x0133, 0x0138, 0xFFFF, 0, 0, 0}, {0x6872, 0x6E61, 0x0061, 0x0000, 0x0000, 0x0000, 0x0005, 0x0000} };
Original dp_darkrai_ko = { 0x01EB, 0, 40, {70, 90, 90, 125, 90, 135}, 0x3880, 0x0001, 0x7B00, {0x0062, 0x005F, 0x00E4, 0x00AB}, 0x141E, 0x0F14, {0x012E, 0x012B, 0x013C, 0x0135, 0x013C, 0x012B, 0x0133, 0xFFFF, 0, 0, 0}, {0x0000, 0x0000, 0x0000, 0x0000, 0x3364, 0x735F, 0x3105, 0x625F} };
Original dp_uxie_ko = { 0x01E0, 0, 50, {75, 75, 130, 95, 75, 130}, 0x625A, 0x0002, 0x1A8C, {0x005D, 0x0119, 0x00F8, 0x0085}, 0x0A19, 0x140F, {0x013F, 0x0142, 0x0133, 0x012F, 0xFFFF, 0, 0, 0, 0, 0, 0}, {0x0015, 0x3000, 0x1000, 0x0000, 0x0015, 0x3000, 0x1005, 0x0000} };
Original dp_azelf_ko = { 0x01E2, 0, 50, {75, 125, 70, 115, 70, 125}, 0x625A, 0x0002, 0x1A8C, {0x005D, 0x00FD, 0x00F8, 0x01A1}, 0x0A19, 0x140F, {0x012B, 0x0144, 0x012F, 0x0136, 0x0130, 0xFFFF, 0, 0, 0, 0, 0}, {0x0015, 0x3000, 0x1000, 0x0000, 0x0015, 0x3000, 0x1005, 0x0000} };
Original dp_rotom_ko = { 0x01DF, 0, 15, {50, 50, 77, 91, 95, 77}, 0x0D2F, 0x0000, 0x1A46, {0x0054, 0x006D, 0x00FD, 0x0068}, 0x0A1E, 0x0F0A, {0x06C0, 0x0BFA, 0x0759, 0xFFFF, 0, 0, 0, 0, 0, 0, 0}, {0x0000, 0x0000, 0x0000, 0x0000, 0x0100, 0x0100, 0x0005, 0x002C} };
Original dp_dialga = { 0x01E3, 0, 47, {100, 120, 120, 90, 150, 100}, 0xFAF2, 0x0001, 0x2E00, {0x00E8, 0x00F6, 0x0151, 0x01CB}, 0x0523, 0x050F, {0x012E, 0x0133, 0x012B, 0x0136, 0x0131, 0x012B, 0xFFFF, 0, 0, 0, 0}, {0x10E4, 0x0000, 0x0217, 0x4300, 0x4652, 0x0000, 0x9D05, 0x0000} };
Original dp_palkia = { 0x01E4, 0, 47, {90, 120, 100, 100, 150, 120}, 0xFAF2, 0x0001, 0x2E00, {0x0160, 0x00F6, 0x0151, 0x01CC}, 0x0514, 0x050F, {0x013A, 0x012B, 0x0136, 0x0135, 0x0133, 0x012B, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000}, {0x10E4, 0x0000, 0x0217, 0x4300, 0x4652, 0x0000, 0x9D05, 0x0000} };
Original dp_DIARUGA = { 0x01E3, 0, 47, {100, 120, 120, 90, 150, 100}, 0xFAF2, 0x0001, 0x2E00, {0x00E8, 0x00F6, 0x0151, 0x01CB}, 0x0523, 0x050F, {0x0078, 0x0054, 0x0053, 0x009C, 0x005D, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}, {0x10E4, 0x0000, 0x0217, 0x4300, 0x4652, 0x0000, 0x9D05, 0x0000} };
Original dp_PARUKIA = { 0x01E4, 0, 47, {90, 120, 100, 100, 150, 120}, 0xFAF2, 0x0001, 0x2E00, {0x0160, 0x00F6, 0x0151, 0x01CC}, 0x0514, 0x050F, {0x0082, 0x009C, 0x005E, 0x0053, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}, {0x10E4, 0x0000, 0x0217, 0x4300, 0x4652, 0x0000, 0x9D05, 0x0000} };
Original dp_dialga_ko = { 0x01E3, 0, 47, {100, 120, 120, 90, 150, 100}, 0xFAF2, 0x0001, 0x2E00, {0x00E8, 0x00F6, 0x0151, 0x01CB}, 0x0523, 0x050F, {0x0626, 0x094A, 0x06D9, 0x0401, 0xFFFF, 0, 0, 0, 0, 0, 0}, {0x10E4, 0x0000, 0x0217, 0x4300, 0x4652, 0x0000, 0x9D05, 0x0000} };
Original dp_palkia_ko = { 0x01E4, 0, 47, {90, 120, 100, 100, 150, 120}, 0xFAF2, 0x0001, 0x2E00, {0x0160, 0x00F6, 0x0151, 0x01CC}, 0x0514, 0x050F, {0x0C52, 0x04A0, 0x094A, 0xFFFF, 0, 0, 0, 0, 0, 0, 0}, {0x10E4, 0x0000, 0x0217, 0x4300, 0x4652, 0x0000, 0x9D05, 0x0000} };

Original pt_giratina_o = { 0x01E7, 0, 47, {150, 120, 100, 100, 90, 120}, 0xFAF2, 0x0001, 0x1A00, {0x01D2, 0x00F6, 0x0151, 0x01D3}, 0x0505, 0x050F, {0x0131, 0x0133, 0x013C, 0x012B, 0x013E, 0x0133, 0x0138, 0x012B, 0xFFFF, 0, 0}, {0x0FFC, 0x1B99, 0x1A06, 0x0000, 0xFFA3, 0x0133, 0x2705, 0x2122} }; //origin
Original pt_giratina_a = { 0x01E7, 0, 47, {150, 100, 120, 90, 100, 120}, 0xFAF2, 0x0001, 0x2E00, {0x01D2, 0x00F6, 0x0151, 0x01D3}, 0x0505, 0x050F, {0x0131, 0x0133, 0x013C, 0x012B, 0x013E, 0x0133, 0x0138, 0x012B, 0xFFFF, 0, 0}, {0x0290, 0x0000, 0x0000, 0x0000, 0x0004, 0x0000, 0x0005, 0x0000} }; //altered
Original pt_dialga = { 0x01E3, 0, 70, {100, 120, 120, 90, 150, 100}, 0x8ACE, 0x0006, 0x2E00, {0x01CB, 0x0179, 0x019E, 0x00A3}, 0x0F05, 0x140A, {0x012E, 0x0133, 0x012B, 0x0136, 0x0131, 0x012B, 0xFFFF, 0, 0, 0, 0}, {0x7570, 0x6464, 0x656C, 0x0070, 0x0000, 0x0000, 0x0005, 0x0000} };
Original pt_palkia = { 0x01E4, 0, 70, {90, 120, 100, 100, 150, 120}, 0x8ACE, 0x0006, 0x2E00, {0x01CC, 0x0179, 0x019E, 0x00A3}, 0x0F05, 0x140A, {0x013A, 0x012B, 0x0136, 0x0135, 0x0133, 0x012B, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000}, {0x7570, 0x6464, 0x656C, 0x0070, 0x0000, 0x0000, 0x0005, 0x0000} };
Original pt_uxie = { 0x01E0, 0, 50, {75, 75, 130, 95, 75, 130}, 0x625A, 0x0002, 0x1A8C, {0x0081, 0x0119, 0x00F8, 0x0085}, 0x0A14, 0x140F, {0x013F, 0x0142, 0x0133, 0x012F, 0xFFFF, 0, 0, 0, 0, 0, 0}, {0x4444, 0x4444, 0x4444, 0x4444, 0x2234, 0x2222, 0x2205, 0x4322} };
Original pt_azelf = { 0x01E2, 0, 50, {75, 125, 70, 115, 70, 125}, 0x625A, 0x0002, 0x1A8C, {0x0081, 0x00FD, 0x00F8, 0x01A1}, 0x0A14, 0x140F, {0x012B, 0x0144, 0x012F, 0x0136, 0x0130, 0xFFFF, 0, 0, 0, 0, 0}, {0x4444, 0x4444, 0x4444, 0x4444, 0x2234, 0x2222, 0x2205, 0x4322} };
Original pt_rotom = { 0x01DF, 0, 20, {50, 50, 77, 91, 95, 77}, 0x1F40, 0, 0x1A46, {0x0054, 0x006D, 0x00FD, 0x0068}, 0x0A1E, 0x0F0A, {0x013C, 0x0139, 0x013E, 0x0139, 0x0137, 0xFFFF, 0, 0, 0, 0, 0}, {0x0000, 0x0005, 0xe000, 0xfa00, 0xfc00, 0x4000, 0x3a05, 0x0800} };
// Original pt_GIRATEiNA_o = { 0x01E7, 0, 47, {150, 120, 100, 100, 90, 120}, 0xFAF2, 0x0001, 0x1A00, {0x01D2, 0x00F6, 0x0151, 0x01D3}, 0x0505, 0x050F, {0x005F, 0x009A, 0x0077, 0x0054, 0x007B, 0xFFFF, 0, 0, 0, 0, 0}, {0x0FFC, 0x1B99, 0x1A06, 0x0000, 0xFFA3, 0x0133, 0x2705, 0x2122} }; //origin
// Original pt_GIRATEiNA_a = { 0x01E7, 0, 47, {150, 100, 120, 90, 100, 120}, 0xFAF2, 0x0001, 0x1A00, {0x01D2, 0x00F6, 0x0151, 0x01D3}, 0x0505, 0x050F, {0x005F, 0x009A, 0x0077, 0x0054, 0x007B, 0xFFFF, 0, 0, 0, 0, 0}, {0x0FFC, 0x1B99, 0x1A06, 0x0000, 0xFFA3, 0x0133, 0x2705, 0x2122} }; //origin// Original pt_dialga = { 0x01E3, 0, 70, {100, 120, 120, 90, 150, 100}, 0, 0, 0, {0, 0, 0, 0}, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0} };
// Original pt_palkia = { 0x01E4, 0, 70, {90, 120, 100, 100, 150, 120}, 0, 0, 0, {0, 0, 0, 0}, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0} };
Original pt_YUKUSHII = { 0x01E0, 0, 50, {75, 75, 130, 95, 75, 130}, 0x625A, 0x0002, 0x1A8C, {0x0081, 0x0119, 0x00F8, 0x0085}, 0x0A14, 0x140F, {0x0097, 0x0060, 0x0068, 0x00F1, 0xFFFF, 0, 0, 0, 0, 0, 0}, {0x4444, 0x4444, 0x4444, 0x4444, 0x2234, 0x2222, 0x2205, 0x4322} };
// Original pt_AGUNOMU = { 0x01E2, 0, 50, {75, 125, 70, 115, 70, 125}, 0x625A, 0x0002, 0x1A8C, {0x0081, 0x00FD, 0x00F8, 0x01A1}, 0x0A14, 0x140F, {0x012B, 0x0144, 0x012F, 0x0136, 0x0130, 0xFFFF, 0, 0, 0, 0, 0}, {0x4444, 0x4444, 0x4444, 0x4444, 0x2234, 0x2222, 0x2205, 0x4322} };
// Original pt_rotom = { 0x01DF, 0, 20, {50, 50, 77, 91, 95, 77}, 0x1F40, 0, 0x1A46, {0x0054, 0x006D, 0x00FD, 0x0068}, 0x0A1E, 0x0F0A, {0x013C, 0x0139, 0x013E, 0x0139, 0x0137, 0xFFFF, 0, 0, 0, 0, 0}, {0x0000, 0x0005, 0xe000, 0xfa00, 0xfc00, 0x4000, 0x3a05, 0x0800} };

Original* OGW_LangVers[LANGUAGES_MAX][VERSIONS_MAX - 1][OG_WILDS_MAX] = {
	{ {&dp_giratina, &dp_arceus, &dp_dialga, &dp_palkia, &dp_shaymin, &dp_darkrai, &dp_uxie, &dp_azelf, &dp_rotom}, {&pt_giratina_o, &pt_giratina_a, &pt_dialga, &pt_palkia, &pt_uxie, &pt_azelf, &pt_rotom, NULL, NULL} }, //_0
	{ {&dp_GIRATEiNA, &dp_ARUSEUSU, &dp_DIARUGA, &dp_PARUKIA, &dp_SHIeIMI, &dp_DAAKURAI, &dp_YUKUSHII, &dp_AGUNOMU, &dp_ROTOMU}, {&pt_giratina_o, &pt_giratina_a, &pt_dialga, &pt_palkia, &pt_YUKUSHII, &pt_azelf, &pt_rotom, NULL, NULL} }, //jp
	{ {&dp_giratina, &dp_arceus, &dp_dialga, &dp_palkia, &dp_shaymin, &dp_darkrai, &dp_uxie, &dp_azelf, &dp_rotom}, {&pt_giratina_o, &pt_giratina_a, &pt_dialga, &pt_palkia, &pt_uxie, &pt_azelf, &pt_rotom, NULL, NULL} }, //en
	{ {&dp_giratina, &dp_arceus, &dp_dialga, &dp_palkia, &dp_shaymin, &dp_darkrai, &dp_crehelf, &dp_crefadet, &dp_motisma}, {&pt_giratina_o, &pt_giratina_a, &pt_dialga, &pt_palkia, &pt_uxie, &pt_azelf, &pt_rotom, NULL, NULL} }, //fr
	{ {&dp_giratina, &dp_arceus, &dp_dialga, &dp_palkia, &dp_shaymin, &dp_darkrai, &dp_uxie, &dp_azelf, &dp_rotom}, {&pt_giratina_o, &pt_giratina_a, &pt_dialga, &pt_palkia, &pt_uxie, &pt_azelf, &pt_rotom, NULL, NULL} }, //it
	{ {&dp_giratina, &dp_arceus, &dp_dialga, &dp_palkia, &dp_shaymin, &dp_darkrai, &dp_selfe, &dp_tobutz, &dp_rotom}, {&pt_giratina_o, &pt_giratina_a, &pt_dialga, &pt_palkia, &pt_uxie, &pt_azelf, &pt_rotom, NULL, NULL} }, //ge
	{ {&dp_giratina, &dp_arceus, &dp_dialga, &dp_palkia, &dp_shaymin, &dp_darkrai, &dp_uxie, &dp_azelf, &dp_rotom}, {&pt_giratina_o, &pt_giratina_a, &pt_dialga, &pt_palkia, &pt_uxie, &pt_azelf, &pt_rotom, NULL, NULL} }, //_6
	{ {&dp_giratina, &dp_arceus, &dp_dialga, &dp_palkia, &dp_shaymin, &dp_darkrai, &dp_uxie, &dp_azelf, &dp_rotom}, {&pt_giratina_o, &pt_giratina_a, &pt_dialga, &pt_palkia, &pt_uxie, &pt_azelf, &pt_rotom, NULL, NULL} }, //sp
	{ {&dp_giratina_ko, &dp_arceus_ko, &dp_dialga_ko, &dp_palkia_ko, &dp_shaymin_ko, &dp_darkrai_ko, &dp_uxie_ko, &dp_azelf_ko, &dp_rotom_ko}, {&pt_giratina_o, &pt_giratina_a, &pt_dialga, &pt_palkia, &pt_uxie, &pt_azelf, &pt_rotom, NULL, NULL} } //ko
};
