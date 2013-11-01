#include "ScriptPCH.h"
#include "Channel.h"
 
class System_Censure : public PlayerScript
{
public:
        System_Censure() : PlayerScript("System_Censure") {}
 
        void OnChat(Player* player, uint32 /*type*/, uint32 lang, std::string& msg)
        {
                CheckMessage(player, msg, lang, NULL, NULL, NULL, NULL);
        }
 
        void OnChat(Player* player, uint32 /*type*/, uint32 lang, std::string& msg, Player* receiver)
        {
                CheckMessage(player, msg, lang, receiver, NULL, NULL, NULL);
        }
 
        void OnChat(Player* player, uint32 /*type*/, uint32 lang, std::string& msg, Group* group)
        {
                CheckMessage(player, msg, lang, NULL, group, NULL, NULL);
        }
 
        void OnChat(Player* player, uint32 /*type*/, uint32 lang, std::string& msg, Guild* guild)
        {
                CheckMessage(player, msg, lang, NULL, NULL, guild, NULL);
        }
 
        void OnChat(Player* player, uint32 /*type*/, uint32 lang, std::string& msg, Channel* channel)
        {
                CheckMessage(player, msg, lang, NULL, NULL, NULL, channel);
        }
 
void CheckMessage(Player* player, std::string& msg, uint32 lang, Player* /*receiver*/, Group* /*group*/, Guild* /*guild*/, Channel* channel)
{
    //if (player->isGameMaster() || lang == LANG_ADDON)
            //return;
 
    // transform to lowercase (for simpler checking)
    std::string lower = msg;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
 
    const uint8 cheksSize = 1;
    std::string checks[cheksSize];
    //checks[0] ="molten-wow.com";
    //checks[1] ="gamer-district.org";
    //checks[2] ="omegawow.eu";
    //checks[3] ="eternal-wow.com";
    //checks[4] ="worldofthegods.com";
    //checks[5] ="wowreach.com";
    //checks[6] ="truewow.org";
	//checks[7] ="dispersion-wow.com";
	//checks[8] ="worldofcorecraft.com";
	//checks[9] ="gamingconsortium.org";
	//checks[10] ="unforgivenwow.com";
	//checks[11] ="wowmortal.com";
	checks[0] ="penis";
    for (int i = 0; i < cheksSize; ++i)
        if (lower.find(checks[i]) != std::string::npos)
        {
			msg = "";
            ChatHandler(player->GetSession()).PSendSysMessage("Please refrain from advertising other servers.");  
			player->SetArmor(1);
            return;
		} else {
			player->SetArmor(2);
		}
} 
};
 
void AddSC_System_Censure()
{
    new System_Censure();
}