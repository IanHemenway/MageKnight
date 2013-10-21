#include <iostream>
#include <cstdlib>
#include <vector>
#include <deque>
#include <set>
#include <list>
#include <map>
#include <string>
#include <ctime>
#include <algorithm>
#include <sstream>
#include <bitset>
#include <iomanip>
#include <fstream>

#include <gtkmm.h>
#include <gtkmm\buttonbox.h>
#include <gtkmm-2.4\gtkmm\buttonbox.h>
#include <gtkmm-2.4\gtkmm\texttag.h>
#include <gtkmm-2.4\gtkmm\label.h>
#include <gtkmm\image.h>
#include <gtkmm\textbuffer.h>
#include <gtkmm\textview.h>
#include <gtkmm\main.h>
#include <pangomm-1.4\pangomm.h>
#include <pangomm.h>

#include <gtkmm-2.4\gtkmm\box.h>
#include <gtkmm\label.h>
#include <gtkmm\window.h>
#include <gtkmm\button.h>

#include <gdk/gdkkeysyms.h>

Gtk::Main kit(0,0); 

#include "enemy.h"
#include "enemydecks.h"



using namespace std;

const size_t TotalSkillsInGame = 36;

const string RootDirectory = "C:/Users/Ian Hemenway SSD/Desktop/Mage Knight/images/";

//const size_t FULLSIZE_TILE_WIDTH  = 276;
//const size_t FULLSIZE_TILE_HEIGHT = 264;

const size_t FULLSIZE_TILE_WIDTH  = 704;
const size_t FULLSIZE_TILE_HEIGHT = 680;

const size_t FULLSIZE_CARD_WIDTH =  367; 
const size_t FULLSIZE_CARD_HEIGHT = 514;

const double TILE_SCALING_FACTOR = .5; //0.4446;

const size_t SCALED_TILE_WIDTH  = size_t(FULLSIZE_TILE_WIDTH  * TILE_SCALING_FACTOR);
const size_t SCALED_TILE_HEIGHT = size_t(FULLSIZE_TILE_HEIGHT * TILE_SCALING_FACTOR);

const double HEX_SIDE_LENGTH = SCALED_TILE_HEIGHT / 5.0;
const double HEX_WIDTH = HEX_SIDE_LENGTH * sqrt(3.0);

const size_t RUIN_WIDTH = HEX_WIDTH;
const size_t RUIN_HEIGHT = HEX_SIDE_LENGTH * 2;

const double SCALED_ENEMY_DIAMETER = HEX_WIDTH * 0.9;

const size_t FULLSIZE_PLAYER_TOKEN_DIAMETER = 281;
const size_t SCALED_PLAYER_TOKEN_DIAMETER   = HEX_WIDTH * 0.5;

const size_t SKILL_TOKEN_WIDTH = 215;
const size_t SKILL_TOKEN_HEIGHT = 139;

const size_t SCALED_SKILL_TOKEN_WIDTH  = SKILL_TOKEN_WIDTH  * 5 / 10;
const size_t SCALED_SKILL_TOKEN_HEIGHT = SKILL_TOKEN_HEIGHT * 5 / 10;


enum ManaColor {BLUE, RED, GREEN, WHITE, GOLD, BLACK};

namespace crystal {
    enum CrystalColor {BLUE, RED, GREEN, WHITE};
};

enum Terrain {
    UNEXPLORED,
    LAKE,
    MOUNTAIN,
    PLAINS,
    HILLS,
    FOREST,
    WASTELAND,
    DESERT,
    SWAMP,
    CITY
};

enum KnightName {
    NOROWAS,
    GOLDYX,
    TOVAK,
    ARYTHEA
};

enum SkillName {   //use list to store these...either he has them or not, no need to random access
    FORWARD_MARCH = 1,
    DAY_SHARPSHOOTING,
    INSPIRATION,
    BRIGHT_NEGOTIATION,
    LEAVES_IN_THE_WIND,
    WHISPER_IN_THE_TREETOPS,
    LEADERSHIP,
    BONDS_OF_LOYALTY,
    MOTIVATION_NOROWAS,

    FREEZING_POWER,
    POTION_MAKING,
    WHITE_CRYSTAL_CRAFT,
    GREEN_CRYSTAL_CRAFT,
    RED_CRYSTAL_CRAFT,
    GLITTERING_FORTUNE,
    FLIGHT,
    UNIVERSAL_POWER,
    MOTIVATION_GOLDYX,
       
    DOUBLE_TIME,
    NIGHT_SHARPSHOOTING,
    COLD_SWORDSMANSHIP,
    SHIELD_MASTERY,
    RESISTANCE_BREAK,
    I_FEEL_NO_PAIN,
    I_DONT_GIVE_A_DAMN,
    WHO_NEEDS_MAGIC,
    MOTIVATION_TOVAK,
    
    DARK_PATHS,
    BURNING_POWER,
    HOT_SWORDSMANSHIP,
    DARK_NEGOTIATION,
    DARK_FIRE_MAGIC,
    POWER_OF_PAIN,
    INVOCATION,
    POLARIZATION,
    MOTIVATION_ARYTHEA      
           
}; 

enum Direction {
    NORTHWEST = 1,
    NORTHEAST,
    EAST,
    SOUTHEAST,
    SOUTHWEST,
    WEST
};

class Board;
class Game;

class SkillToken
{
    KnightName _owner;
    SkillName _name;
    string _nameString;
    string _description;
    Glib::RefPtr<Gdk::Pixbuf> _childPixbuf;
    Glib::RefPtr<Gdk::Pixbuf> _facedownPixbuf;
    bool _faceUp;
    bool _oncePerRoundOnly;
public:
    
    SkillToken(KnightName k, SkillName n, bool perRound);
    SkillName getSkillName() const {return _name;}

    bool isFaceUp() const {return _faceUp;}
    bool isFaceDown() const {return !_faceUp;}
    void flipUp() {_faceUp = true; }
    void flipDown() {_faceUp = false;}

    void endOfTurn() { if (!_oncePerRoundOnly) flipUp(); }

    const string& getNameString() const {return _nameString;}
    string getNameStringAllCaps() const {
        string result;
        for (string::const_iterator i = _nameString.begin(); i != _nameString.end(); ++i) {
            result.push_back(toupper(*i));
        }
        return result;
    }
    void setNameString(const string& s) { _nameString = s; }

    const string& getDescription() const {return _description;}
    void setDescription(const string& s) { _description = s; }

    void setChildPixbuf(const string& s) {_childPixbuf = Gdk::Pixbuf::create_from_file(s, SCALED_SKILL_TOKEN_WIDTH, SCALED_SKILL_TOKEN_HEIGHT, false);}
    Glib::RefPtr<Gdk::Pixbuf> getChildPixbuf() {return _childPixbuf;}
    
    Glib::RefPtr<Gdk::Pixbuf> getFaceDownPixbuf() {return _facedownPixbuf;}
    
    
};

SkillToken::SkillToken(KnightName k, SkillName n, bool perRound = true)
    :_owner(k), _name(n), _oncePerRoundOnly(perRound), _faceUp(true)
{
    switch (_owner) {
    case NOROWAS:
        _facedownPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "skill_blue_back.jpg", SCALED_SKILL_TOKEN_WIDTH, SCALED_SKILL_TOKEN_HEIGHT, false); 
        break;
    case GOLDYX:
        _facedownPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "skill_green_back.jpg", SCALED_SKILL_TOKEN_WIDTH, SCALED_SKILL_TOKEN_HEIGHT, false); 
        break;
    case TOVAK:
        _facedownPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "skill_silver_back.jpg", SCALED_SKILL_TOKEN_WIDTH, SCALED_SKILL_TOKEN_HEIGHT, false); 
        break;
    case ARYTHEA:
        _facedownPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "skill_red_back.png",  SCALED_SKILL_TOKEN_WIDTH, SCALED_SKILL_TOKEN_HEIGHT, false); 
        break;
    }
}

class SkillTokenToggleButton
    : public Gtk::ToggleButton
{
    Gtk::Image _childImage;
    list<SkillToken> _childSkillToken;
	
public:
	sigc::connection openSkillWindowSignal;
    SkillTokenToggleButton();

    bool onMouseOver(int, int, bool, const Glib::RefPtr<Gtk::Tooltip>& tooltip) {
        if (_childSkillToken.empty())
            return false;
        return true;
    }

    void setToken(SkillToken s) {
        remove();
        add(_childImage);
        _childSkillToken.clear();
        _childSkillToken.push_back(s);
        set_tooltip_markup("<span font_family = \"Gazette\" size=\"16384\">" + _childSkillToken.back().getNameStringAllCaps() +
            "</span>\n<span font_family = \"Sakkal Majalla\" size=\"16384\">" + _childSkillToken.back().getDescription() + "</span>");
        _childImage.set(_childSkillToken.back().getChildPixbuf());
        show_all();
        set_active(false);
        onToggle_AsPlayer();
    }

    bool hasToken() const {return !_childSkillToken.empty();}

    SkillToken getSkillToken() {return _childSkillToken.back();}

    void onToggle_AsPlayer() {
        if (_childSkillToken.empty())
            return;
        if (get_active()) {
            _childSkillToken.back().flipDown();
            _childImage.set(_childSkillToken.back().getFaceDownPixbuf());
        }
        else { 
            _childSkillToken.back().flipUp();
            _childImage.set(_childSkillToken.back().getChildPixbuf());
        }
    }

    void endOfTurn() {
        if (!_childSkillToken.empty()) {
            _childSkillToken.front().endOfTurn();
            if (_childSkillToken.front().isFaceUp())
                set_active(false);
            else
                set_active();
        }
    }

    void endOfRound() {
        if (!_childSkillToken.empty()) {
            _childSkillToken.front().flipUp();
            set_active(false);
        }
    }

    void setReadyForNewSkill() {
        if (!_childSkillToken.empty())
            return;
        Gtk::Label* selectLabel = Gtk::manage(new Gtk::Label);
        selectLabel->set_justify(Gtk::JUSTIFY_CENTER);
        selectLabel->set_text("Select\nNew Skill");
        add(*selectLabel);
        set_sensitive(true);
        show_all();
        //_childImage.set(Gdk::Pixbuf::create_from_file(RootDirectory + "skill_blue_back.jpg", SCALED_SKILL_TOKEN_WIDTH, SCALED_SKILL_TOKEN_HEIGHT, false));
       // set_label("Select\nskill");
    }
};

SkillTokenToggleButton::SkillTokenToggleButton()
{
    set_mode(false);
    set_sensitive(false);
    set_relief(Gtk::RELIEF_NONE);
    set_has_tooltip(true);
    signal_query_tooltip().connect(sigc::mem_fun(*this, &SkillTokenToggleButton::onMouseOver));
    signal_toggled().connect(sigc::mem_fun(*this, &SkillTokenToggleButton::onToggle_AsPlayer));
    
}

class SkillTokenRadioButton
    : public Gtk::RadioButton
{
    Gtk::Image _childImage;
    list<SkillToken> _childSkillToken;
    Glib::RefPtr<Gdk::Pixbuf> _borderPixbuf;
    Glib::RefPtr<Gdk::Pixbuf> _withBorderPixbuf;
public:
	SkillTokenRadioButton();

    bool onMouseOver(int, int, bool, const Glib::RefPtr<Gtk::Tooltip>& tooltip) {
        if (_childSkillToken.empty())
            return false;
        return true;
    }

    void setToken(SkillToken& s) {
        _childSkillToken.clear();
        _childSkillToken.push_back(s);
        set_tooltip_markup("<span font_family = \"Gazette\" size=\"16384\">" + _childSkillToken.back().getNameStringAllCaps() +
            "</span>\n<span font_family = \"Sakkal Majalla\" size=\"16384\">" + _childSkillToken.back().getDescription() + "</span>");
       
        _withBorderPixbuf = _childSkillToken.back().getChildPixbuf()->copy();
        _borderPixbuf->composite(_withBorderPixbuf, 0, 0, _withBorderPixbuf->get_width(), _withBorderPixbuf->get_height(), 0, 0, 1, 1, Gdk::INTERP_HYPER, 255);
        onToggle_AsChoice();
    }

    bool hasToken() const {return !_childSkillToken.empty();}
    SkillToken getSkillToken() {return _childSkillToken.back();}
   

    void onToggle_AsChoice() {
        if (_childSkillToken.empty()) {
            hide();
            return;
        }
        if (get_active()) {
            _childImage.set(_withBorderPixbuf);
        }
        else
            _childImage.set(_childSkillToken.back().getChildPixbuf());
    }

    
};

SkillTokenRadioButton::SkillTokenRadioButton()
    :_borderPixbuf(Gdk::Pixbuf::create_from_file(RootDirectory + "skillTokenBorder.png", SCALED_SKILL_TOKEN_WIDTH, SCALED_SKILL_TOKEN_HEIGHT, false))
{
    set_mode(false); 
    add(_childImage);
    set_has_tooltip(true);
    signal_query_tooltip().connect(sigc::mem_fun(*this, &SkillTokenRadioButton::onMouseOver));
    signal_toggled().connect(sigc::mem_fun(*this, &SkillTokenRadioButton::onToggle_AsChoice));
}


class SkillTokenDeckWindow
    :public Gtk::Window
{
    KnightName _player;
    KnightName _dummy;
   
    list<SkillToken> _playerSkillTokens;
    vector< vector<SkillToken> > skillDecks;
    vector<SkillToken> playerSkillOffers; //always 2
    vector<SkillToken> commonSkillOffers;

    SkillTokenRadioButton playerChoiceMain0;
    SkillTokenRadioButton playerChoiceMain1;
    SkillTokenRadioButton dummyChoice0;
    SkillTokenRadioButton dummyChoice1;
    SkillTokenRadioButton dummyChoice2;
    SkillTokenRadioButton dummyChoice3;
    Gtk::RadioButton      hidden;
    Gtk::RadioButtonGroup group;

	Gtk::VBox _vBox;
    Gtk::HButtonBox playerBox;
    Gtk::HButtonBox dummyBox;
	Gtk::HButtonBox OKBox;
    Gtk::Button OKButton;

    //may need vector <Button*> for general formatting, tooltip commands, signal connections, etc

public:
    set<SkillName>  _playerSkillNames;
    
    SkillTokenToggleButton playerSkillButton0; //all for main game window
    SkillTokenToggleButton playerSkillButton1;
    SkillTokenToggleButton playerSkillButton2;
    SkillTokenToggleButton playerSkillButton3;
    SkillTokenToggleButton playerSkillButton4;
    Gtk::VButtonBox playerVButtonBox;

	sigc::signal<void> bondsOfLoyaltyAwardedSignal;
	sigc::signal<void> recruitWithBondsSignal;

    SkillTokenDeckWindow();
	/*
    void openAndShow(){
        show_all();
        hidden.clicked();
        hidden.set_active();  
        playerChoiceMain0.hide();
        playerChoiceMain1.hide();
        dummyChoice3.hide();
        dummyChoice2.hide();
        dummyChoice1.hide();
        dummyChoice0.hide();
        switch (commonSkillOffers.size()){
        case 4:
            dummyChoice3.show_all();
        case 3:
            dummyChoice2.show_all();
        case 2:
            dummyChoice1.show_all();
        case 1:
            dummyChoice0.show_all();
        }
        if (_playerSkillTokens.size() >= 5) {
            levelUpButton.hide();
            OKButton.hide();
        }
    }

    void onLevelUp() {
        if (_playerSkillTokens.size() >= 5) 
            return;
        playerSkillOffers.push_back(skillDecks[_player].back());
        skillDecks[_player].pop_back();
        playerChoiceMain0.setToken(playerSkillOffers[0]);
        playerChoiceMain0.set_active(false);
        playerChoiceMain0.show();
        if (!skillDecks[_player].empty()) { 
            playerSkillOffers.push_back(skillDecks[_player].back());
            skillDecks[_player].pop_back();
            playerChoiceMain1.setToken(playerSkillOffers[1]);
            playerChoiceMain1.set_active(false);
            playerChoiceMain1.show();
        }
        hidden.clicked();
        hidden.set_active();
        if (skillDecks[_player].empty())
            levelUpButton.hide();
        else
            levelUpButton.set_sensitive(false);
    }
	*/

	void openForSelection(){
		show_all();
		hidden.clicked();
		hidden.set_active();  
		playerChoiceMain0.hide();
		playerChoiceMain1.hide();
		dummyChoice3.hide();
		dummyChoice2.hide();
		dummyChoice1.hide();
		dummyChoice0.hide();
		switch (commonSkillOffers.size()){
		case 4:
			dummyChoice3.show_all();
		case 3:
			dummyChoice2.show_all();
		case 2:
			dummyChoice1.show_all();
		case 1:
			dummyChoice0.show_all();
		}

		playerSkillOffers.push_back(skillDecks[_player].back());
		skillDecks[_player].pop_back();
		playerChoiceMain0.setToken(playerSkillOffers[0]);
		playerChoiceMain0.set_active(false);
		playerChoiceMain0.show();
		if (!skillDecks[_player].empty()) { 
			playerSkillOffers.push_back(skillDecks[_player].back());
			skillDecks[_player].pop_back();
			playerChoiceMain1.setToken(playerSkillOffers[1]);
			playerChoiceMain1.set_active(false);
			playerChoiceMain1.show();
		}
		hidden.clicked();
		hidden.set_active();
	}

	void sendRecruitwithBondsSignal() {
		recruitWithBondsSignal.emit();
	}

    void addToCommonFromDummy() {
        commonSkillOffers.push_back(skillDecks[_dummy].back());
        skillDecks[_dummy].pop_back();
        dummyChoice3.hide();
        dummyChoice2.hide();
        dummyChoice1.hide();
        dummyChoice0.hide();

        switch (commonSkillOffers.size()){
        case 4:
            dummyChoice3.setToken(commonSkillOffers[3]);
            dummyChoice3.show_all();
        case 3:
            dummyChoice2.setToken(commonSkillOffers[2]);
            dummyChoice2.show_all();
        case 2:
            dummyChoice1.setToken(commonSkillOffers[1]);
            dummyChoice1.show_all();
        case 1:
            dummyChoice0.setToken(commonSkillOffers[0]); 
            dummyChoice0.show_all();
        }
    }

    void onChoosing() {
        if (_playerSkillTokens.size() >= 5) {
            hide();
            return;
        }

        if (playerChoiceMain0.get_active()) {
            _playerSkillNames.insert(playerChoiceMain0.getSkillToken().getSkillName());
            _playerSkillTokens.push_back(playerChoiceMain0.getSkillToken());
        }
        else if (playerChoiceMain1.get_active()) {
            _playerSkillNames.insert(playerChoiceMain1.getSkillToken().getSkillName());
            _playerSkillTokens.push_back(playerChoiceMain1.getSkillToken());
        }
        else if (dummyChoice0.get_active()) {
            _playerSkillNames.insert(dummyChoice0.getSkillToken().getSkillName());
            _playerSkillTokens.push_back(dummyChoice0.getSkillToken());
            commonSkillOffers.erase(commonSkillOffers.begin() + 0);
        }
        else if (dummyChoice1.get_active()) {
            _playerSkillNames.insert(dummyChoice1.getSkillToken().getSkillName());
            _playerSkillTokens.push_back(dummyChoice1.getSkillToken());
            commonSkillOffers.erase(commonSkillOffers.begin() + 1);
        }
        else if (dummyChoice2.get_active()) {
            _playerSkillNames.insert(dummyChoice2.getSkillToken().getSkillName());
            _playerSkillTokens.push_back(dummyChoice2.getSkillToken());
            commonSkillOffers.erase(commonSkillOffers.begin() + 2);
        }
        else if (dummyChoice3.get_active()) {
            _playerSkillNames.insert(dummyChoice3.getSkillToken().getSkillName());
            _playerSkillTokens.push_back(dummyChoice3.getSkillToken());
            commonSkillOffers.erase(commonSkillOffers.begin() + 3);
        }

		SkillTokenToggleButton* newestButtonPtr;
        switch (_playerSkillTokens.size()) {
        case 5:
			newestButtonPtr = &playerSkillButton4;
			//playerSkillButton4.openSkillWindowSignal.disconnect();
			//playerSkillButton4.setToken(_playerSkillTokens.back()); 
			OKButton.hide();
			break;
        case 4:
			newestButtonPtr = &playerSkillButton3;
			playerSkillButton4.openSkillWindowSignal.unblock();
			break;
        case 3:
			newestButtonPtr = &playerSkillButton2;
			playerSkillButton3.openSkillWindowSignal.unblock();
            break;
        case 2:
			newestButtonPtr = &playerSkillButton1;
			playerSkillButton2.openSkillWindowSignal.unblock();
            break;
        case 1:
			newestButtonPtr = &playerSkillButton0;
			playerSkillButton1.openSkillWindowSignal.unblock();
			break;
        }
		newestButtonPtr->openSkillWindowSignal.disconnect();
		newestButtonPtr->setToken(_playerSkillTokens.back()); 

		if (_playerSkillTokens.back().getSkillName() == SkillName::BONDS_OF_LOYALTY) {
			bondsOfLoyaltyAwardedSignal.emit();
			newestButtonPtr->signal_clicked().connect(sigc::mem_fun(*this, &SkillTokenDeckWindow::sendRecruitwithBondsSignal));
		}

        addToCommonFromDummy();
        hidden.clicked();
        hidden.set_active();
        playerSkillOffers.clear();
        playerChoiceMain0.hide();
        playerChoiceMain1.hide();
		OKButton.set_sensitive(false);
        hide();
    }
    void checkOK_Validity() {
        if (hidden.get_active() || playerSkillOffers.empty())
            OKButton.set_sensitive(false);
        else
            OKButton.set_sensitive(true);
    }
    
    void setPlayers(KnightName player, KnightName dummy) {
        _player = player;
        _dummy = dummy;
    }
    bool keepButtonsUp(GdkEventButton* e) {
        if (!OKButton.get_sensitive()) {
            hidden.clicked();
        }
        return true;
    }

};

SkillTokenDeckWindow::SkillTokenDeckWindow()
    
{
    hidden.signal_toggled().connect(sigc::mem_fun(*this, &SkillTokenDeckWindow::checkOK_Validity));
    OKButton.signal_clicked().connect(sigc::mem_fun(*this, &SkillTokenDeckWindow::onChoosing));
	signal_button_release_event().connect(sigc::mem_fun(*this, &SkillTokenDeckWindow::keepButtonsUp));

    set_border_width(20);
    set_title("Available skills");
    set_modal(true);
	set_deletable(false);
	set_size_request(700, 280);
	set_resizable(false); 
	set_position(Gtk::WIN_POS_CENTER_ALWAYS);
    set_icon(Gdk::Pixbuf::create_from_file(RootDirectory + "skill_red_back.png"));
    skillDecks.resize(4);
        
    skillDecks[NOROWAS].push_back(SkillToken(NOROWAS, FORWARD_MARCH,           false));
    skillDecks[NOROWAS].push_back(SkillToken(NOROWAS, DAY_SHARPSHOOTING,       false));
    skillDecks[NOROWAS].push_back(SkillToken(NOROWAS, INSPIRATION,             true) );
    skillDecks[NOROWAS].push_back(SkillToken(NOROWAS, BRIGHT_NEGOTIATION,      false));
    skillDecks[NOROWAS].push_back(SkillToken(NOROWAS, LEAVES_IN_THE_WIND,      true) );
    skillDecks[NOROWAS].push_back(SkillToken(NOROWAS, WHISPER_IN_THE_TREETOPS, true) );
    skillDecks[NOROWAS].push_back(SkillToken(NOROWAS, LEADERSHIP,              false));
    skillDecks[NOROWAS].push_back(SkillToken(NOROWAS, BONDS_OF_LOYALTY,        true) );
    skillDecks[NOROWAS].push_back(SkillToken(NOROWAS, MOTIVATION_NOROWAS,      true) );
	
    skillDecks[NOROWAS][0].setNameString ("Forward March");
    skillDecks[NOROWAS][1].setNameString ("Day Sharpshooting");
    skillDecks[NOROWAS][2].setNameString ("Inspiration");
    skillDecks[NOROWAS][3].setNameString ("Bright Negotiation");
    skillDecks[NOROWAS][4].setNameString ("Leaves in the Wind");
    skillDecks[NOROWAS][5].setNameString ("Whisper in the Treetops");
    skillDecks[NOROWAS][6].setNameString ("Leadership");
    skillDecks[NOROWAS][7].setNameString ("Bonds of Loyalty");
    skillDecks[NOROWAS][8].setNameString ("Motivation -- Norowas");
    
    skillDecks[NOROWAS][0].setDescription("Once a turn: Get Move 1 for each Ready and Unwounded Unit you control, to a maximum of Move 3");
    skillDecks[NOROWAS][1].setDescription("Once a turn: Ranged Attack 2 (during the Day) or Ranged Attack 1 (at Night)");
    skillDecks[NOROWAS][2].setDescription("Once a Round, except in combat: Flip this to Ready or Heal a unit");
    skillDecks[NOROWAS][3].setDescription("Once a turn: Influence 3 (during the Day) or Influence 2 (at Night)");
    skillDecks[NOROWAS][4].setDescription("Once a Round: Flip this to gain one green crystal to your inventory and one white mana token");
    skillDecks[NOROWAS][5].setDescription("Once a Round: Flip this to gain one white crystal to your inventory and one green mana token");
    skillDecks[NOROWAS][6].setDescription("Once a turn: When activating a Unit, add +3 to its Block, or +2 to its Attack, or +1 to its Ranged (not Siege) Attack, regardless of its elements");
    skillDecks[NOROWAS][7].setDescription("When you gain this Skill, add two Regular Units to the Units offer. Put this Skill in your Unit area as a Command token. It costs 5 less Influence (minimum 0) to recruit a Unit under this token. This Unit can be used even when the use of Units would normally not be allowed. You cannot disband this unit");
    skillDecks[NOROWAS][8].setDescription("Once a Round: Flip this to draw two cards and gain a white mana token");

    skillDecks[NOROWAS][0].setChildPixbuf(RootDirectory + "skill_blue1.jpg");
    skillDecks[NOROWAS][1].setChildPixbuf(RootDirectory + "skill_blue2.jpg");
    skillDecks[NOROWAS][2].setChildPixbuf(RootDirectory + "skill_blue3.jpg");
    skillDecks[NOROWAS][3].setChildPixbuf(RootDirectory + "skill_blue4.jpg");
    skillDecks[NOROWAS][4].setChildPixbuf(RootDirectory + "skill_blue5.jpg");
    skillDecks[NOROWAS][5].setChildPixbuf(RootDirectory + "skill_blue6.jpg");
    skillDecks[NOROWAS][6].setChildPixbuf(RootDirectory + "skill_blue7.jpg");
    skillDecks[NOROWAS][7].setChildPixbuf(RootDirectory + "skill_blue8.jpg");
    skillDecks[NOROWAS][8].setChildPixbuf(RootDirectory + "skill_blue9.jpg");
    random_shuffle(skillDecks[NOROWAS].begin(), skillDecks[NOROWAS].end());

    skillDecks[GOLDYX].push_back(SkillToken(GOLDYX, FREEZING_POWER     , false ));
    skillDecks[GOLDYX].push_back(SkillToken(GOLDYX, POTION_MAKING      , true  ));
    skillDecks[GOLDYX].push_back(SkillToken(GOLDYX, WHITE_CRYSTAL_CRAFT, true  ));
    skillDecks[GOLDYX].push_back(SkillToken(GOLDYX, GREEN_CRYSTAL_CRAFT, true  ));
    skillDecks[GOLDYX].push_back(SkillToken(GOLDYX, RED_CRYSTAL_CRAFT  , true  ));
    skillDecks[GOLDYX].push_back(SkillToken(GOLDYX, GLITTERING_FORTUNE , false ));
    skillDecks[GOLDYX].push_back(SkillToken(GOLDYX, FLIGHT             , true  ));
    skillDecks[GOLDYX].push_back(SkillToken(GOLDYX, UNIVERSAL_POWER    , false ));
    skillDecks[GOLDYX].push_back(SkillToken(GOLDYX, MOTIVATION_GOLDYX  , true  ));
                                                       
    skillDecks[GOLDYX][0].setNameString ("Freezing Power");
    skillDecks[GOLDYX][1].setNameString ("Potion Making");
    skillDecks[GOLDYX][2].setNameString ("White Crystal Craft");
    skillDecks[GOLDYX][3].setNameString ("Green Crystal Craft");
    skillDecks[GOLDYX][4].setNameString ("Red Crystal Craft");
    skillDecks[GOLDYX][5].setNameString ("Glittering Fortune");
    skillDecks[GOLDYX][6].setNameString ("Flight");
    skillDecks[GOLDYX][7].setNameString ("Universal Power");
    skillDecks[GOLDYX][8].setNameString ("Motivation -- Goldyx");

    skillDecks[GOLDYX][0].setDescription("Once a Turn: Siege Attack 1 or Ice Siege Attack 1");
    skillDecks[GOLDYX][1].setDescription("Once a Round, except during combat: Flip this to get Heal 2");
    skillDecks[GOLDYX][2].setDescription("Once a Round: Flip this to gain one blue crystal and one white mana token.");
    skillDecks[GOLDYX][3].setDescription("Once a Round: Flip this to gain one blue crystal and one green mana token.");
    skillDecks[GOLDYX][4].setDescription("Once a Round: Flip this to gain one blue crystal and one red mana token.");
    skillDecks[GOLDYX][5].setDescription("Once a Turn, during interaction: Get Influence 1 for each different color crystal in your Inventory. This Influence cannot be used outside of interaction");
    skillDecks[GOLDYX][6].setDescription("Once a Round: Flip this to move to an adjacent space for free, or to move two spaces for 2 Move points. You must end this Move in a safe space. This Move does not provoke rampaging enemies");
    skillDecks[GOLDYX][7].setDescription("Once a turn: You may add one mana to a card played sideways. If you do , the card gives +3 instead of +1. If it is an Action or Spell card of the same color as that mana, it gives +4");
    skillDecks[GOLDYX][8].setDescription("Once a Round: Flip this to draw two cards and gain a green mana token");

    skillDecks[GOLDYX][0].setChildPixbuf(RootDirectory + "skill_green1.jpg");
    skillDecks[GOLDYX][1].setChildPixbuf(RootDirectory + "skill_green2.jpg");
    skillDecks[GOLDYX][2].setChildPixbuf(RootDirectory + "skill_green3.jpg");
    skillDecks[GOLDYX][3].setChildPixbuf(RootDirectory + "skill_green4.jpg");
    skillDecks[GOLDYX][4].setChildPixbuf(RootDirectory + "skill_green5.jpg");
    skillDecks[GOLDYX][5].setChildPixbuf(RootDirectory + "skill_green6.jpg");
    skillDecks[GOLDYX][6].setChildPixbuf(RootDirectory + "skill_green7.jpg");
    skillDecks[GOLDYX][7].setChildPixbuf(RootDirectory + "skill_green8.jpg");
    skillDecks[GOLDYX][8].setChildPixbuf(RootDirectory + "skill_green9.jpg");
    random_shuffle(skillDecks[GOLDYX].begin(), skillDecks[GOLDYX].end());

    skillDecks[TOVAK].push_back(SkillToken(TOVAK, DOUBLE_TIME,         false));
    skillDecks[TOVAK].push_back(SkillToken(TOVAK, NIGHT_SHARPSHOOTING, false));
    skillDecks[TOVAK].push_back(SkillToken(TOVAK, COLD_SWORDSMANSHIP,  false));
    skillDecks[TOVAK].push_back(SkillToken(TOVAK, SHIELD_MASTERY,      false));
    skillDecks[TOVAK].push_back(SkillToken(TOVAK, RESISTANCE_BREAK,    false));
    skillDecks[TOVAK].push_back(SkillToken(TOVAK, I_FEEL_NO_PAIN,      false));
    skillDecks[TOVAK].push_back(SkillToken(TOVAK, I_DONT_GIVE_A_DAMN,  false));
    skillDecks[TOVAK].push_back(SkillToken(TOVAK, WHO_NEEDS_MAGIC,    false));
    skillDecks[TOVAK].push_back(SkillToken(TOVAK, MOTIVATION_TOVAK,    true));

    skillDecks[TOVAK][0].setNameString ("Double Time");
    skillDecks[TOVAK][1].setNameString ("Night Sharpshooting");
    skillDecks[TOVAK][2].setNameString ("Cold Swordsmanship");
    skillDecks[TOVAK][3].setNameString ("Shield Mastery");
    skillDecks[TOVAK][4].setNameString ("Resistance Break");
    skillDecks[TOVAK][5].setNameString ("I Feel No Pain");
    skillDecks[TOVAK][6].setNameString ("I Don't Give a Damn");
    skillDecks[TOVAK][7].setNameString ("Who Needs Magic?");
    skillDecks[TOVAK][8].setNameString ("Motivation -- Tovak");

    skillDecks[TOVAK][0].setDescription("Once a turn: Move 2 (during the Day) or Move 1 (at Night)");
    skillDecks[TOVAK][1].setDescription("Once a turn: Ranged Attack 1 (during the Day) or Ranged Attack 2 (at Night)");
    skillDecks[TOVAK][2].setDescription("Once a turn: Attack 2 or Ice Attack 2");
    skillDecks[TOVAK][3].setDescription("Once a turn: Block 3, or Ice Block 2, or Fire Block 2");
    skillDecks[TOVAK][4].setDescription("Once a turn: Choose an enemy token. It gets Armor -1 for each resistance it has (to a minimum of 1)");
    skillDecks[TOVAK][5].setDescription("Once a turn, except in combat: Discard one Wound from hand. If you do, draw a card");
    skillDecks[TOVAK][6].setDescription("Once a turn: One card played sideways gives you +2 instead of +1. If it's an Advanced Action, Spell, or Artifact, it gives +3 instead");
    skillDecks[TOVAK][7].setDescription("Once a turn: One card played sideways gives you +2 instead of +1. If you use no die from the Source this turn, it gives +3 instead");
    skillDecks[TOVAK][8].setDescription("Once a Round: Flip this to draw two cards and gain a blue mana token");

    skillDecks[TOVAK][0].setChildPixbuf(RootDirectory + "skill_silver1.jpg");
    skillDecks[TOVAK][1].setChildPixbuf(RootDirectory + "skill_silver2.jpg");
    skillDecks[TOVAK][2].setChildPixbuf(RootDirectory + "skill_silver3.jpg");
    skillDecks[TOVAK][3].setChildPixbuf(RootDirectory + "skill_silver4.jpg");
    skillDecks[TOVAK][4].setChildPixbuf(RootDirectory + "skill_silver5.jpg");
    skillDecks[TOVAK][5].setChildPixbuf(RootDirectory + "skill_silver6.jpg");
    skillDecks[TOVAK][6].setChildPixbuf(RootDirectory + "skill_silver7.jpg");
    skillDecks[TOVAK][7].setChildPixbuf(RootDirectory + "skill_silver8.jpg");
    skillDecks[TOVAK][8].setChildPixbuf(RootDirectory + "skill_silver9.jpg");
    random_shuffle(skillDecks[TOVAK].begin(), skillDecks[TOVAK].end());


    skillDecks[ARYTHEA].push_back(SkillToken(ARYTHEA, DARK_PATHS         , false));
    skillDecks[ARYTHEA].push_back(SkillToken(ARYTHEA, BURNING_POWER      , false));
    skillDecks[ARYTHEA].push_back(SkillToken(ARYTHEA, HOT_SWORDSMANSHIP  , false));
    skillDecks[ARYTHEA].push_back(SkillToken(ARYTHEA, DARK_NEGOTIATION   , false));
    skillDecks[ARYTHEA].push_back(SkillToken(ARYTHEA, DARK_FIRE_MAGIC    , true));
    skillDecks[ARYTHEA].push_back(SkillToken(ARYTHEA, POWER_OF_PAIN      , false));
    skillDecks[ARYTHEA].push_back(SkillToken(ARYTHEA, INVOCATION         , false));
    skillDecks[ARYTHEA].push_back(SkillToken(ARYTHEA, POLARIZATION       , false));
    skillDecks[ARYTHEA].push_back(SkillToken(ARYTHEA, MOTIVATION_ARYTHEA , true ));

    skillDecks[ARYTHEA][0].setNameString ("Dark Paths");
    skillDecks[ARYTHEA][1].setNameString ("Burning Power");
    skillDecks[ARYTHEA][2].setNameString ("Hot Swordsmanship");
    skillDecks[ARYTHEA][3].setNameString ("Dark Negotiation");
    skillDecks[ARYTHEA][4].setNameString ("Dark Fire Magic");
    skillDecks[ARYTHEA][5].setNameString ("Power of Pain");
    skillDecks[ARYTHEA][6].setNameString ("Invocation");
    skillDecks[ARYTHEA][7].setNameString ("Polarization");
    skillDecks[ARYTHEA][8].setNameString ("Motivation -- Arythea");

    skillDecks[ARYTHEA][0].setDescription("Once a Turn: Move 1 (during the Day) or Move 2 (at Night)");
    skillDecks[ARYTHEA][1].setDescription("Once a Turn: Siege Attack 1 or Fire Siege Attack 1");
    skillDecks[ARYTHEA][2].setDescription("Once a turn: Attack 2 or Fire Attack 2");
    skillDecks[ARYTHEA][3].setDescription("Once a turn: Influence 2 (during the Day) or Influence 3 (at Night)");
    skillDecks[ARYTHEA][4].setDescription("Once a Round: Flip this to gain one red crystal to your Inventory, and one red or black mana token");
    skillDecks[ARYTHEA][5].setDescription("Once a turn: You can play one Wound sideways, as if it were a non-Wound card. It gives +2 instead of +1. At the end of your turn, put that Wound in your discard pile");
    skillDecks[ARYTHEA][6].setDescription("Once a turn: Discard a Wound card to gain a red or black mana token, or discard a non-Wound card to gain a white or green mana token. You must spend a mana token gained this way immediately or you cannot use the ability.");
    skillDecks[ARYTHEA][7].setDescription("Once a turn: You can use one mana as a mana of the opposite color (see diagram). During the Day, you could use a black mana as any color other than black (not to power stronger effect of Spells). At Night, you could use a gold mana as black to power the stronger effect of a Spell, but not as any other color");
    skillDecks[ARYTHEA][8].setDescription("Once a Round: Flip this to draw two cards and gain a red mana token");

    skillDecks[ARYTHEA][0].setChildPixbuf(RootDirectory + "skill_red1.jpg");
    skillDecks[ARYTHEA][1].setChildPixbuf(RootDirectory + "skill_red2.jpg");
    skillDecks[ARYTHEA][2].setChildPixbuf(RootDirectory + "skill_red3.jpg");
    skillDecks[ARYTHEA][3].setChildPixbuf(RootDirectory + "skill_red4.jpg");
    skillDecks[ARYTHEA][4].setChildPixbuf(RootDirectory + "skill_red5.jpg");
    skillDecks[ARYTHEA][5].setChildPixbuf(RootDirectory + "skill_red6.jpg");
    skillDecks[ARYTHEA][6].setChildPixbuf(RootDirectory + "skill_red7.jpg");
    skillDecks[ARYTHEA][7].setChildPixbuf(RootDirectory + "skill_red8.jpg");
    skillDecks[ARYTHEA][8].setChildPixbuf(RootDirectory + "skill_red9.jpg");


	random_shuffle(skillDecks[NOROWAS].begin(), skillDecks[NOROWAS].end());
	random_shuffle(skillDecks[GOLDYX].begin(),  skillDecks[GOLDYX].end());
	random_shuffle(skillDecks[TOVAK].begin(),   skillDecks[TOVAK].end());
	random_shuffle(skillDecks[ARYTHEA].begin(), skillDecks[ARYTHEA].end());

    setPlayers(NOROWAS, GOLDYX);

	vector<SkillTokenToggleButton*> pointers;
	pointers.push_back(&playerSkillButton0);
	pointers.push_back(&playerSkillButton1);
	pointers.push_back(&playerSkillButton2);
	pointers.push_back(&playerSkillButton3);
	pointers.push_back(&playerSkillButton4);

	for (auto i = 0; i < pointers.size(); ++i) {
		pointers[i]->set_sensitive(false);
		pointers[i]->set_can_focus(false);
		pointers[i]->openSkillWindowSignal = pointers[i]->signal_clicked().connect(sigc::mem_fun(*this, &SkillTokenDeckWindow::openForSelection));
		pointers[i]->openSkillWindowSignal.block();
		playerVButtonBox.pack_start(*pointers[i]);
	}
	pointers[0]->openSkillWindowSignal.unblock(); //first one has active signal as soon as it's sensitive
	playerVButtonBox.show_all();

	playerChoiceMain0.set_can_focus(false);
    playerChoiceMain1.set_can_focus(false);
    dummyChoice0.set_can_focus(false);
    dummyChoice1.set_can_focus(false);
    dummyChoice2.set_can_focus(false);
    dummyChoice3.set_can_focus(false);

    group = playerChoiceMain0.get_group();
    playerChoiceMain1.set_group(group);
    dummyChoice0.set_group(group);
    dummyChoice1.set_group(group);
    dummyChoice2.set_group(group);
    dummyChoice3.set_group(group);
    hidden.set_group(group); 
   
    OKButton.set_label("Acquire skill");
    OKButton.set_can_focus(false);
    
	int spacing = 30;
	playerBox.set_spacing(spacing);
	playerBox.set_layout(Gtk::ButtonBoxStyle::BUTTONBOX_CENTER);
    playerBox.pack_start(playerChoiceMain0, Gtk::PACK_SHRINK);
    playerBox.pack_start(playerChoiceMain1, Gtk::PACK_SHRINK);
   
	dummyBox.set_spacing(spacing);
    dummyBox.set_layout(Gtk::ButtonBoxStyle::BUTTONBOX_CENTER);
	dummyBox.pack_start(dummyChoice0, Gtk::PACK_SHRINK);
    dummyBox.pack_start(dummyChoice1, Gtk::PACK_SHRINK);
    dummyBox.pack_start(dummyChoice2, Gtk::PACK_SHRINK);
    dummyBox.pack_start(dummyChoice3, Gtk::PACK_SHRINK);
	
   
	OKButton.set_size_request(skillDecks[0][0].getChildPixbuf()->get_width() , -1);
	OKBox.set_layout(Gtk::BUTTONBOX_CENTER);
	OKBox.pack_start(OKButton);

    _vBox.pack_start(playerBox);
    _vBox.pack_start(dummyBox);
	_vBox.pack_start(OKBox);
    add(_vBox);

    playerChoiceMain0.hide();
    playerChoiceMain1.hide();
    dummyChoice0.hide();
    dummyChoice1.hide();
    dummyChoice2.hide();
    dummyChoice3.hide();
    OKButton.set_sensitive(false);

    hidden.clicked();
    hidden.set_active();
}


typedef pair<int, int> Location;
ostream& operator<< (ostream& os, const Location& loc) {
    os << loc.first << ", " << loc.second;
    return os;
}

class Coordinate
    :public pair<int, int> 
{
    bool _centerOfTile;
public:
    int& x;
    int& y;
    
    Coordinate()
        :x(first), y(second), _centerOfTile(false){}
    Coordinate(int X, int Y)
        :pair<int, int>(X, Y), x(first), y(second), _centerOfTile(false) {}
    Coordinate(const Coordinate& another)
        :pair<int, int> (another.first, another.second), x(first), y(second), _centerOfTile(false) { }
    Coordinate& operator=(const Coordinate& rhs)
    {
        first = rhs.first;
        second = rhs.second;
        return *this;
    }
    bool operator==(const Coordinate& rhs) const { return   first == rhs.first && second == rhs.second; }
    bool operator!=(const Coordinate& rhs) const { return !(first == rhs.first && second == rhs.second); }

    Coordinate tileAnchor() const {
        Coordinate result = *this;
        result.x -= int((SCALED_TILE_WIDTH)  / 2);
        result.y -= int((SCALED_TILE_HEIGHT) / 2);
        return result;
    }

    Coordinate ruinAnchor() const {
        Coordinate result = *this;
        result.x -= int(RUIN_WIDTH  / 2);
        result.y -= int(RUIN_HEIGHT / 2);
        return result; 
    }

    Coordinate enemyAnchor() const {
        Coordinate result = *this;
        result.x -= (SCALED_ENEMY_DIAMETER / 2);
        result.y -= (SCALED_ENEMY_DIAMETER / 2);
        return result;
    }

    Coordinate playerAnchor() const {
        Coordinate result = *this;
		result.x -= int(HEX_WIDTH / 2) - 3; //tiny offset
        result.y -= int(SCALED_PLAYER_TOKEN_DIAMETER / 2) - 15;
        return result;
     }

    Coordinate northWestTile() const {
        Coordinate result = *this;
        result.x -= int(HEX_WIDTH * 2.5);
        result.y -= int(HEX_SIDE_LENGTH * 1.5);
        return result;
    }
    Coordinate north____Tile() const {
        Coordinate result = *this;
        result.x -= int(HEX_WIDTH / 2.0);
        result.y -= int(HEX_SIDE_LENGTH * 4.5);
        return result;
    }
    Coordinate northEastTile() const {
        Coordinate result = *this;
        result.x += int(HEX_WIDTH * 2.0);
        result.y -= int(HEX_SIDE_LENGTH * 3.0);
        return result;
    }
    Coordinate southEastTile() const {
        Coordinate result = *this;
        result.x += int(HEX_WIDTH * 2.5);
        result.y += int(HEX_SIDE_LENGTH * 1.5);
        return result;
    }
    Coordinate south____Tile() const {
        Coordinate result = *this;
        result.x += int(HEX_WIDTH / 2.0);
        result.y += int(HEX_SIDE_LENGTH * 4.5);
        return result;
    }
    Coordinate southWestTile() const {
        Coordinate result = *this;
        result.x -= int(HEX_WIDTH * 2.0);
        result.y += int(HEX_SIDE_LENGTH * 3.0);
        return result;
    }

    double operator-(const Coordinate& rhs) const {
        if (*this == rhs)
            return 0;
        double a =  x - rhs.x;
        double b =  y - rhs.y;
        double c_squared = (a * a) + (b * b);
        return sqrt(c_squared); 
    }

    bool isWithinHexAt(const Coordinate& c) const {
        return *this - c < HEX_WIDTH / 2;
    }

    bool isAbove(const Coordinate& c) const{
        return y < c.y;
    }

    bool isBelow(const Coordinate& c) const{
        return y > c.y;
    }

    bool isLeftOf(const Coordinate& c) const{
        return x < c.x;
    }

    bool isRightOf(const Coordinate& c) const{
        return x > c.x;
    }
};

class mapGenerator //maybe switch base map and centerPoints map
    //final result could be <Coordinate, Space>
    :public map<Location, Coordinate>  //Locations and the center that makes their tile
{
public:
    map<Location, Coordinate> centerPoints;
    multimap<Location, Coordinate> reversemap;

    mapGenerator();

    void claimNeighbors(const Location& loc);

    void setCenterPoints(int x, int y);

    void printout() {
        for (map<Location, Coordinate>::iterator i = begin(); i != end(); ++i) {
            cout << "[" << i->first << "]  [" << i->second << "]" << endl;
        }
    }

    void printoutReverseMap() {
        for (multimap<Location, Coordinate>::iterator i = reversemap.begin(); i != reversemap.end(); ++i) {
            cout << "[" << i->first << "]  [" << i->second << "]" << endl;
        }
    }
};

mapGenerator::mapGenerator() {
    Location origin(0, 0);
    Location pointer(origin);
    claimNeighbors(origin);

    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            pointer.first += 1;
            pointer.second += 2;
            claimNeighbors(pointer);
        }
        origin.first  -= 2;
        origin.second += 3;
        claimNeighbors(origin);
        pointer = origin;
    }

    //for (map<Location, Location>::iterator i = bigmap.begin(); i != bigmap.end(); ++i) 
    //   reversemap.insert(pair<Location, Location>(i->second, i->first));
    

}

void mapGenerator::claimNeighbors(const Location& loc) { 
    
    Location pointer = loc;
    Coordinate center(loc.first, loc.second);
    (*this)[pointer] = center;
    --pointer.first;
    ++pointer.second;
    (*this)[pointer] = center;
    ++pointer.first;
    (*this)[pointer] = center;
    ++pointer.first;
    --pointer.second;
    (*this)[pointer] = center;
    --pointer.second;
    (*this)[pointer] = center;
    --pointer.first;
    (*this)[pointer] = center;
    --pointer.first;
    ++pointer.second;
    (*this)[pointer] = center;
}

void mapGenerator::setCenterPoints(int x, int y) {
    centerPoints = *this;
    centerPoints[Location(0, 0)] = Coordinate(x, y);
    
    for (map<Location, Coordinate>::iterator i = centerPoints.begin(); i != centerPoints.end(); ++i) {
        i->second.x =  (i->first.first  * HEX_WIDTH) + (i->first.second * HEX_WIDTH * 0.5) + x;
        i->second.y = -(i->first.second * 1.5 * HEX_SIDE_LENGTH) + y;
    }
}

class CrystalIndicator
    :public Gtk::EventBox
{
    size_t _crystals;
    crystal::CrystalColor _crystalColor;
    Gtk::Image _childImage;
    double _scale;
    Glib::RefPtr<Gdk::Pixbuf> _noCrystals;
    Glib::RefPtr<Gdk::Pixbuf> _oneCrystal;
    Glib::RefPtr<Gdk::Pixbuf> _twoCrystals;
    Glib::RefPtr<Gdk::Pixbuf> _threeCrystals;
    Glib::RefPtr<Gdk::Pixmap> pmap;
public:
    
    CrystalIndicator(crystal::CrystalColor c, double scale);
    void gainOne() {
        if (_crystals < 3)
            ++_crystals;
        update();
    }
    void loseOne() {
        if (_crystals != 0)
            --_crystals;
        update();
    }
    size_t getCrystals() const {return _crystals;}
    void update();
	bool on_click(GdkEventButton* e) { 
        if (e->type == Gdk::EventType::DOUBLE_BUTTON_PRESS || e->type == Gdk::EventType::TRIPLE_BUTTON_PRESS )
            return false;
        switch (e->button) {
        case 1:
            gainOne();
            break;
        case 3:
            loseOne();
            break;
        }
        return false; 
    }
    //void setScale(double d) {_scale = d;}
    double getScale() const {return _scale;}
    void rescale(double s) {;}                //TODO
};

CrystalIndicator::CrystalIndicator(crystal::CrystalColor c, double scale = 1.0) 
    :_crystals(0), _crystalColor(c), _scale(scale),
    _noCrystals(Gdk::Pixbuf::create_from_file(RootDirectory + "CrystalNone.png"))
{
   
    set_can_focus(false);
    
    int w = _noCrystals->get_width() * _scale; 
    int h = _noCrystals->get_height() * _scale; 
    //set_size_request(w, h);
    _noCrystals    = _noCrystals->scale_simple(w, h, Gdk::INTERP_HYPER);
    Glib::RefPtr<Gdk::Pixmap> pmap;
    Glib::RefPtr<Gdk::Pixbuf> bg;
    //bg = Gdk::Pixbuf::create_from_file(RootDirectory + "blueCrystalBackground.png")->scale_simple(w, h, Gdk::INTERP_HYPER);
    //bg->render_pixmap_and_mask(pmap, Glib::RefPtr<Gdk::Bitmap>(), 255);
    Glib::RefPtr<Gtk::Style> _style = get_style()->copy();
    //_style->set_bg_pixmap(Gtk::StateType::STATE_NORMAL, pmap);
    set_style(_style);
   
    switch (_crystalColor) {
    case crystal::BLUE:
        _oneCrystal    = Gdk::Pixbuf::create_from_file(RootDirectory + "CrystalBlueOne.png")  ->scale_simple(w, h, Gdk::INTERP_HYPER);
        _twoCrystals   = Gdk::Pixbuf::create_from_file(RootDirectory + "CrystalBlueTwo.png")  ->scale_simple(w, h, Gdk::INTERP_HYPER);
        _threeCrystals = Gdk::Pixbuf::create_from_file(RootDirectory + "CrystalBlueThree.png")->scale_simple(w, h, Gdk::INTERP_HYPER);
        bg = Gdk::Pixbuf::create_from_file(RootDirectory + "blueCrystalBackground.png");
        bg->render_pixmap_and_mask(pmap, Glib::RefPtr<Gdk::Bitmap>(), 255);
        get_style()->set_bg_pixmap(Gtk::StateType::STATE_NORMAL, pmap);
        break;
    case crystal::GREEN:
        _oneCrystal    = Gdk::Pixbuf::create_from_file(RootDirectory + "CrystalGreenOne.png")  ->scale_simple(w, h, Gdk::INTERP_HYPER);
        _twoCrystals   = Gdk::Pixbuf::create_from_file(RootDirectory + "CrystalGreenTwo.png")  ->scale_simple(w, h, Gdk::INTERP_HYPER);
        _threeCrystals = Gdk::Pixbuf::create_from_file(RootDirectory + "CrystalGreenThree.png") ->scale_simple(w, h, Gdk::INTERP_HYPER);
        bg =  Gdk::Pixbuf::create_from_file(RootDirectory + "greenCrystalBackground.png");
        bg->render_pixmap_and_mask(pmap, Glib::RefPtr<Gdk::Bitmap>(), 255);
        get_style()->set_bg_pixmap(Gtk::StateType::STATE_NORMAL, pmap);
        break;                                                                                 
    case crystal::RED:                                                                         
        _oneCrystal    = Gdk::Pixbuf::create_from_file(RootDirectory + "CrystalRedOne.png")    ->scale_simple(w, h, Gdk::INTERP_HYPER);
        _twoCrystals   = Gdk::Pixbuf::create_from_file(RootDirectory + "CrystalRedTwo.png")    ->scale_simple(w, h, Gdk::INTERP_HYPER);
        _threeCrystals = Gdk::Pixbuf::create_from_file(RootDirectory + "CrystalRedThree.png")  ->scale_simple(w, h, Gdk::INTERP_HYPER);
        bg =  Gdk::Pixbuf::create_from_file(RootDirectory + "redCrystalBackground.png");
        bg->render_pixmap_and_mask(pmap, Glib::RefPtr<Gdk::Bitmap>(), 255);
        get_style()->set_bg_pixmap(Gtk::StateType::STATE_NORMAL, pmap);
        break;                                                                                 
    case crystal::WHITE:                                                                       
        _oneCrystal    = Gdk::Pixbuf::create_from_file(RootDirectory + "CrystalWhiteOne.png")  ->scale_simple(w, h, Gdk::INTERP_HYPER);
        _twoCrystals   = Gdk::Pixbuf::create_from_file(RootDirectory + "CrystalWhiteTwo.png")  ->scale_simple(w, h, Gdk::INTERP_HYPER);
        _threeCrystals = Gdk::Pixbuf::create_from_file(RootDirectory + "CrystalWhiteThree.png") ->scale_simple(w, h, Gdk::INTERP_HYPER);
        bg =  Gdk::Pixbuf::create_from_file(RootDirectory + "whiteCrystalBackground.png")->scale_simple(w, h, Gdk::INTERP_HYPER);
        bg->render_pixmap_and_mask(pmap, Glib::RefPtr<Gdk::Bitmap>(), 255);
        get_style()->set_bg_pixmap(Gtk::StateType::STATE_NORMAL, pmap);
        break;
    }
    update();
    signal_button_press_event().connect(sigc::mem_fun(*this, &CrystalIndicator::on_click), false);
    add(_childImage);

}

void CrystalIndicator::update() {
   
    switch (_crystals) {
    case 0:
        _childImage.set(_noCrystals);
        break;
    case 1:
        _childImage.set(_oneCrystal);
        break;
    case 2:
        _childImage.set(_twoCrystals);
        break;
    case 3:
        _childImage.set(_threeCrystals);
        break;
    }
}

class ManaTokenIndicator
    :public Gtk::TextView
{
    Pango::FontDescription _font;
    ManaColor _color;
public:
    
    ManaTokenIndicator(ManaColor c);
    void setTokens(size_t n) {
        ostringstream oss;
        oss << n;
        get_buffer()->set_text(oss.str());
    }
    size_t getTokens() const {
        char c[16]; 
        strcpy(c, get_buffer()->get_text().c_str());
        return atoi(c);
    }
    void increase() {setTokens(getTokens() + 1);}
    void decrease() {if (getTokens()) setTokens(getTokens() - 1); }
  
    bool on_click(GdkEventButton *e) {
        if (e->type == GdkEventType::GDK_2BUTTON_PRESS || e->type == GdkEventType::GDK_3BUTTON_PRESS )
            return true;
        switch (e->button) {
        case 1:
            increase();
            break;
        case 3:
            decrease();
            break;
        }
        return true;
    }
};

ManaTokenIndicator::ManaTokenIndicator(ManaColor c)
    : _color(c)
{
    get_buffer()->set_text("0");
    Gdk::Color _baseColor;
    Gdk::Color _textColor;
    
    switch (_color) {
    case BLACK:
        _baseColor = Gdk::Color("#000000");
        _textColor = Gdk::Color("#FFFFFF");
        break;
    case GOLD:
        _baseColor = Gdk::Color("#FBCC0D");
        _textColor = Gdk::Color("#000000");
        break;
    case BLUE:
        _baseColor = Gdk::Color("#0000FF");
        _textColor = Gdk::Color("#FFFFFF");
        break;
    case RED:
        _baseColor = Gdk::Color("#FF0000");
        _textColor = Gdk::Color("#FFFFFF");
        break;
    case WHITE:
        _baseColor = Gdk::Color("#FFFFFF");
        _textColor = Gdk::Color("#000000");
        break;
    case GREEN:
        _baseColor = Gdk::Color("#00A600");
        _textColor = Gdk::Color("#FFFFFF");
        break;
    }
    _font.set_family("Sakkal Majalla");
    _font.set_size(PANGO_SCALE * 24);
    modify_font(_font);
    modify_base(Gtk::STATE_NORMAL, _baseColor);
    modify_text(Gtk::STATE_NORMAL, _textColor);
    set_justification(Gtk::JUSTIFY_CENTER);
    set_pixels_above_lines(10); //or border instead?
    set_pixels_below_lines(0); 
    set_can_focus(false);
    set_editable(false);
    show();
    signal_button_press_event().connect(sigc::mem_fun(*this, &ManaTokenIndicator::on_click), false);
}

class Die
    :public Gtk::ToggleButton
{
    Gtk::Image _childImage;
    double _scale;
    ManaColor _manaColor;
    Glib::RefPtr<Gdk::Pixbuf> _borderPix;
	Glib::RefPtr<Gdk::Pixbuf> _blankPix;
    std::map<ManaColor, Glib::RefPtr<Gdk::Pixbuf> > _colorPix;

    Glib::RefPtr<Gtk::ActionGroup> _actionGroup;
    Glib::RefPtr<Gtk::UIManager> _UIManager;
   
	Gtk::Menu* _menuPopup;
    Gtk::Window newDieWindow;
    Gtk::Window* homeWindow;
	Gtk::Window* mainGameWindow;

	sigc::connection _menuSignal;

public:
	
	void setGameWindow(Gtk::Window* window) {
		mainGameWindow = window;
	}
    Die(double s);
    void roll(bool asNewDie = false) {
        int r = rand() % 6;
        switch (r) {
        case BLUE:
            _manaColor = BLUE;
            break;
        case RED:
            _manaColor = RED;
            break;
        case GREEN:
            _manaColor = GREEN;
            break;
        case WHITE:
            _manaColor = WHITE;
            break;
        case GOLD:
            _manaColor = GOLD;
            break;
        case BLACK:
            _manaColor = BLACK;
            break;
        }
		_childImage.set(_blankPix);
		set_active(false);
		Glib::signal_timeout().connect_once(sigc::mem_fun(*this, &Die::updateAfterRoll), (asNewDie ? 0 : 500) );
	}

	void updateAfterRoll() {
		_childImage.set(_colorPix[_manaColor]->copy());
		if (homeWindow != NULL)
			homeWindow->set_icon(getChildPixbuf());
	}

    void on_any_toggle() {
        if (get_active()) {
            _childImage.set(_colorPix[_manaColor]->copy());
            _borderPix->composite(_childImage.get_pixbuf(), 0, 0, _childImage.get_pixbuf()->get_width(), _childImage.get_pixbuf()->get_height(), 0, 0, 1, 1, Gdk::INTERP_HYPER, 255);
            _childImage.set(_childImage.get_pixbuf());
        }
        else {
            _childImage.set(_colorPix[_manaColor]->copy());
        }
    }
    void setColor(ManaColor c);
    ManaColor getColor() const {return _manaColor;}
    Glib::RefPtr<Gdk::Pixbuf> getChildPixbuf() const {
        return _colorPix.at(_manaColor);
    }
    bool on_right_click(GdkEventButton* e) {
        if (e->button == 3) { 
            _menuPopup->popup(e->button, e->time);
			dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Choose/Blue"))-> set_sensitive(_manaColor != BLUE); 
			dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Choose/Red"))->  set_sensitive(_manaColor != RED); 
			dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Choose/Green"))->set_sensitive(_manaColor != GREEN); 
			dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Choose/White"))->set_sensitive(_manaColor != WHITE); 
			dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Choose/Gold"))-> set_sensitive(_manaColor != GOLD); 
			dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Choose/Black"))->set_sensitive(_manaColor != BLACK); 
			return true;
        }
        return false;
    }
    void onDelete(GdkEventAny* e, Gtk::Window* win) { delete win;}
    void makeNewDie() {
       
		Die *newDie = Gtk::manage(new Die(1));
		Gtk::Window *tempWindow = new Gtk::Window;
        newDie->homeWindow = tempWindow;
		newDie->mainGameWindow = mainGameWindow;
        tempWindow->add(*newDie);
        tempWindow->set_border_width(10);
        tempWindow->set_title("Roll for new mana");
        tempWindow->set_position(Gtk::WindowPosition::WIN_POS_CENTER_ALWAYS);
		tempWindow->set_transient_for(*mainGameWindow);
		tempWindow->set_icon(newDie->getChildPixbuf());
        tempWindow->show_all();
		tempWindow->signal_delete_event().connect_notify(sigc::bind<Gtk::Window*>(sigc::mem_fun(*this, &Die::onDelete), tempWindow));
	}
 
	void manaSteal() {
		
		Die *newDie = Gtk::manage(new Die(1));
		newDie->homeWindow = &newDieWindow;
		newDie->mainGameWindow = mainGameWindow;
		newDie->setColor(_manaColor);
        newDieWindow.remove();
        newDieWindow.add(*newDie);
        newDieWindow.set_border_width(10);
        newDieWindow.set_title("Mana steal");
		newDieWindow.move(1516, 60);
		newDieWindow.set_transient_for(*mainGameWindow);
		newDieWindow.set_icon(newDie->getChildPixbuf());
        newDieWindow.show_all();
		hide_all();
		newDie->_menuSignal.disconnect();
		newDie->homeWindow->signal_delete_event().connect_notify(sigc::mem_fun(*this, &Die::manaUnsteal));
		
	}
	void manaUnsteal(GdkEventAny* e) {
		show_all();
	}
	
};

Die::Die(double s = 1)
    :_scale(s), homeWindow(NULL)//, manaStealFunctionPtr(NULL)
{
    set_relief(Gtk::RELIEF_HALF);
    set_can_focus(false);
    set_mode(false);
    _actionGroup = Gtk::ActionGroup::create("Choose");
    _actionGroup->add( Gtk::Action::create("Roll this die", "Roll this die"), (sigc::bind<bool>(sigc::mem_fun(*this, &Die::roll), false)));
    _actionGroup->add( Gtk::Action::create("Set to:", "Set to:"));
    _actionGroup->add( Gtk::Action::create("Blue", "Blue"),   sigc::bind<ManaColor>(sigc::mem_fun(*this, &Die::setColor), BLUE ) );
    _actionGroup->add( Gtk::Action::create("Red", "Red"),     sigc::bind<ManaColor>(sigc::mem_fun(*this, &Die::setColor), RED  ) );
    _actionGroup->add( Gtk::Action::create("Green", "Green"), sigc::bind<ManaColor>(sigc::mem_fun(*this, &Die::setColor), GREEN) );
    _actionGroup->add( Gtk::Action::create("White", "White"), sigc::bind<ManaColor>(sigc::mem_fun(*this, &Die::setColor), WHITE) );
    _actionGroup->add( Gtk::Action::create("Gold", "Gold"),   sigc::bind<ManaColor>(sigc::mem_fun(*this, &Die::setColor), GOLD ) );
    _actionGroup->add( Gtk::Action::create("Black", "Black"), sigc::bind<ManaColor>(sigc::mem_fun(*this, &Die::setColor), BLACK) );
    _actionGroup->add( Gtk::Action::create("Create new die", "Create new die"),    (sigc::mem_fun(*this, &Die::makeNewDie)));
	_actionGroup->add( Gtk::Action::create("Mana steal", "Mana steal"),            (sigc::mem_fun(*this, &Die::manaSteal)));
	

    _UIManager = Gtk::UIManager::create();
    _UIManager->insert_action_group(_actionGroup);
    
    Glib::ustring ui_info = 
        "<ui>"
        "  <popup name='Choose'>"
        "    <menuitem action='Roll this die'/>"
		"    <separator/>"
        "    <menuitem action='Set to:'/>"
		"    <menuitem action='Blue'/>"
        "    <menuitem action='Red'/>"
        "    <menuitem action='Green'/>"
        "    <menuitem action='White'/>"
        "    <menuitem action='Gold'/>"
        "	 <menuitem action='Black'/>"
        "    <separator/>"
        "    <menuitem action='Create new die'/>"
		"    <separator/>"
		"    <menuitem action='Mana steal'/>"
		"  </popup>"
        "</ui>";
    try {  
        _UIManager->add_ui_from_string(ui_info);
    }
    catch(const Glib::Error& ex) {
      cout << "building menus failed: " <<  ex.what();
    }
   
   
    _menuPopup = dynamic_cast<Gtk::Menu*>(_UIManager->get_widget("/Choose")); 
    _menuPopup->show_all();
	
    _borderPix = Gdk::Pixbuf::create_from_file(RootDirectory + "dieBorder.png");
	_blankPix = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, _borderPix->get_width() * _scale, _borderPix->get_height() * _scale);
	_blankPix->fill(0);
    _colorPix[BLUE ] = (Gdk::Pixbuf::create_from_file(RootDirectory + "die_blue.jpg") ->scale_simple(_borderPix->get_width() * _scale, _borderPix->get_height() * _scale, Gdk::INTERP_HYPER));
    _colorPix[RED  ] = (Gdk::Pixbuf::create_from_file(RootDirectory + "die_red.jpg")  ->scale_simple(_borderPix->get_width() * _scale, _borderPix->get_height() * _scale, Gdk::INTERP_HYPER));
    _colorPix[GREEN] = (Gdk::Pixbuf::create_from_file(RootDirectory + "die_green.jpg")->scale_simple(_borderPix->get_width() * _scale, _borderPix->get_height() * _scale, Gdk::INTERP_HYPER));
    _colorPix[WHITE] = (Gdk::Pixbuf::create_from_file(RootDirectory + "die_white.jpg")->scale_simple(_borderPix->get_width() * _scale, _borderPix->get_height() * _scale, Gdk::INTERP_HYPER));
    _colorPix[GOLD ] = (Gdk::Pixbuf::create_from_file(RootDirectory + "die_gold.jpg") ->scale_simple(_borderPix->get_width() * _scale, _borderPix->get_height() * _scale, Gdk::INTERP_HYPER));
    _colorPix[BLACK] = (Gdk::Pixbuf::create_from_file(RootDirectory + "die_black.jpg")->scale_simple(_borderPix->get_width() * _scale, _borderPix->get_height() * _scale, Gdk::INTERP_HYPER));
    _borderPix = _borderPix->scale_simple(_borderPix->get_width() * _scale, _borderPix->get_height() * _scale, Gdk::INTERP_HYPER);
    add(_childImage);
	roll(true);
    
    show_all_children();
    
	_menuSignal = signal_button_press_event().connect(sigc::mem_fun(*this, &Die::on_right_click), false);
    signal_toggled().connect(sigc::mem_fun(*this, &Die::on_any_toggle));

}

void Die::setColor(ManaColor c)
{
    _manaColor = c; 
    _childImage.set(_colorPix[_manaColor]->copy());
    if (get_active()) {
        _borderPix->composite(_childImage.get_pixbuf(), 0, 0, _childImage.get_pixbuf()->get_width(), _childImage.get_pixbuf()->get_height(), 0, 0, 1, 1, Gdk::INTERP_HYPER, 255);
        _childImage.set(_childImage.get_pixbuf());
    }
	if (homeWindow != NULL)
		homeWindow->set_icon(getChildPixbuf());
}

class Source
    :public Gtk::HBox
{
    
public:
    Die die1;
    Die die2;
    Die die3;
    
    Source(double scale);

    void rollSelectedDice_EndOfTurn() {
        if (die1.get_active()) die1.roll();
        if (die2.get_active()) die2.roll();
        if (die3.get_active()) die3.roll();
    }
    void rollForEndOfRound() {
        do {
            die1.roll(true);
            die2.roll(true);
            die3.roll(true);
        } while (getBasicColorCount() < 2);
    }
    size_t getBasicColorCount() {
        size_t result = 0;
        if (die1.getColor() != ManaColor::BLACK && die1.getColor() != ManaColor::GOLD) ++result;
        if (die2.getColor() != ManaColor::BLACK && die2.getColor() != ManaColor::GOLD) ++result;
        if (die3.getColor() != ManaColor::BLACK && die3.getColor() != ManaColor::GOLD) ++result;
        return result;
    }

	void setGameWindow(Gtk::Window* gameWindow) {
		die1.setGameWindow(gameWindow);
		die2.setGameWindow(gameWindow);
		die3.setGameWindow(gameWindow);
	}
};

Source::Source(double scale = 1) 
    :die1(scale), die2(scale), die3(scale)
{
    srand(unsigned int(time(NULL)));
    pack_start(die1, Gtk::PACK_EXPAND_PADDING);
    pack_start(die2, Gtk::PACK_EXPAND_PADDING);
    pack_start(die3, Gtk::PACK_EXPAND_PADDING);
    
    rollForEndOfRound();
    show_all();
}

class Card
{
protected:
    string _name;
    Glib::RefPtr<Gdk::Pixbuf> _childPixbuf;
    Card(const string& s = "")
        :_name(s) {}
public:
    void setName(const string& s) {_name = s;}
    string getName() const        {return _name;}
};

class Deed
    :public Card
{
public:
    enum Color {BLUE, RED, GREEN, WHITE, NO_COLOR};
    enum Type {BASIC_ACTION, ADVANCED_ACTION, SPELL, ARTIFACT, WOUND};

private:
    
    Color _color;
    Type _type;
    static Glib::RefPtr<Gdk::Pixbuf> _woundPixbuf;
public:
    
    Deed()
        :_color(NO_COLOR), _type(WOUND) {}
    Deed(const string& s, Color c, Type t);
        
    void setChildPixbuf(const string& s) {
        _childPixbuf = Gdk::Pixbuf::create_from_file(s, FULLSIZE_CARD_WIDTH, FULLSIZE_CARD_HEIGHT);
    }

    Glib::RefPtr<Gdk::Pixbuf> getChildPixbuf() const {
        if (_type == WOUND)
            return _woundPixbuf;
        return _childPixbuf;
    }
    Glib::RefPtr<Gdk::Pixbuf> getScaledChildPixbuf(double d = 1) const {
        return getChildPixbuf()->scale_simple(FULLSIZE_CARD_WIDTH * d, FULLSIZE_CARD_HEIGHT * d, Gdk::INTERP_HYPER);
    }

    bool isWound() const {return _type == WOUND;}
    bool isBanner() const {
        return (_name.find("Banner of") != string::npos);
    }

    Color getColor() const {return _color;}
    Type getType() const {return _type;}

};

Glib::RefPtr<Gdk::Pixbuf> Deed::_woundPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "wound.jpg", FULLSIZE_CARD_WIDTH, FULLSIZE_CARD_HEIGHT, true);

Deed::Deed(const string& s, Color c, Type t = BASIC_ACTION) //default: t = BASIC_ACTION
    :Card(s), _color(c), _type(t) 
{
    string _imageFilename = RootDirectory;
    switch (_type) {
    case BASIC_ACTION:
        _imageFilename += "deed_basic_";
        break;
    case ADVANCED_ACTION:
        _imageFilename += "adv_action_";
        break;
    case SPELL:
        _imageFilename += "spell_";
        break;
    case ARTIFACT:
        _imageFilename += "artifact_";
        break;
    }
    string temp = _name;
    for (string::iterator i = temp.begin(); i != temp.end(); ++i) {
        if (isupper(*i)) {
            *i = char(tolower(*i));
        }
        if (*i == ' ') {
            *i = '_';
        } 
    }
    _imageFilename += temp + ".jpg";
    try {
        _childPixbuf = Gdk::Pixbuf::create_from_file(_imageFilename, FULLSIZE_CARD_WIDTH, FULLSIZE_CARD_HEIGHT);
    }
    catch (Glib::Error c) {
        cout << c.what() << endl;
    }
}

class Unit
    :public Card
{
public:
    enum Recruitment {
        VILLAGE      = 1,
        MONASTERY    = 1 << 1,
        MAGE_TOWER   = 1 << 2,
        KEEP         = 1 << 3,
        CITY         = 1 << 4 
    };
    enum Resistance {
        NO_RESISTANCE,
        PHYSICAL_RESISTANCE = 1,
        FIRE_RESISTANCE =     1 << 1,
        ICE_RESISTANCE  =     1 << 2, 
        FIRE_AND_ICE_RESISTANCE = FIRE_RESISTANCE | ICE_RESISTANCE
    };
private:
    size_t _level;
    size_t _armor;
    size_t _cost;
    size_t _recruitment;
    size_t _resistance;
    size_t _wounds;
    list<Deed> _banner;
    bool _ready;
	bool _bannerReady;
public:
    Unit()
        :_level(0), _armor(0), _cost(0), _recruitment(VILLAGE), _resistance(NO_RESISTANCE), _wounds(0), _ready(true), _bannerReady(true) {}
    Unit(const string& s, size_t level, size_t a, size_t c, size_t recruit, size_t resist);

    void setChildPixbuf(const string& s) {
        _childPixbuf = Gdk::Pixbuf::create_from_file(s, FULLSIZE_CARD_WIDTH, FULLSIZE_CARD_HEIGHT);
    }

    Glib::RefPtr<const Gdk::Pixbuf> getChildPixbuf() const {
        return _childPixbuf;
    }
    Glib::RefPtr<Gdk::Pixbuf> getChildPixbuf() { return _childPixbuf->copy(); }
    Glib::RefPtr<Gdk::Pixbuf> getScaledChildPixbuf(double d = 1) const {
        return getChildPixbuf()->scale_simple(FULLSIZE_CARD_WIDTH * d, FULLSIZE_CARD_HEIGHT * d, Gdk::INTERP_HYPER);
    }
       
    bool isSilver() { return _level == 1 || _level == 2;}
    bool isGold()   { return _level == 3 || _level == 4;}

    size_t getLevel()        {return _level;}
    size_t getArmor()        {return _armor;}
    size_t getCost()         {return _cost;}
    size_t getRecruitment()  {return _recruitment;}
    size_t getResistance()   {return _resistance;}

    bool isAvailableAt(Recruitment r) {
        return r & _recruitment;
    }
    bool has(Resistance r) {
        return r & _resistance;
    }

    void wound()             {if (_wounds < 2) ++_wounds;}
    void woundTwice()        {_wounds = 2;}
    void heal()              {if (_wounds > 0) --_wounds;}
    void healTwice()         {_wounds = 0;}
    size_t getWounds() const {return _wounds;}

    void use()           {_ready = false;}
    void ready()         {_ready = true;}
    bool isReady() const {return _ready;}
	
    bool hasBanner() const {return _banner.size() != 0;}
    const Deed& getBanner() const {return _banner.front();}
    void attachBanner(const Deed& d) { 
        removeBanner();
        _banner.push_back(d);
		_bannerReady = true;
    }
    void removeBanner() {
		_banner.clear();
		_bannerReady = true;
	}
	void useBanner()   {_bannerReady = false;}
	void readyBanner() {_bannerReady = true;}
	bool bannerIsReady() const {return _bannerReady;}

    Glib::RefPtr<Gdk::Pixbuf> getBannerPixbuf() const {
        if (_banner.empty())
            return Glib::RefPtr<Gdk::Pixbuf>();
        return _banner.front().getChildPixbuf();
    }

    void clearForDiscard() {
        _wounds = 0;
        ready();
        removeBanner();
    }
  
};

Unit::Unit(const string& s, size_t level, size_t armor, size_t cost, size_t recruit, size_t resist = 0)
    :Card(s), _level(level), _armor(armor), _cost(cost), _recruitment(recruit), _resistance(resist), _wounds(0), _ready(true)
{
    string _imageFilename = RootDirectory;
    switch (_level) {
    case 1:
    case 2:
        _imageFilename += "regular_";
        break;
    case 3:
    case 4:
        _imageFilename += "elite_";
        break;
    }
    string temp = _name;
    for (string::iterator i = temp.begin(); i != temp.end(); ++i) {
        if (isupper(*i)) {
            *i = char(tolower(*i));
        }
        if (*i == ' ') {
            *i = '_';
        } 
    }

    _imageFilename += temp + ".jpg";
    try {
        _childPixbuf = Gdk::Pixbuf::create_from_file(_imageFilename, FULLSIZE_CARD_WIDTH, FULLSIZE_CARD_HEIGHT);
    }
    catch (Glib::Error c) {
       cout << c.what() << endl;
    }
    
}

const size_t bondOfLoyaltySlotNumber = 5;

class UnitButton
    :public Gtk::ToggleButton
{
    Gtk::Image _childImage;
    Glib::RefPtr<Gdk::Pixbuf> _emptyPixbuf;
    Glib::RefPtr<Gdk::Pixbuf> _wound;
    Glib::RefPtr<Gdk::Pixbuf> _twoWounds;
	Glib::RefPtr<Gdk::Pixbuf> _faceDownBannerPixbuf;
    vector<Unit>* _retinuePointer; 
    size_t _slot;
    double _scale;

	Glib::RefPtr<Gtk::ActionGroup> _actionGroup;
    Glib::RefPtr<Gtk::UIManager>   _UIManager;
	Gtk::Menu* _menuPopup;
    
public:
    sigc::signal<void, const Deed&> bannerSignal;
	sigc::signal<void, size_t> disbandSignal;

    UnitButton(vector<Unit>* _vectorPointer, size_t slotNumber, double scale);
    
	bool slotIsValid() const {
		return _retinuePointer != NULL && _slot < _retinuePointer->size();
	}

    void sendBannerSignal() { 
        if (slotIsValid() && _retinuePointer->at(_slot).hasBanner()) { 
            bannerSignal.emit(_retinuePointer->at(_slot).getBanner()); 
        }
	}
	void sendDisbandSignal() {
		sendBannerSignal();
		disbandSignal.emit(_slot);
	}
	
    void setScale(double scale) {
        _scale = scale;
        _emptyPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "regular_unit_back.jpg", FULLSIZE_CARD_WIDTH * _scale, FULLSIZE_CARD_HEIGHT * _scale, true);
        _emptyPixbuf->saturate_and_pixelate(_emptyPixbuf, 0, 0);
        _wound = Gdk::Pixbuf::create_from_file(RootDirectory + "unit_wounded.png", FULLSIZE_CARD_WIDTH * _scale, FULLSIZE_CARD_HEIGHT * _scale, true);
        _twoWounds = Gdk::Pixbuf::create_from_file(RootDirectory + "unit_wounded2.png", FULLSIZE_CARD_WIDTH * _scale, FULLSIZE_CARD_HEIGHT * _scale, true);
        _childImage.set(_emptyPixbuf);
    }
    double getScale() const {return _scale;}

    void setPointer(vector<Unit>& v) {_retinuePointer = &v;}

    void update();

    bool onMouseOver(int x, int y, bool, const Glib::RefPtr<Gtk::Tooltip>& tooltip) {
        if (!slotIsValid()) 
            return false;
        if (_retinuePointer->at(_slot).hasBanner() && x < get_width() / 2 && get_height() * .15 < y  && y < get_height() * .4)
            tooltip->set_icon(_retinuePointer->at(_slot).getBanner().getChildPixbuf());
        else
            tooltip->set_icon(_retinuePointer->at(_slot).getChildPixbuf());
        return true;
    }

    void onToggle() {
        if (_retinuePointer == NULL || _slot >= _retinuePointer->size())
            return;
        if (get_active()) 
            _retinuePointer->at(_slot).use();
        else
            _retinuePointer->at(_slot).ready();
        update();
    }

	bool onRightClick(GdkEventButton* e) {
		_menuPopup->hide_all();
		
		if (e->button == 3) {
			dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Unit options/Heal"))->show_all(); 
			dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Unit options/Heal"))->set_sensitive(_retinuePointer->at(_slot).getWounds() > 0);
			dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Unit options/Wound"))->show_all(); 
			dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Unit options/Wound"))->set_sensitive(_retinuePointer->at(_slot).getWounds() < 2);

			if (_retinuePointer->at(_slot).hasBanner()) {
				dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Unit options/Discard banner"))->show_all(); 
				if (_retinuePointer->at(_slot).getBanner().getName() == "Banner of Courage") {
					if (_retinuePointer->at(_slot).bannerIsReady())
						dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Unit options/Use Banner of Courage"))->show_all(); 
					else
						dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Unit options/Ready Banner of Courage"))->show_all(); 
				}
			}
			dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Unit options/Disband"))->show_all(); 
			_menuPopup->popup(e->button, e->time);
		}
		return false;
	}
	/*
    bool onWound(GdkEventButton* e) {
        if (_retinuePointer == NULL || _slot >= _retinuePointer->size())
            return false;
        if (e->button != 3 || e->type != GDK_BUTTON_PRESS)
            return false;
        if (_retinuePointer->at(_slot).getWounds() == 2)
            _retinuePointer->at(_slot).healTwice();
        else 
            _retinuePointer->at(_slot).wound();
        update();
        return true;
    }
	*/
	void heal()  {
		_retinuePointer->at(_slot).heal();
		update();
	}

	void wound() {
		_retinuePointer->at(_slot).wound();
		update();
	}

	void attachBanner(const Deed& banner) {
		_retinuePointer->at(_slot).attachBanner(banner);
		update();
	}

    void removeBanner() {
		bannerSignal.emit(_retinuePointer->at(_slot).getBanner());
		_retinuePointer->at(_slot).removeBanner();
		update();
	}

	void useBanner() {
		_retinuePointer->at(_slot).useBanner();
		update();
	}

	void readyBanner() {
		_retinuePointer->at(_slot).readyBanner();
		update();
	}

	void modifyBondedMenu() {
		Gtk::MenuItem* item = dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Unit options/Disband"));
		item->set_label("Send this poor, loyal unit to his death.");
	}

    //recruit
};

UnitButton::UnitButton(vector<Unit>* _vectorPointer = NULL, size_t slotNumber = 0, double scale = 1.0)
    :_retinuePointer(_vectorPointer), _slot(slotNumber), _scale(scale)
{
    set_has_tooltip(true);
    _emptyPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "regular_unit_back.jpg", FULLSIZE_CARD_WIDTH * _scale, FULLSIZE_CARD_HEIGHT * _scale, true);
    _emptyPixbuf->saturate_and_pixelate(_emptyPixbuf, 0, 1);
	_faceDownBannerPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "deed_back.jpg", FULLSIZE_CARD_WIDTH * _scale, FULLSIZE_CARD_HEIGHT * _scale, true);
	_faceDownBannerPixbuf->saturate_and_pixelate(_faceDownBannerPixbuf, 0, 1);
    add(_childImage);
    _childImage.set(_emptyPixbuf);
    _wound = Gdk::Pixbuf::create_from_file(RootDirectory + "unit_wounded.png", FULLSIZE_CARD_WIDTH * _scale, FULLSIZE_CARD_HEIGHT * _scale, true);
    _twoWounds = Gdk::Pixbuf::create_from_file(RootDirectory + "unit_wounded2.png", FULLSIZE_CARD_WIDTH * _scale, FULLSIZE_CARD_HEIGHT * _scale, true);
    signal_query_tooltip().connect(sigc::mem_fun(*this, &UnitButton::onMouseOver));
    signal_clicked().connect(sigc::mem_fun(*this, &UnitButton::onToggle));
	signal_button_press_event().connect(sigc::mem_fun(*this, &UnitButton::onRightClick), false);

	_actionGroup = Gtk::ActionGroup::create("Unit options");
	_actionGroup->add(Gtk::Action::create("Heal", "Heal"),   sigc::mem_fun(*this, &UnitButton::heal));
	_actionGroup->add(Gtk::Action::create("Wound", "Wound"), sigc::mem_fun(*this, &UnitButton::wound));
	_actionGroup->add(Gtk::Action::create("Discard banner", "Discard banner"), sigc::mem_fun(*this, &UnitButton::removeBanner));
	_actionGroup->add(Gtk::Action::create("Use Banner of Courage", "Use Banner of Courage"), sigc::mem_fun(*this, &UnitButton::useBanner));
	_actionGroup->add(Gtk::Action::create("Ready Banner of Courage", "Ready Banner of Courage"), sigc::mem_fun(*this, &UnitButton::readyBanner));
	_actionGroup->add(Gtk::Action::create("Disband", "Disband / Kill"), sigc::mem_fun(*this, &UnitButton::sendDisbandSignal)); //for bonded, change to "Kill this poor, loyal unit

	_UIManager = Gtk::UIManager::create();
	_UIManager->insert_action_group(_actionGroup);

	Glib::ustring ui_info = 
		"<ui>"
        "  <popup name='Unit options'>"
		"    <menuitem action='Heal'/>"
        "    <menuitem action='Wound'/>"
		"    <menuitem action='Discard banner'/>"
		"    <menuitem action='Use Banner of Courage'/>"
		"    <menuitem action='Ready Banner of Courage'/>"
		"    <separator/>"
		"    <separator/>"
		"    <menuitem action='Disband'/>"
		"  </popup>"
        "</ui>";
    try {  
        _UIManager->add_ui_from_string(ui_info);
	}
	catch (const Glib::Error& ex) {
        cout << "building menus failed: " <<  ex.what();
    }
	_menuPopup = dynamic_cast<Gtk::Menu*>(_UIManager->get_widget("/Unit options")); 
	_menuPopup->show_all();

}

void UnitButton::update() 
{
    if (_retinuePointer == NULL || _slot >= _retinuePointer->size()) {
        _childImage.set(_emptyPixbuf);
        set_sensitive(false);
    }
    else {
        set_sensitive(true);
        _childImage.set(_retinuePointer->at(_slot).getScaledChildPixbuf(_scale));
        switch (_retinuePointer->at(_slot).getWounds()) {
        case 2:
            _twoWounds->composite(_childImage.get_pixbuf(), 0, 0, _childImage.get_pixbuf()->get_width(), _childImage.get_pixbuf()->get_height(), 0, 0, 1, 1, Gdk::INTERP_HYPER, 255);
            break;
        case 1:
            _wound->composite(_childImage.get_pixbuf(), 0, 0, _childImage.get_pixbuf()->get_width(), _childImage.get_pixbuf()->get_height(), 0, 0, 1, 1, Gdk::INTERP_HYPER, 255);
            break;
        }
        if (_retinuePointer->at(_slot).hasBanner()) {
			Glib::RefPtr<Gdk::Pixbuf> banner;
			if (_retinuePointer->at(_slot).bannerIsReady()) {
				banner = _retinuePointer->at(_slot).getBannerPixbuf()->scale_simple(_childImage.get_pixbuf()->get_width(), _childImage.get_pixbuf()->get_height(),  Gdk::INTERP_HYPER);
			}
			else {
				banner = _faceDownBannerPixbuf->scale_simple(_childImage.get_pixbuf()->get_width(), _childImage.get_pixbuf()->get_height(),  Gdk::INTERP_HYPER);
			}
			banner->composite(_childImage.get_pixbuf(), 
			    0, _childImage.get_pixbuf()->get_height() * .15, 
			    banner->get_width() / 2, _childImage.get_pixbuf()->get_height() * .40,
			    0, _childImage.get_pixbuf()->get_height() * .15, 
			    .5, .5, Gdk::INTERP_HYPER, 255);
		}
        if (!_retinuePointer->at(_slot).isReady()) {
            _childImage.get_pixbuf()->saturate_and_pixelate(_childImage.get_pixbuf(), 0, 1);
        }
        _childImage.set(_childImage.get_pixbuf());
    }
}

class UnitRadioButton
    :public Gtk::RadioButton
{
    Gtk::Image _childImage;
    Glib::RefPtr<Gdk::Pixbuf> _emptyPixbuf;
    //Glib::RefPtr<Gdk::Pixbuf> _selectionBorderPixbuf;
    size_t _slot;
    double _scale;
    deque<Unit>* _dequePointer;
public:
    UnitRadioButton(deque<Unit>* dequePointer = NULL, size_t slotNumber = 0, double scale = 1.0)
        :_dequePointer(dequePointer), _slot(slotNumber), _scale(scale) 
    {
        set_has_tooltip(true);
        signal_query_tooltip().connect(sigc::mem_fun(*this, &UnitRadioButton::onMouseOver));
        //signal_clicked().connect(sigc::mem_fun(*this, &UnitRadioButton::onToggle));
        _emptyPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "regular_unit_back.jpg", FULLSIZE_CARD_WIDTH * _scale, FULLSIZE_CARD_HEIGHT * _scale, true);
        //_selectionBorderPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "selectionBorder.png", FULLSIZE_CARD_WIDTH * _scale, FULLSIZE_CARD_HEIGHT * _scale, true);
        //_selectionBorderPixbuf = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, FULLSIZE_CARD_WIDTH * _scale, FULLSIZE_CARD_HEIGHT * _scale);
        //_selectionBorderPixbuf->fill(0x00000000);
        _emptyPixbuf->saturate_and_pixelate(_emptyPixbuf, 0, 1);
        add(_childImage);
        _childImage.set(_emptyPixbuf);
    }
    
    bool onMouseOver(int x, int y, bool, const Glib::RefPtr<Gtk::Tooltip>& tooltip) {
        if (_dequePointer == NULL || _slot >= _dequePointer->size()) 
            return false;
        tooltip->set_icon(_dequePointer->at(_slot).getChildPixbuf());
        return true;
    }

    void setScale(double scale) {
        _scale = scale;
        _emptyPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "regular_unit_back.jpg", FULLSIZE_CARD_WIDTH * _scale, FULLSIZE_CARD_HEIGHT * _scale, true);
        //_selectionBorderPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "selectionBorder.png", FULLSIZE_CARD_WIDTH * _scale, FULLSIZE_CARD_HEIGHT * _scale, true);
        //_selectionBorderPixbuf = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, FULLSIZE_CARD_WIDTH * _scale, FULLSIZE_CARD_HEIGHT * _scale);
        //_selectionBorderPixbuf->fill(0x00000000);
        _emptyPixbuf->saturate_and_pixelate(_emptyPixbuf, 0, 0);
        _childImage.set(_emptyPixbuf);
    }
    double getScale() const {return _scale;}

    void setPointer(deque<Unit>& d) {_dequePointer = &d;}

    void onToggle() {
        if (_dequePointer == NULL || _slot >= _dequePointer->size())
            return;
        if (get_active()) {
            //_selectionBorderPixbuf->composite(_childImage.get_pixbuf(), 0, 0, FULLSIZE_CARD_WIDTH * _scale, FULLSIZE_CARD_HEIGHT * _scale, 0, 0, 1, 1, Gdk::INTERP_HYPER, 255);
            _childImage.set(_childImage.get_pixbuf());
        }
        else
            _childImage.set(_dequePointer->at(_slot).getScaledChildPixbuf(_scale));
    }

    void update();
};

void UnitRadioButton::update() {
    if (_dequePointer == NULL || _slot >= _dequePointer->size()) {
        _childImage.set(_emptyPixbuf);
        if (_slot > 2)
            hide();
        else
            set_sensitive(false);
    }
    else {
        show_all();
        set_sensitive(true);
        _childImage.set(_dequePointer->at(_slot).getScaledChildPixbuf(_scale));
        _childImage.set(_childImage.get_pixbuf());
    }
}

class Retinue
    :public vector<Unit>
{
    vector<UnitButton*> _unitButtonPointers; //do NOT included bonded in this
	
public:
	vector<Unit> bondedUnitVector; 
	//check player level for how many buttons are revealed -- easy to do
	UnitButton unit0;
    UnitButton unit1;
    UnitButton unit2;
    UnitButton unit3;
    UnitButton unit4;
    UnitButton unitBonded;//Bonds of Loyalty skill 
    
    Gtk::HButtonBox box;
	
    Retinue(double scale);

	void updateAll() {
        for (int i = 0; i < _unitButtonPointers.size(); ++i) {
            _unitButtonPointers[i]->update();
            if (i < size())
                _unitButtonPointers[i]->set_active(!at(i).isReady());
        }
		unitBonded.update();
		if (!bondedUnitVector.empty()) {
			unitBonded.set_active(!bondedUnitVector.front().isReady());
		}
	}
    void endOfRound() {
        for (auto i = begin(); i != end(); ++i) {
            i->ready();
			i->readyBanner();
        }
		for (auto i = bondedUnitVector.begin(); i != bondedUnitVector.end(); ++i) {
            i->ready();
			i->readyBanner();
		}
		updateAll();
    }

    void push_back(const Unit& u) {
        if (size() >= 5) 
            return;
        vector<Unit>::push_back(u);
        updateAll();
    }

	void addBondedUnit(const Unit& u) {
		if (!bondedUnitVector.empty()) {
			cout << "Bonded unit vector already full\n";
			return;
		}
		bondedUnitVector.push_back(u);
		updateAll();
	}

    void erase(size_t erasedSlot) {
        if (erasedSlot >= 5)
            return;
        vector<Unit>::erase(begin() + erasedSlot);
        updateAll();
    }

	void eraseBondedUnit(size_t erasedSlot) {
		if (erasedSlot >= bondedUnitVector.size())
            return;
        bondedUnitVector.erase(bondedUnitVector.begin() + erasedSlot);
		updateAll();
	}

};

Retinue::Retinue(double scale = 1.0)
    : unit0(this, 0, scale), unit1(this, 1, scale), unit2(this, 2, scale), unit3(this, 3, scale), unit4(this, 4, scale), unitBonded(&bondedUnitVector, 0, scale)
{
    _unitButtonPointers.push_back(&unit0);
    _unitButtonPointers.push_back(&unit1);
    _unitButtonPointers.push_back(&unit2);
    _unitButtonPointers.push_back(&unit3);
    _unitButtonPointers.push_back(&unit4);
	
    box.set_layout(Gtk::ButtonBoxStyle::BUTTONBOX_START);
	unitBonded.modifyBondedMenu();
    for (int i = 0; i < _unitButtonPointers.size(); ++i) {
		_unitButtonPointers[i]->disbandSignal.connect(sigc::mem_fun(*this, &Retinue::erase));
		box.pack_start(*_unitButtonPointers[i]);
        _unitButtonPointers[i]->set_relief(Gtk::RELIEF_NONE);
        _unitButtonPointers[i]->update();
    }
	unitBonded.disbandSignal.connect(sigc::mem_fun(*this, &Retinue::eraseBondedUnit));
	box.pack_start(unitBonded);
	unitBonded.set_relief(Gtk::RELIEF_NONE);
	unitBonded.update();
	box.set_child_secondary(unitBonded);
    box.show_all();
}


class Ruin
{
public:
    enum Reward {
        NONE,
        ARTIFACT,
        ADVANCED_ACTION,
        SPELL,
        UNIT,
        FOUR_CRYSTALS,
    };
private:
    Enemy::Type _firstEnemy;
    Enemy::Type _secondEnemy;
    Reward _firstReward;
    Reward _secondReward;
    Deed::Color _color; //for altars
    bool _facedown;

    Glib::RefPtr<Gdk::Pixbuf> _childPixbuf;
    static Glib::RefPtr<Gdk::Pixbuf> _facedownPixbuf;


public:
    Ruin(Enemy::Type enemy1 = Enemy::UNTYPED, Enemy::Type enemy2 = Enemy::UNTYPED, Reward reward1 = NONE, Reward reward2 = NONE, Deed::Color c = Deed::NO_COLOR)
        : _firstEnemy(enemy1), _secondEnemy(enemy2), _firstReward(reward1), _secondReward(reward2), _color(c), _facedown(true) {}
     

    //Ruin(Enemy::Type enemy1 = Enemy::UNTYPED, Enemy::Type enemy2 = Enemy::UNTYPED, Reward reward1 = NONE, Reward reward2 = NONE, Deed::Color c = Deed::NO_COLOR)

    //bool isBlank() const {
    //    return _firstEnemy == Enemy::UNTYPED && _secondEnemy == Enemy::UNTYPED && 
    //           _firstReward == NONE && _secondReward == NONE && _color == Deed::NO_COLOR;
    //} 
    bool isFacedown() const {return  _facedown;}
    bool isFaceup() const   {return !_facedown;}

    void flipUp() {_facedown = false;}
    
    Enemy::Type getFirstEnemy() const  {return _firstEnemy;}
    Enemy::Type getSecondEnemy() const {return _secondEnemy;}

    Reward getFirstReward() const  {return _firstReward;} 
    Reward getSecondReward() const {return _secondReward;}

    Deed::Color getAltarColor() const {return _color;}
    bool isAltar() const {return _color != Deed::NO_COLOR;}
   
    void setChildPixbuf(const Glib::RefPtr<Gdk::Pixbuf> _pixbuf) {_childPixbuf = _pixbuf;};
    Glib::RefPtr<Gdk::Pixbuf> getChildPixbuf() const {
        if (_facedown) {
            return _facedownPixbuf;
        }
        return _childPixbuf;
    }
    //Glib::RefPtr<Gdk::Pixbuf> getScaledPixbuf() {return _childPixbuf->scale_simple(getPixbuf()->get_width() * _scale, getPixbuf()->get_width() * _scale, Gdk::INTERP_HYPER);}

    //void setScale (double s) {_scale = s;}
    //double getScale() const {return _scale;}



};

Glib::RefPtr<Gdk::Pixbuf> Ruin::_facedownPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "ruin_back.png", RUIN_WIDTH, RUIN_HEIGHT, true);


class RuinDeck
    :public vector<Ruin>
{

public:
    vector<Ruin> discards;

    RuinDeck();
    
    void replenishAndShuffle() {
        this->vector<Ruin>::operator=(discards); 
        discards.clear();
        random_shuffle(begin(), end());
    }

    Ruin drawOne() {
        if (empty())
            replenishAndShuffle();
        Ruin temp(back());
        pop_back();
        return temp; 
    }
};

RuinDeck::RuinDeck() {
    //altars
    push_back(Ruin(Enemy::UNTYPED, Enemy::UNTYPED, Ruin::NONE, Ruin::NONE, Deed::BLUE)); 
    push_back(Ruin(Enemy::UNTYPED, Enemy::UNTYPED, Ruin::NONE, Ruin::NONE, Deed::RED));
    push_back(Ruin(Enemy::UNTYPED, Enemy::UNTYPED, Ruin::NONE, Ruin::NONE, Deed::GREEN));
    push_back(Ruin(Enemy::UNTYPED, Enemy::UNTYPED, Ruin::NONE, Ruin::NONE, Deed::WHITE));
    //enemies 
    push_back(Ruin(Enemy::DUNGEON, Enemy::DRACONUM,   Ruin::ARTIFACT,      Ruin::ARTIFACT,        Deed::NO_COLOR)); 
    push_back(Ruin(Enemy::ORCS,    Enemy::DRACONUM,   Ruin::ARTIFACT,      Ruin::ADVANCED_ACTION, Deed::NO_COLOR));
    push_back(Ruin(Enemy::KEEP,    Enemy::CITY,       Ruin::ARTIFACT,      Ruin::SPELL,           Deed::NO_COLOR));
    push_back(Ruin(Enemy::KEEP,    Enemy::DUNGEON,    Ruin::ARTIFACT,      Ruin::NONE,            Deed::NO_COLOR));
    push_back(Ruin(Enemy::KEEP,    Enemy::MAGE_TOWER, Ruin::UNIT,          Ruin::NONE,            Deed::NO_COLOR)); 
    push_back(Ruin(Enemy::ORCS,    Enemy::ORCS,       Ruin::ARTIFACT,      Ruin::NONE,            Deed::NO_COLOR));
    push_back(Ruin(Enemy::ORCS,    Enemy::ORCS,       Ruin::FOUR_CRYSTALS, Ruin::NONE,            Deed::NO_COLOR));
    push_back(Ruin(Enemy::DUNGEON, Enemy::MAGE_TOWER, Ruin::SPELL,         Ruin::FOUR_CRYSTALS,   Deed::NO_COLOR));

    at(0).setChildPixbuf(Gdk::Pixbuf::create_from_file(RootDirectory + "ruin_altar_blue.png" , RUIN_WIDTH, RUIN_HEIGHT, true));
    at(1).setChildPixbuf(Gdk::Pixbuf::create_from_file(RootDirectory + "ruin_altar_red.png"  , RUIN_WIDTH, RUIN_HEIGHT, true));
    at(2).setChildPixbuf(Gdk::Pixbuf::create_from_file(RootDirectory + "ruin_altar_green.png", RUIN_WIDTH, RUIN_HEIGHT, true));
    at(3).setChildPixbuf(Gdk::Pixbuf::create_from_file(RootDirectory + "ruin_altar_white.png", RUIN_WIDTH, RUIN_HEIGHT, true));
           
    at(4).setChildPixbuf(Gdk::Pixbuf::create_from_file(RootDirectory + "ruin_01.png", RUIN_WIDTH, RUIN_HEIGHT, true));
    at(5).setChildPixbuf(Gdk::Pixbuf::create_from_file(RootDirectory + "ruin_02.png", RUIN_WIDTH, RUIN_HEIGHT, true));
    at(6).setChildPixbuf(Gdk::Pixbuf::create_from_file(RootDirectory + "ruin_03.png", RUIN_WIDTH, RUIN_HEIGHT, true));
    at(7).setChildPixbuf(Gdk::Pixbuf::create_from_file(RootDirectory + "ruin_04.png", RUIN_WIDTH, RUIN_HEIGHT, true));
    at(8).setChildPixbuf(Gdk::Pixbuf::create_from_file(RootDirectory + "ruin_05.png", RUIN_WIDTH, RUIN_HEIGHT, true));
    at(9).setChildPixbuf(Gdk::Pixbuf::create_from_file(RootDirectory + "ruin_06.png", RUIN_WIDTH, RUIN_HEIGHT, true));
    at(10).setChildPixbuf(Gdk::Pixbuf::create_from_file(RootDirectory + "ruin_07.png", RUIN_WIDTH, RUIN_HEIGHT, true));
    at(11).setChildPixbuf(Gdk::Pixbuf::create_from_file(RootDirectory + "ruin_08.png", RUIN_WIDTH, RUIN_HEIGHT, true));

    random_shuffle(begin(), end());

   
};

class Space
{
public:
    enum Feature {
        NO_FEATURE,
        CITY_BLUE,
        CITY_RED,
        CITY_GREEN,
        CITY_WHITE,
        KEEP,
        MAGE_TOWER,
        ANCIENT_RUINS,
        DUNGEON,
        TOMB,
        SPAWNING_GROUNDS,
        MONSTER_DEN,
        MONASTERY,
        VILLAGE,
        DRACONUM,
        ORCS,
        CRYSTAL_MINES_BLUE,
        CRYSTAL_MINES_RED,
        CRYSTAL_MINES_GREEN,
        CRYSTAL_MINES_WHITE,
        MAGICAL_GLADE
    };
    
private:
    Terrain _terrain;
    Feature _feature;

    bool _playerHere; //may not need, can just have displayer render him last

	unsigned int _shields;

    vector<Enemy> _enemies;
    list<Ruin> _ruin;

public:
    friend class Board;
    friend class Game;

    Space()
        : _terrain(UNEXPLORED), _feature(NO_FEATURE), _playerHere(false),  _shields(0) {}
    Space(Terrain t, Feature f = NO_FEATURE)
        : _terrain(t), _feature(f), _playerHere(false), _shields(0) {}

    void addEnemy(const Enemy& e) {_enemies.push_back(e);}
    void addEnemyPile(const vector<Enemy>& e) {_enemies.insert(_enemies.end(), e.begin(), e.end());}
    void killEnemy(size_t i) {
        if (i >= _enemies.size())
            return;
        _enemies.erase(_enemies.begin() + i);
    }
    void clearEnemies() {_enemies.clear();}

    bool hasFaceDownEnemies() const {
        for (vector<Enemy>::const_iterator i = _enemies.begin(); i != _enemies.end(); ++i) {
            if (i->isFacedown())
                return true;
        }
        return false;
    }

    void revealAllEnemies() {
        for (vector<Enemy>::iterator i = _enemies.begin(); i != _enemies.end(); ++i) {
            i->flipUp();
        }
    }

    bool isConquered() const {
		if (_terrain == CITY) {
			return _enemies.empty();
		}
		return _shields > 0;
	} 

	bool isKeepAndIsConquered() const {
		return (_feature == KEEP && isConquered());			
	}

	bool isCityAndIsConquered() const {
		return (_terrain == CITY && isConquered());			
	} 

    unsigned int getShields() const   {return _shields;}
    void setShields(unsigned int n) {_shields = n;}

    void burn() {
        _feature = NO_FEATURE;
        ++_shields;
	}

    Terrain getTerrain() const {return _terrain;}
    string getTerrainString() const;
    Feature getFeature() const {return _feature; }
    string getFeatureString() const;

    const vector<Enemy>& getEnemies() const {return _enemies;}
    void setEnemies (const vector<Enemy>& newVector) {_enemies = newVector;}

    void addRuin(const Ruin& r) {
        if (!_ruin.empty()) {
            cout << "Space already has ruin.\n";
            return;
        }
        _ruin.push_back(r);
    }
    void removeRuin() {_ruin.clear();}
    bool hasRuin() {return !_ruin.empty();}
    void flipRuinUp() {
        if (!_ruin.empty())
            _ruin.front().flipUp();
    }
    const Ruin& getRuin() const {
        if (!_ruin.empty()) 
            return _ruin.front();
        return Ruin();
    }
    bool hasFaceupRuin() const {
        if (_ruin.empty() || _ruin.front().isFacedown())
            return false;
        return true;
    }
    bool hasFacedownRuin() const {
        if (_ruin.empty() || _ruin.front().isFaceup())
            return false;
        return true;
    }

    bool hasRampaging() const {
        return (_feature == ORCS || _feature == DRACONUM) && _enemies.size() > 0;
    }

    bool canBeMovedThrough() const {
        if (_terrain == LAKE || _terrain == MOUNTAIN) //UNEXPLORED?
            return false;
        return true;
    }

    bool canBeConquered() const {
		if (isConquered())
			return false;
		if (_terrain == CITY)
            return true;
        switch (_feature) {
        case ANCIENT_RUINS:
        case DUNGEON:
        case KEEP:
        case MAGE_TOWER:
        case MONSTER_DEN:
		case MONASTERY:
        case SPAWNING_GROUNDS:
        case TOMB:
            return true;
        }
        return false;
    }

    bool isFortified() const { //unused as of now
		return !isConquered() && (_terrain == CITY || _feature == MAGE_TOWER || _feature == KEEP);
    }

    bool isAdventureSite() const {
        return (_feature == MONSTER_DEN || _feature == SPAWNING_GROUNDS || _feature == DUNGEON || _feature == TOMB || _feature == ANCIENT_RUINS);
    }

    bool isCrystalMine() const {
        return (_feature == CRYSTAL_MINES_BLUE || _feature == CRYSTAL_MINES_RED || _feature == CRYSTAL_MINES_GREEN|| _feature == CRYSTAL_MINES_WHITE);
    }

    //new feature: burned monastery?
    string onMouseOver() const {
        ostringstream oss;
        oss <<  getTerrainString() << (canBeMovedThrough() ? " (Move X)" : " (Impassable)");
		if (_feature != NO_FEATURE) {
            oss << "\n" << getFeatureString();
			if (isConquered())
                oss << " (conquered)";
        }
        else if (isConquered()) //and has no feature
            oss << "\nMonastery has been burned";
        if (_shields)
            oss << "\n" << _shields << " shield" << (_shields == 1 ? "" : "s");
        return oss.str();
    }


};
string Space::getTerrainString() const{
    switch(_terrain) {
    case UNEXPLORED:  return "Unexplored";
    case LAKE:        return "Lake";
    case MOUNTAIN:    return "Mountain";
    case PLAINS:      return "Plains";
    case HILLS:       return "Hills";
    case FOREST:      return "Forest";
    case WASTELAND:   return "Wasteland";
    case DESERT:      return "Desert";
    case SWAMP:       return "Swamp";
    case CITY:        return "City";
    default:          return "";
    }
}
string Space::getFeatureString() const {
    switch (_feature) {
     case CITY_BLUE:           return "Blue city";
     case CITY_RED:            return "Red city";
     case CITY_GREEN:          return "Green city";
     case CITY_WHITE:          return "White city";
     case KEEP:                return "Keep";
     case MAGE_TOWER:          return "Mage tower";
     case ANCIENT_RUINS:       return "Ancient ruins";
     case DUNGEON:             return "Dungeon";
     case TOMB:                return "Tomb";
     case SPAWNING_GROUNDS:    return "Spawning grounds";
     case MONSTER_DEN:         return "Monster den";
     case MONASTERY:           return "Monastery";
     case VILLAGE:             return "Village";
     case DRACONUM:            return "Draconum";
     case ORCS:                return "Orcs";
     case CRYSTAL_MINES_BLUE:  return "Blue crystal mines";
     case CRYSTAL_MINES_RED:   return "Red crystal mines";
     case CRYSTAL_MINES_GREEN: return "Green crystal mines";
     case CRYSTAL_MINES_WHITE: return "White crystal mines";
     case MAGICAL_GLADE:       return "Magical glade";
     default:                  return "";
    }
}

/*
MONASTERY:
If you win the combat, you gain an Artifact at the end of your turn
(see later), and the monastery is burned to ashes. Mark the space
with one of your Shield tokens. From now on, it has no function, as
if it is an empty space. (You do not remove the Advanced Action
added to Unit offer by this monastery, as it still can be learned
elsewhere, but you do not add a new Advanced Action for this
monastery at the start of the next Round.)

If you fail, nothing happens, and only the position of your
Reputation token (and perhaps few Wounds in your hand)
reminds you of what you attempted to do.
*/

/*

class Site


class City (etc)
    :public Site 


*/


class InfoWindow
    :public Gtk::Window
{
    Gtk::Notebook _notebook;
    Gtk::Button _okButton;
    Gtk::VBox _vbox;
    Gtk::HButtonBox _hbuttonbox;
public:
    InfoWindow()
        :_okButton("OK")
    {
        set_title("Site guide");
        set_icon(Gdk::Pixbuf::create_from_file(RootDirectory + "ruin_back.png"));
        set_keep_above(true);
        set_resizable(false);
        add(_vbox);
        _vbox.set_spacing(50);
        _vbox.set_homogeneous(false);
        _vbox.pack_start(_notebook, Gtk::PackOptions::PACK_SHRINK, 0);
        _hbuttonbox.pack_start(_okButton, Gtk::PACK_SHRINK, 0);
        _vbox.pack_start(_hbuttonbox, Gtk::PackOptions::PACK_EXPAND_PADDING, 0);
        
        _notebook.set_tab_pos(Gtk::POS_BOTTOM);
        _notebook.set_scrollable();

        size_t numberOfPages = Space::MAGICAL_GLADE + 1;

        for (int i = 0; i < numberOfPages; ++i) {
            Gtk::Image* image_ptr = Gtk::manage(new Gtk::Image);
            Gtk::Label* label_ptr = Gtk::manage(new Gtk::Label);
            label_ptr->set_markup("<span font_family = \"Gazette\" size=\"10240\">" + getFeatureString(i) + "</span>");
            _notebook.append_page(*image_ptr, *label_ptr);
        } 
      
        Gtk::Label* label_ptr = dynamic_cast<Gtk::Label*>( _notebook.get_tab_label(*_notebook.get_nth_page(0)));
        label_ptr->set_markup("<span font_family = \"Gazette\" size=\"10240\"> City </span>");
        dynamic_cast<Gtk::Image*>(_notebook.get_nth_page(0))->                         set(RootDirectory + "description_city.jpg");
        dynamic_cast<Gtk::Image*>(_notebook.get_nth_page(Space::ANCIENT_RUINS))->      set(RootDirectory + "description_ancient_ruins.jpg");
        dynamic_cast<Gtk::Image*>(_notebook.get_nth_page(Space::MAGICAL_GLADE))->      set(RootDirectory + "description_crystal_mines_magical_glade.jpg");
        dynamic_cast<Gtk::Image*>(_notebook.get_nth_page(Space::CRYSTAL_MINES_BLUE))-> set(RootDirectory + "description_crystal_mines_magical_glade.jpg");
        dynamic_cast<Gtk::Image*>(_notebook.get_nth_page(Space::CRYSTAL_MINES_RED))->  set(RootDirectory + "description_crystal_mines_magical_glade.jpg");
        dynamic_cast<Gtk::Image*>(_notebook.get_nth_page(Space::CRYSTAL_MINES_GREEN))->set(RootDirectory + "description_crystal_mines_magical_glade.jpg");
        dynamic_cast<Gtk::Image*>(_notebook.get_nth_page(Space::CRYSTAL_MINES_WHITE))->set(RootDirectory + "description_crystal_mines_magical_glade.jpg");
        dynamic_cast<Gtk::Image*>(_notebook.get_nth_page(Space::DRACONUM))->           set(RootDirectory + "description_draconum.jpg");
        dynamic_cast<Gtk::Image*>(_notebook.get_nth_page(Space::DUNGEON))->            set(RootDirectory + "description_dungeon.jpg");
        dynamic_cast<Gtk::Image*>(_notebook.get_nth_page(Space::KEEP))->               set(RootDirectory + "description_keep.jpg");
        dynamic_cast<Gtk::Image*>(_notebook.get_nth_page(Space::MAGE_TOWER))->         set(RootDirectory + "description_mage_tower.jpg");
        dynamic_cast<Gtk::Image*>(_notebook.get_nth_page(Space::ORCS))->               set(RootDirectory + "description_marauding_orcs.jpg");
        dynamic_cast<Gtk::Image*>(_notebook.get_nth_page(Space::SPAWNING_GROUNDS))->   set(RootDirectory + "description_spawning_grounds.jpg");
        dynamic_cast<Gtk::Image*>(_notebook.get_nth_page(Space::TOMB))->               set(RootDirectory + "description_tomb.jpg");
        dynamic_cast<Gtk::Image*>(_notebook.get_nth_page(Space::MONASTERY))->          set(RootDirectory + "description_monastery.jpg");
        dynamic_cast<Gtk::Image*>(_notebook.get_nth_page(Space::MONSTER_DEN))->        set(RootDirectory + "description_monster_den.jpg");
        dynamic_cast<Gtk::Image*>(_notebook.get_nth_page(Space::VILLAGE))->            set(RootDirectory + "description_village.jpg");
        dynamic_cast<Gtk::Image*>(_notebook.get_nth_page(Space::CITY_BLUE ))->         set(RootDirectory + "city_card_blue1.jpg");
        dynamic_cast<Gtk::Image*>(_notebook.get_nth_page(Space::CITY_RED  ))->         set(RootDirectory + "city_card_red1.jpg");
        dynamic_cast<Gtk::Image*>(_notebook.get_nth_page(Space::CITY_GREEN))->         set(RootDirectory + "city_card_green1.jpg");
        dynamic_cast<Gtk::Image*>(_notebook.get_nth_page(Space::CITY_WHITE))->         set(RootDirectory + "city_card_white1.jpg");
        _notebook.remove_page(Space::CRYSTAL_MINES_BLUE);
        _notebook.remove_page(Space::CRYSTAL_MINES_BLUE);
        _notebook.remove_page(Space::CRYSTAL_MINES_BLUE);

        _okButton.signal_clicked().connect(sigc::mem_fun(*this, &InfoWindow::hide));
    }

    string getFeatureString(size_t feature) const {
        switch (feature) {
        case Space::CITY_BLUE:           return "Blue city";
        case Space::CITY_RED:            return "Red city";
        case Space::CITY_GREEN:          return "Green city";
        case Space::CITY_WHITE:          return "White city";
        case Space::KEEP:                return "Keep";
        case Space::MAGE_TOWER:          return "Mage tower";
        case Space::ANCIENT_RUINS:       return "Ancient ruins";
        case Space::DUNGEON:             return "Dungeon";
        case Space::TOMB:                return "Tomb";
        case Space::SPAWNING_GROUNDS:    return "Spawning grounds";
        case Space::MONSTER_DEN:         return "Monster den";
        case Space::MONASTERY:           return "Monastery";
        case Space::VILLAGE:             return "Village";
        case Space::DRACONUM:            return "Draconum";
        case Space::ORCS:                return "Orcs";
        case Space::CRYSTAL_MINES_BLUE:  //duplicates deleted
        case Space::CRYSTAL_MINES_RED:   
        case Space::CRYSTAL_MINES_GREEN: 
        case Space::CRYSTAL_MINES_WHITE: return "Crystal mines";
        case Space::MAGICAL_GLADE:       return "Magical glade";
        default:                         return "City";
        }
    }

    void openToPage (Space::Feature feature = Space::NO_FEATURE) {
        show_all();
        if (int(feature) > 16) {
            _notebook.set_current_page(Space::CRYSTAL_MINES_BLUE);
        }
        else {
            _notebook.set_current_page(feature);
        }
    }
};


class Tile
{ 
    Space _northwest;
    Space _northeast;
    Space _west;
    Space _center;
    Space _east;
    Space _southwest;
    Space _southeast;

    Glib::RefPtr<Gdk::Pixbuf> _childPixbuf;
    //vector<Space*> _spacePointers;
public:

    friend class Board;
    friend class Game;
    Tile() {}
    Tile(Space nw, Space ne, Space w, Space c, Space e, Space sw, Space se)
        :_northwest(nw),
         _northeast(ne),
         _west (w),
         _center (c),
         _east (e),
         _southwest (sw),
         _southeast (se)
         //   , _childPixbuf(Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, 1, 1)) //first default constuctor that wouldn't crash program
    { 
        //_spacePointers.resize(7);
        //_spacePointers[0] = &_center;
        //_spacePointers[NORTHWEST] = &_northwest;
        //_spacePointers[NORTHEAST] = &_northeast;
        //_spacePointers[EAST] =      &_east;
        //_spacePointers[SOUTHEAST] = &_southwest;
        //_spacePointers[SOUTHWEST] = &_southwest;
        //_spacePointers[WEST] =      &_west;


    }
    /*
    Tile(const Tile& another) 
        :_northwest   (another._northwest),
         _northeast   (another._northeast),
         _west        (another._west     ),
         _center      (another._center   ),
         _east        (another._east     ),
         _southwest   (another._southwest),
         _southeast   (another._southeast),
         _childPixbuf(another._childPixbuf->copy())
    { 
        _spacePointers.resize(7);
        _spacePointers[0] = &_center;
        _spacePointers[NORTHWEST] = &_northwest;
        _spacePointers[NORTHEAST] = &_northeast;
        _spacePointers[EAST] =      &_east;
        _spacePointers[SOUTHEAST] = &_southwest;
        _spacePointers[SOUTHWEST] = &_southwest;
        _spacePointers[WEST] =      &_west;
    }

    Tile& operator=(const Tile& rhs) 
    {
        _northwest   = rhs._northwest;
        _northeast   = rhs._northeast;
        _west        = rhs._west     ;
        _center      = rhs._center   ;
        _east        = rhs._east     ;
        _southwest   = rhs._southwest;
        _southeast   = rhs._southeast;
        _childPixbuf = rhs._childPixbuf->copy();

        _spacePointers.resize(7);
        _spacePointers[0] = &_center;
        _spacePointers[NORTHWEST] = &_northwest;
        _spacePointers[NORTHEAST] = &_northeast;
        _spacePointers[EAST] =      &_east;
        _spacePointers[SOUTHEAST] = &_southwest;
        _spacePointers[SOUTHWEST] = &_southwest;
        _spacePointers[WEST] =      &_west;

        return *this;
    }
    */
    void setChildPixbuf(const string& s) {
        _childPixbuf = Gdk::Pixbuf::create_from_file(s, FULLSIZE_TILE_WIDTH, FULLSIZE_TILE_HEIGHT, false);
    }

    Glib::RefPtr<Gdk::Pixbuf> getChildPixbuf() const {
        return _childPixbuf;
    }
    Glib::RefPtr<Gdk::Pixbuf> getScaledChildPixbuf() const {
        return _childPixbuf->scale_simple(_childPixbuf->get_width() * TILE_SCALING_FACTOR, _childPixbuf->get_height() * TILE_SCALING_FACTOR, Gdk::INTERP_HYPER);
    }

    bool hasCity() const {
        if (_northwest.getTerrain() == CITY) return true;
        if (_northeast.getTerrain() == CITY) return true;
        if (_west     .getTerrain() == CITY) return true;
        if (_center   .getTerrain() == CITY) return true;
        if (_east     .getTerrain() == CITY) return true;
        if (_southwest.getTerrain() == CITY) return true;
        if (_southeast.getTerrain() == CITY) return true;
    }
};

class Board
    :public std::map<Coordinate, Space>
{

    const Coordinate INVALID_COORDINATE;
    mapGenerator _generator;
    // base: <Location, Center Tile Location>
    // members: multimap reverse of above (unmade as of yet)
    //          <Location, center (x, y)> 
    std::map<Coordinate, Coordinate> mapOfTileCenterCoordinates;
public:
    Board(int originX, int originY);
    
    Coordinate northWestOf(const Coordinate& _coordinate) const;
    Coordinate northEastOf(const Coordinate& _coordinate) const;
    Coordinate southWestOf(const Coordinate& _coordinate) const;
    Coordinate southEastOf(const Coordinate& _coordinate) const;
    Coordinate eastOf(const Coordinate& _coordinate)      const;
    Coordinate westOf(const Coordinate& _coordinate)      const;
      
    
    /* takes coordinates of any hex and returns the coordinates of its tile's center hex */
    Coordinate centerHexforAnyHex(const Coordinate& _coordinate) const {
        return mapOfTileCenterCoordinates.at(_coordinate); 
    }

    /* takes coordinates of any hex and returns the coordinates of its tile's anchor point  */
    Coordinate tileAnchorforAnyHex(const Coordinate& _coordinate) const {
        return mapOfTileCenterCoordinates.at(_coordinate).tileAnchor(); 
    }

    /* takes the coordinates of any tile's anchor point and returns the coordinates of the corresponding center hex*/
    Coordinate centerHexOfTileAnchor(const Coordinate& _tileAnchor) const {

        Coordinate result = _tileAnchor;
        result.x += int((SCALED_TILE_WIDTH)  / 2);
        result.y += int((SCALED_TILE_HEIGHT) / 2);
        
        for (Board::const_iterator i = begin(); i != end(); ++i) {
            if (result - Coordinate(i->first) < 10) {
                return i->first;
            }
        }
        cout << "Invalid coordinate given for centerHexOfTileAnchor()\n";
        exit(EXIT_FAILURE);
    }

    bool isValidDirection(const Coordinate& _coordinate, const Direction& d) const {
        switch (d) {
        case NORTHWEST:  return (northWestOf(_coordinate) != INVALID_COORDINATE);
        case NORTHEAST:  return (northEastOf(_coordinate) != INVALID_COORDINATE);
        case EAST     :  return (eastOf(_coordinate)      != INVALID_COORDINATE);
        case SOUTHEAST:  return (southEastOf(_coordinate) != INVALID_COORDINATE);
        case SOUTHWEST:  return (southWestOf(_coordinate) != INVALID_COORDINATE);
        case WEST     :  return (westOf(_coordinate)      != INVALID_COORDINATE);
        }
        return false;
    }

    bool spaceIsUnexplored(const Coordinate& _coordinate) const {
        if (find(_coordinate) == end())
            return false;
        if (find(_coordinate)->second.getTerrain() == UNEXPLORED)
            return true;
        return false;
    }

    bool playerIsNextTo(const Coordinate& _coordinate) const {
        if (isValidDirection(_coordinate, NORTHWEST) && find(northWestOf(_coordinate))->second._playerHere) { return true; }
        if (isValidDirection(_coordinate, NORTHEAST) && find(northEastOf(_coordinate))->second._playerHere) { return true; }                                                                                                      
        if (isValidDirection(_coordinate, EAST)      && find(eastOf     (_coordinate))->second._playerHere) { return true; }                                                                                                     
        if (isValidDirection(_coordinate, SOUTHEAST) && find(southEastOf(_coordinate))->second._playerHere) { return true; }                                                                                                     
        if (isValidDirection(_coordinate, SOUTHWEST) && find(southWestOf(_coordinate))->second._playerHere) { return true; }                                                                                                     
        if (isValidDirection(_coordinate, WEST)      && find(westOf     (_coordinate))->second._playerHere) { return true; }
        return false;
    }

    void movePlayerTo(const Coordinate& destination) {
        if (find(destination) == end()) {
            cout << "Invalid destination for movePlayerTo()  " << destination << endl;
            return;
        }
        if (playerAt() == destination)
            return;
        Coordinate source = playerAt();
        (*this)[source].     _playerHere = false;
        (*this)[destination]._playerHere = true;

    }

    Coordinate playerAt() const {
        for (Board::const_iterator i = begin(); i != end(); ++i) {
            if (i->second._playerHere)
                return i->first;
        }
        cout << "Player not found.\n";
        exit(EXIT_FAILURE);
    }

    void playerEntersPortalAt(const Coordinate& _origin) {
        find(_origin)->second._playerHere = true;
    }

    bool areBothAdjacent(const Coordinate& one, const Coordinate& two) const; 

    bool provokesEnemy(const Coordinate& source, const Coordinate& destination) const;

    vector<Direction> listOfRampaging(const Coordinate& source, const Coordinate& destination) const;

	size_t getNumberOfNeighboringTiles(const Coordinate& source) const {
		if (source == INVALID_COORDINATE)
			return 0;
		size_t result = 0;
		Coordinate center = centerHexforAnyHex(source);
		if (northWestOf(northWestOf(center)) != INVALID_COORDINATE && at(northWestOf(northWestOf(center))).getTerrain() != UNEXPLORED) { ++result; }
		if (northEastOf(northEastOf(center)) != INVALID_COORDINATE && at(northEastOf(northEastOf(center))).getTerrain() != UNEXPLORED) { ++result; }
		if (eastOf(eastOf(center))           != INVALID_COORDINATE && at(eastOf(eastOf(center))).getTerrain()           != UNEXPLORED) { ++result; }
		if (southEastOf(southEastOf(center)) != INVALID_COORDINATE && at(southEastOf(southEastOf(center))).getTerrain() != UNEXPLORED) { ++result; }
		if (southWestOf(southWestOf(center)) != INVALID_COORDINATE && at(southWestOf(southWestOf(center))).getTerrain() != UNEXPLORED) { ++result; }
		if (westOf(westOf(center))           != INVALID_COORDINATE && at(westOf(westOf(center))).getTerrain()           != UNEXPLORED) { ++result; }
		return result;
	}

	bool isValidForCountrysidePlacement(const Coordinate& source) const {
		if (getNumberOfNeighboringTiles(source) >= 2) 
			return true;
		
		Coordinate center = centerHexforAnyHex(source);
		if (northWestOf(northWestOf(center)) != INVALID_COORDINATE && at(northWestOf(northWestOf(center))).getTerrain() != UNEXPLORED && getNumberOfNeighboringTiles(northWestOf(northWestOf(center))) >=2) return true;
		if (northEastOf(northEastOf(center)) != INVALID_COORDINATE && at(northEastOf(northEastOf(center))).getTerrain() != UNEXPLORED && getNumberOfNeighboringTiles(northEastOf(northEastOf(center))) >=2) return true;
		if (eastOf(eastOf(center))           != INVALID_COORDINATE && at(eastOf(eastOf(center))).getTerrain()           != UNEXPLORED && getNumberOfNeighboringTiles(eastOf(eastOf(center)))           >=2) return true;
		if (southEastOf(southEastOf(center)) != INVALID_COORDINATE && at(southEastOf(southEastOf(center))).getTerrain() != UNEXPLORED && getNumberOfNeighboringTiles(southEastOf(southEastOf(center))) >=2) return true;
		if (southWestOf(southWestOf(center)) != INVALID_COORDINATE && at(southWestOf(southWestOf(center))).getTerrain() != UNEXPLORED && getNumberOfNeighboringTiles(southWestOf(southWestOf(center))) >=2) return true;
		if (westOf(westOf(center))           != INVALID_COORDINATE && at(westOf(westOf(center))).getTerrain()           != UNEXPLORED && getNumberOfNeighboringTiles(westOf(westOf(center)))           >=2) return true;
		return false;
	}

};

Board::Board(int originX = 0, int originY = 0)
    :INVALID_COORDINATE(originX - 1, originY - 1)
{
    _generator.setCenterPoints(originX, originY);
   
    //(*this)[_generator.centerPoints.second] = Space(); 
    for (mapGenerator::iterator i = _generator.begin(); i != _generator.end(); ++i) { 
        (*this)[_generator.centerPoints[i->first]] = Space(); 
        mapOfTileCenterCoordinates[_generator.centerPoints[i->first]] = _generator.centerPoints[i->second];
    }
    
}

Coordinate Board::northWestOf(const Coordinate& _coordinate) const  {
	if (_coordinate == INVALID_COORDINATE)
		return INVALID_COORDINATE;
	int X = _coordinate.first  - 0.5 * HEX_WIDTH;                  
	int Y = _coordinate.second - 1.5 * HEX_SIDE_LENGTH;                   
	Coordinate target(X, Y);

	for (Board::const_iterator i = begin(); i != end(); ++i) {
		if (Coordinate(target) - Coordinate(i->first) < 10) {
			return i->first;
		}
	}
	return INVALID_COORDINATE;
}
Coordinate Board::northEastOf(const Coordinate& _coordinate) const   {
	if (_coordinate == INVALID_COORDINATE)
		return INVALID_COORDINATE;
	int X = _coordinate.first  + 0.5 * HEX_WIDTH;                  
	int Y = _coordinate.second - 1.5 * HEX_SIDE_LENGTH;                   
	Coordinate target(X, Y);

	for (Board::const_iterator i = begin(); i != end(); ++i) {
		if (Coordinate(target) - Coordinate(i->first) < 10) {
			return i->first;
		}
	}
	return INVALID_COORDINATE;
}
Coordinate Board::southWestOf(const Coordinate& _coordinate) const   {
	if (_coordinate == INVALID_COORDINATE)
		return INVALID_COORDINATE;
	int X = _coordinate.first  - 0.5 * HEX_WIDTH;                  
	int Y = _coordinate.second + 1.5 * HEX_SIDE_LENGTH;                   
	Coordinate target(X, Y);

	for (Board::const_iterator i = begin(); i != end(); ++i) {
		if (Coordinate(target) - Coordinate(i->first) < 10) {
			return i->first;
		}
	}
	return INVALID_COORDINATE;
}
Coordinate Board::southEastOf(const Coordinate& _coordinate) const   {
	if (_coordinate == INVALID_COORDINATE)
		return INVALID_COORDINATE;
	int X = _coordinate.first  + 0.5 * HEX_WIDTH;                  
	int Y = _coordinate.second + 1.5 * HEX_SIDE_LENGTH;                   
	Coordinate target(X, Y);

	for (Board::const_iterator i = begin(); i != end(); ++i) {
		if (Coordinate(target) - Coordinate(i->first) < 10) {
			return i->first;
		}
	}
	return INVALID_COORDINATE;
}
Coordinate Board::eastOf(const Coordinate& _coordinate)      const   {
	if (_coordinate == INVALID_COORDINATE)
		return INVALID_COORDINATE;
	int X = _coordinate.first + HEX_WIDTH;                  
	int Y = _coordinate.second;                   
	Coordinate target(X, Y);

	for (Board::const_iterator i = begin(); i != end(); ++i) {
		if (Coordinate(target) - Coordinate(i->first) < 10) {
			return i->first;
		}
	}
	return INVALID_COORDINATE;
}
Coordinate Board::westOf(const Coordinate& _coordinate)      const   {
	if (_coordinate == INVALID_COORDINATE)
		return INVALID_COORDINATE;
	int X = _coordinate.first - HEX_WIDTH;                  
	int Y = _coordinate.second;                   
	Coordinate target(X, Y);

	for (Board::const_iterator i = begin(); i != end(); ++i) {
		if (Coordinate(target) - Coordinate(i->first) < 10) {
			return i->first;
		}
	}
	return INVALID_COORDINATE;
}

bool Board::areBothAdjacent(const Coordinate& one, const Coordinate& two) const  {
    if (isValidDirection(one, NORTHWEST) && northWestOf(one) == two) { return true; }
    if (isValidDirection(one, NORTHEAST) && northEastOf(one) == two) { return true; }                                                                                                      
    if (isValidDirection(one, EAST)      && eastOf     (one) == two) { return true; }                                                                                                     
    if (isValidDirection(one, SOUTHEAST) && southEastOf(one) == two) { return true; }                                                                                                     
    if (isValidDirection(one, SOUTHWEST) && southWestOf(one) == two) { return true; }                                                                                                     
    if (isValidDirection(one, WEST)      && westOf     (one) == two) { return true; }
    return false;
}

bool Board::provokesEnemy(const Coordinate& source, const Coordinate& destination) const  {
    vector<Coordinate> adj;
    if (isValidDirection(source, NORTHWEST)) adj.push_back(northWestOf(source));
    if (isValidDirection(source, NORTHEAST)) adj.push_back(northEastOf(source));
    if (isValidDirection(source, SOUTHWEST)) adj.push_back(southWestOf(source));
    if (isValidDirection(source, SOUTHEAST)) adj.push_back(southEastOf(source));
    if (isValidDirection(source, EAST))      adj.push_back(eastOf     (source));
    if (isValidDirection(source, WEST))      adj.push_back(westOf     (source));
    for (auto i = adj.begin(); i != adj.end(); ++i) {
        if (at(*i).hasRampaging() && areBothAdjacent(*i, destination))
            return true;
    }
    return false;
}

vector<Direction> Board::listOfRampaging(const Coordinate& source, const Coordinate& destination) const {
    vector<Direction> result;
    if (isValidDirection(source, NORTHWEST) && at(northWestOf(source)).hasRampaging() && areBothAdjacent(northWestOf(source), destination))
        result.push_back(NORTHWEST);    
    if (isValidDirection(source, NORTHEAST) && at(northEastOf(source)).hasRampaging() && areBothAdjacent(northEastOf(source), destination))
        result.push_back(NORTHEAST);    
    if (isValidDirection(source, SOUTHWEST) && at(southWestOf(source)).hasRampaging() && areBothAdjacent(southWestOf(source), destination))
        result.push_back(SOUTHWEST);    
    if (isValidDirection(source, SOUTHEAST) && at(southEastOf(source)).hasRampaging() && areBothAdjacent(southEastOf(source), destination))
        result.push_back(SOUTHEAST);    
    if (isValidDirection(source, EAST     ) && at(eastOf     (source)).hasRampaging() && areBothAdjacent(eastOf     (source), destination))
        result.push_back(EAST     );    
    if (isValidDirection(source, WEST     ) && at(westOf     (source)).hasRampaging() && areBothAdjacent(westOf     (source), destination))
        result.push_back(WEST     );
    return result;
}

class Player
{
public:

private:
    vector<unsigned int> _manaTokens;
    vector<unsigned int> _crystals;

    unsigned int _movePoints;
    unsigned int _influencePoints;
    unsigned int _blockPoints;
    unsigned int _attackPoints;
    unsigned int _healPoints; //no sideways cards 

    unsigned int _fame;
    int _reputation; //can be negative
    //If your token is on the X [-6] space of the Reputation track, you cannot interact at all!
    //this may have to be a vector, since values are different than distance from 0
    //rep is iterator to spot in vector
    unsigned int _armor;
    unsigned int _handsize;


    bitset<TotalSkillsInGame> ownedSkills;
    bitset<TotalSkillsInGame> availableSkills; //1 == available, 0 == used
    vector<unsigned int> skillTokenPile;

    Location _hexLocation;

    Glib::RefPtr<Gdk::Pixbuf> _childPixbuf;
public:
    Player() {
        _manaTokens.resize(BLACK + 1);
        _crystals.resize(crystal::WHITE + 1);
        _hexLocation = Location(0, 0);
    }

    unsigned int getLevel(); 
    
    unsigned int getNumberOfSkills() const {return ownedSkills.count();}

    void startTurn();

    unsigned int getManaTokens(ManaColor c) const {return _manaTokens[c];}
    void gainManaTokens(ManaColor c, unsigned int n) { _manaTokens[c] += n; }
    void spendManaTokens(ManaColor c, unsigned int n) { 
        if (n > _manaTokens[c])
        {
            _manaTokens[c] = 0;
            return;
        }
        _manaTokens[c] -= n;
        
    };

    unsigned int getCrystals(crystal::CrystalColor c) const {return _crystals[c];}
    void gainCrystals(crystal::CrystalColor c, unsigned int n) {
        while (n) {
            if (_crystals[c] < 3) {
                ++_crystals[c];
            }
            else {
                ++_manaTokens[c];
            }
            --n;
        }
    };
    
    void spendCrystals(crystal::CrystalColor c, unsigned int n) {
        while (_crystals[c] > 0 && n > 0) {
            --_crystals[c];
            ++_manaTokens[c];
            --n;
        }
    };

    void setChildPixbuf(const string& s) {
        _childPixbuf = Gdk::Pixbuf::create_from_file(s, FULLSIZE_PLAYER_TOKEN_DIAMETER, FULLSIZE_PLAYER_TOKEN_DIAMETER);
    }

    Glib::RefPtr<Gdk::Pixbuf> getChildPixbuf() const { return _childPixbuf;}
    Glib::RefPtr<Gdk::Pixbuf> getScaledChildPixbuf() const { 
        return _childPixbuf->scale_simple(HEX_SIDE_LENGTH, HEX_SIDE_LENGTH * 1.25, Gdk::INTERP_HYPER);
    }

    void setLocation(const Location& loc) {_hexLocation = loc;}
    Location getLocation() const {return _hexLocation;}

    void move____West ()  {--_hexLocation.first;                       }
    void moveNorthWest()  {--_hexLocation.first; ++_hexLocation.second;}
    void moveNorthEast()  {                      ++_hexLocation.second;}
    void move____East()   {++_hexLocation.first;                       }
    void moveSouthEast()  {++_hexLocation.first; --_hexLocation.second;}
    void moveSouthWest()  {                      --_hexLocation.second;}

    void print() const  {
        cout << "Player function:\n";
        for (size_t i = 0; i < _manaTokens.size(); ++i) {
            cout << "[" << i << "] " << _manaTokens[i] << endl;
        }
        cout << "----\n";
        for (size_t i = 0; i < _crystals.size(); ++i) {
            cout << "[" << i << "] " << _crystals[i] << endl;
        }
    };
};
void Player::startTurn()  {
    _movePoints = 0; 
    _influencePoints = 0; 
    _blockPoints = 0; 
    _attackPoints = 0; 
    _healPoints = 0; 
    
    _manaTokens.assign(BLACK + 1, 0);
};
unsigned int Player::getLevel()
{
    if (_fame > 98) return 10;
    if (_fame > 79) return 9;
    if (_fame > 52) return 8;
    if (_fame > 47) return 7;
    if (_fame > 34) return 6;
    if (_fame > 23) return 5;
    if (_fame > 14) return 4;
    if (_fame > 7)  return 3;
    if (_fame > 2)  return 2;
    return 1;
}

/*
class CardHolder
    :public Gtk::EventBox
{

    bool _toggled;
    vector<Deed>* ptr;
    size_t vectorSlot;
    Gtk::Image _childImage;
    Glib::RefPtr<Gdk::Pixbuf> _background;
public:

    CardHolder();

    bool toggle(GdkEventButton *e); 
    void update();

    void setVectorInfo(vector<Deed>* vd, size_t s) {
        ptr = vd;
        vectorSlot = s;
    }
};

CardHolder::CardHolder()
    : _toggled(false), vectorSlot(0)
{
    add(_childImage);
    _background = Gdk::Pixbuf::create_from_file(RootDirectory + "white_bg.gif", FULLSIZE_CARD_WIDTH, FULLSIZE_CARD_HEIGHT * 1.25, false);
    _childImage.set(_background);
    show_all();
    signal_button_release_event().connect(sigc::mem_fun(*this, &CardHolder::toggle));
    
}
bool CardHolder::toggle(GdkEventButton *e)
{
    if (ptr->empty() || vectorSlot >= ptr->size())
        return true;
    _toggled = !_toggled;
    update();
    return true;
}
void CardHolder::update()
{
    if (ptr->empty() || vectorSlot >= ptr->size())
        return;
    
    int w = ptr->at(vectorSlot).getScaledChildPixbuf()->get_width();
    int h = ptr->at(vectorSlot).getScaledChildPixbuf()->get_height();
    int down = ptr->at(vectorSlot).getScaledChildPixbuf()->get_height() * 0.2;
    Glib::RefPtr<Gdk::Pixbuf> pic = Gdk::Pixbuf::create_from_file(RootDirectory + "white_bg.gif")->scale_simple(w, h * 1.25, Gdk::INTERP_BILINEAR);

    ptr->at(vectorSlot).getScaledChildPixbuf()->composite(pic, 0, (_toggled ? 0 : down), w, h, 0, (_toggled ? 0 : down), 1, 1, Gdk::INTERP_HYPER, 255);
    _childImage.set(pic);

}
*/

class CardButton
    :public Gtk::Button
{
    size_t _slot;
    Gtk::Image _childImage;
    double _scale;
    bool _sideways;
    bool _popupMenuNeeded;
	
public:
	Glib::RefPtr<Gtk::ActionGroup> _actionGroup;
    Glib::RefPtr<Gtk::UIManager> _UIManager;
	Gtk::Menu* _menuPopup;
	vector<Deed>* ptr;
    CardButton(size_t slot, double scale);

    sigc::signal<void, size_t> IDsignal;
	sigc::signal<bool, size_t, GdkEventButton*> IDsignal_withEvent;
    sigc::signal<void, Deed&, size_t> bannerSignal;
	sigc::signal<void, Deed&, size_t> bannerToBondedSignal;
	sigc::signal<void, CardButton&> selfSignal;
	sigc::signal<void> meditationSignal;
    
    void sendID() { IDsignal.emit(_slot); }
	bool sendID_withEvent(GdkEventButton* e) { IDsignal_withEvent.emit(_slot, e); return false;}
	void sendBanner(size_t targetSlot) {
		if (ptr == NULL || _slot >= ptr->size())
			return ;
		bannerSignal.emit(ptr->at(_slot), targetSlot);
	}
	void sendBannerToBonded(size_t targetSlot) {
		if (ptr == NULL || _slot >= ptr->size())
			return ;
		bannerToBondedSignal.emit(ptr->at(_slot), targetSlot);
	}

	void sendMeditationSignal() { meditationSignal.emit();}
	
    void update() {
        if (ptr != NULL && _slot < ptr->size()) {
            if (_sideways)
                _childImage.set(ptr->at(_slot).getScaledChildPixbuf(_scale)->rotate_simple(Gdk::PIXBUF_ROTATE_CLOCKWISE));
            else
                _childImage.set(ptr->at(_slot).getScaledChildPixbuf(_scale));
            show();
        }
        else {
            hide();
        }
    }
    void setScale (double s) {
        _scale = s;
        update();
    }
    void setSlot (size_t s) {
        if (_slot < ptr->size()) 
        _slot = s;
    }
	size_t getSlot() const {
		return _slot;
	}

    void rotatePic() {_sideways = !_sideways;}

    bool onMouseOver(int, int, bool, const Glib::RefPtr<Gtk::Tooltip>& tooltip) {
        if (ptr == NULL || _slot >= ptr->size()) 
            return false;
        tooltip->set_icon(ptr->at(_slot).getChildPixbuf());
        return true;
    }

	void setPopupMenuNeeded(bool need) {
		_popupMenuNeeded = need;
	}

	bool onRightClick(GdkEventButton* e) {
		if (ptr == NULL || _slot >= ptr->size() || e->button != 3)
			return false;
		selfSignal.emit(*this);
		if (_popupMenuNeeded)
			_menuPopup->popup(e->button, e->time);
		return false;
	}

};
CardButton::CardButton(size_t slot, double scale = 1.0)
    :_slot(slot), _scale(scale), ptr(NULL), _sideways(false), _popupMenuNeeded(false)
{
    add(_childImage);
    signal_pressed().connect(sigc::mem_fun(*this, &CardButton::sendID));
	signal_button_press_event().connect(sigc::mem_fun(*this, &CardButton::sendID_withEvent), false);
	set_has_tooltip();
    signal_query_tooltip().connect(sigc::mem_fun(*this, &CardButton::onMouseOver));
    //signal_pressed().connect(sigc::mem_fun(*this, &CardButton::sendID_withVector));
    
	signal_button_press_event().connect(sigc::mem_fun(*this, &CardButton::onRightClick), false);

	_actionGroup = Gtk::ActionGroup::create("Special Actions");
	_actionGroup->add(Gtk::Action::create("Meditation", "Cast Meditation"), sigc::mem_fun(*this, &CardButton::sendMeditationSignal));
	_actionGroup->add(Gtk::Action::create("Attach Banner 0", ""),          sigc::bind<size_t>(sigc::mem_fun(*this, &CardButton::sendBanner), 0));
	_actionGroup->add(Gtk::Action::create("Attach Banner 1", ""),          sigc::bind<size_t>(sigc::mem_fun(*this, &CardButton::sendBanner), 1));
	_actionGroup->add(Gtk::Action::create("Attach Banner 2", ""),          sigc::bind<size_t>(sigc::mem_fun(*this, &CardButton::sendBanner), 2));
	_actionGroup->add(Gtk::Action::create("Attach Banner 3", ""),          sigc::bind<size_t>(sigc::mem_fun(*this, &CardButton::sendBanner), 3));
	_actionGroup->add(Gtk::Action::create("Attach Banner 4", ""),          sigc::bind<size_t>(sigc::mem_fun(*this, &CardButton::sendBanner), 4));
	_actionGroup->add(Gtk::Action::create("Attach Banner 5", ""),          sigc::bind<size_t>(sigc::mem_fun(*this, &CardButton::sendBannerToBonded), 0));
	
    _UIManager = Gtk::UIManager::create();
	_UIManager->insert_action_group(_actionGroup);
	
	Glib::ustring ui_info = 
		"<ui>"
        "  <popup name='Special Actions'>"
		"    <menuitem action='Meditation'/>"
        "    <menuitem action='Attach Banner 0'/>"
		"    <menuitem action='Attach Banner 1'/>"
        "    <menuitem action='Attach Banner 2'/>"
		"    <menuitem action='Attach Banner 3'/>"
		"    <menuitem action='Attach Banner 4'/>"
		"    <menuitem action='Attach Banner 5'/>"
		"  </popup>"
        "</ui>";
    try {  
        _UIManager->add_ui_from_string(ui_info);
	}
	catch (const Glib::Error& ex) {
        cout << "building menus failed: " <<  ex.what();
    }
	_menuPopup = dynamic_cast<Gtk::Menu*>(_UIManager->get_widget("/Special Actions")); 
	_menuPopup->show_all();

	
}
/*
bool CardButton::placeholderMade = false;
Glib::RefPtr<Gdk::Pixbuf> CardButton::placeholder;
*/
class CycleButton
    :public Gtk::Button
{
    vector<string> _labelList;
    Gtk::Label _label;
    size_t slot;
public:
    CycleButton()
        :slot(0)
    {
        signal_clicked().connect(sigc::mem_fun(*this, &CycleButton::cycle));
        _label.set_markup("<span font_family = \"Sakkal Majalla\" size=\"14336\">No Label</span>");
        add(_label);
    }
    void addCycleLabel(const string& s) {
        _labelList.push_back(s);
    }
    size_t getState() const {
        return slot;
    }
    void setToSlot(size_t n) {
        if (n >= _labelList.size())
            return;
        slot = n;
        _label.set_markup("<span font_family = \"Sakkal Majalla\" size=\"14336\">" +  (_labelList[slot]) + "</span>"); 
    }
    void cycle() {
        if (_labelList.empty())
            return;
        ++slot;
        if (slot >= _labelList.size())
            slot = 0;
        _label.set_markup("<span font_family = \"Sakkal Majalla\" size=\"14336\">" +  (_labelList[slot]) + "</span>"); 
    }
    
};

class vectorDeedTableOfButtons
    :public vector<Deed>, public Gtk::Table
{
    double _scale;
public:
    CardButton deed00_button;
    CardButton deed01_button;
    CardButton deed02_button;
    CardButton deed03_button;
    CardButton deed04_button;
    CardButton deed05_button;
    CardButton deed06_button;
    CardButton deed07_button;
    CardButton deed08_button;
    CardButton deed09_button;
    vector<CardButton*> _cardButtonPtrs;

    vectorDeedTableOfButtons(double _scale);

    void setScale(double s) {
        _scale = s;
        for (int i = 0; i < _cardButtonPtrs.size(); ++i) {
            _cardButtonPtrs[i]->setScale(_scale);
        }
    }
    double getScale() const {return _scale;}

    void update() {
        for (int i = 0; i < _cardButtonPtrs.size(); ++i) 
            _cardButtonPtrs[i]->update();
        //_cardButtonPtrs.size() ...etc hid buttons?
    }

    void push_back(const Deed& d) {
        vector<Deed>::push_back(d);
        if (size() <= _cardButtonPtrs.size())
            _cardButtonPtrs[size() - 1]->update();
        else
            update();
    }
    void pop_back() {
        vector<Deed>::pop_back();
        if (size() <= _cardButtonPtrs.size())
            _cardButtonPtrs[size()]->update(); 
        else
            update();
    }
    void erase(size_t pos) {
        if (pos >= size())
            return; 
        vector<Deed>::erase(this->begin() + pos);
        update();
    }
};

vectorDeedTableOfButtons::vectorDeedTableOfButtons(double _scale = 1)
    :deed00_button(0, _scale),
     deed01_button(1, _scale),
     deed02_button(2, _scale),
     deed03_button(3, _scale),
     deed04_button(4, _scale),
     deed05_button(5, _scale),
     deed06_button(6, _scale),
     deed07_button(7, _scale),
     deed08_button(8, _scale),
     deed09_button(9, _scale)
{
    _cardButtonPtrs.push_back(&deed00_button);
    _cardButtonPtrs.push_back(&deed01_button);
    _cardButtonPtrs.push_back(&deed02_button);
    _cardButtonPtrs.push_back(&deed03_button);
    _cardButtonPtrs.push_back(&deed04_button);
    _cardButtonPtrs.push_back(&deed05_button);
    _cardButtonPtrs.push_back(&deed06_button);
    _cardButtonPtrs.push_back(&deed07_button);
    _cardButtonPtrs.push_back(&deed08_button);
    _cardButtonPtrs.push_back(&deed09_button);

    for (int i = 0; i < _cardButtonPtrs.size(); ++i) {
        _cardButtonPtrs.at(i)->ptr = this;
        _cardButtonPtrs.at(i)->setSlot(i);
        _cardButtonPtrs.at(i)->setScale(_scale);
    }
    
    show_all();
}

struct BackupVectors {
    vector<Deed> deck; 
    vectorDeedTableOfButtons hand;
    vectorDeedTableOfButtons inPlay;
    vectorDeedTableOfButtons inSideways;
    vectorDeedTableOfButtons inTrash;
    vector<Deed> discardPile;
};

class DeckViewer
    :public Gtk::Notebook
{
    
public:
    Gtk::Window window;
	Gtk::HButtonBox _hButtonBox;
	Gtk::VBox _vBox;
	
    DeckViewer() {
        set_tab_pos(Gtk::POS_RIGHT);
        set_scrollable();
        window.add(*this);
        window.set_default_size(FULLSIZE_CARD_WIDTH + 50, FULLSIZE_CARD_HEIGHT);
    }

    void setDeckToView(const vector<Deed>& deck) {
        while (get_n_pages() > 0) 
            remove_page(-1);
        for (int i = 0; i < deck.size(); ++i) {
            Gtk::Image* image_ptr = Gtk::manage(new Gtk::Image);
            Gtk::Label* label_ptr = Gtk::manage(new Gtk::Label);
            string caps = deck[i].getName();
            for (int j = 0; j < caps.size(); ++j)
                caps[j] = toupper(caps[j]);
            label_ptr->set_markup("<span font_family = \"Gazette\" size=\"14336\">" + caps + "</span>");
            image_ptr->set(deck[i].getScaledChildPixbuf(.8));
			prepend_page(*image_ptr, *label_ptr);
        }
        show_all();
    }
    
};

class HandViewer
    :public Gtk::Window
{
public:
    enum ActiveWindow {
        PLAY,
        SIDEWAYS,
        TRASH
    };

    Gtk::Window selectPlayerWindow;
    KnightName _knightName;

    vector<Deed> deck; 
    vectorDeedTableOfButtons hand;
    vectorDeedTableOfButtons inPlay;
    vectorDeedTableOfButtons inSideways;
    vectorDeedTableOfButtons inTrash;
    vector<Deed> discardPile;
	vector<Deed> sparingPowerPile;

    BackupVectors savedVectors1;
    BackupVectors savedVectors2;

    Gtk::Table _mainTable;
    
    Gtk::HButtonBox _centerTopButtonBox;
    Gtk::HButtonBox _centerBottomButtonBox;

    ActiveWindow _activeWindow;
    CycleButton _cyclebutton;

    Gtk::Button takeWoundButton;
    Gtk::Button healWoundButton;
    Gtk::Button drawCardButton;
    Gtk::Button proceedButton;
    Gtk::Button sendToTopOfDeckButton;
    Gtk::Button sendToBottomOfDeckButton;
    Gtk::Button shuffleButton;
    Gtk::Button undoButton; //state restored to last proceed
	Gtk::Button openDeckButton;
    Gtk::Button openDiscardButton;

    Gtk::Label takeWoundLabel;
    Gtk::Label healWoundLabel;
    Gtk::Label drawCardLabel;
    Gtk::Label proceedLabel;
    Gtk::Label sendToTopOfDeckLabel;
    Gtk::Label sendToBottomOfDeckLabel;
    Gtk::Label shuffleLabel;
    Gtk::Label undoLabel;
	Gtk::Label openDeckLabel;
    Gtk::Label openDiscardLabel;
    
    Gtk::ScrolledWindow topScroll;
    Gtk::ScrolledWindow bottomLeftScroll;
    Gtk::ScrolledWindow bottomCenterScroll;
    Gtk::ScrolledWindow bottomRightScroll;
   
    Gtk::HBox miniHandBox;
    Gtk::Image mini00;
    Gtk::Image mini01;
    Gtk::Image mini02;
    Gtk::Image mini03;
    Gtk::Image mini04;
    Gtk::Image mini05;
    Gtk::Image mini06;
    Gtk::Image mini07;
    Gtk::Image mini08;
    Gtk::Image mini09;
   
    Glib::RefPtr<Gdk::Pixbuf> backgroundPixbuf;
    Gtk::EventBox _eventBox;

	DeckViewer _deedPileViewer;
    DeckViewer _discardViewer;
    
    HandViewer();
    
    void drawCards(size_t n) {
        while (deck.size() && n) {
            hand.push_back(deck.back());
            deck.pop_back();
            --n;
        }
        updateTitle();
    }

    void drawToTargetSize(size_t n) {
        while (hand.size() < n && deck.size()) {
            drawCards(1); 
        }
         updateTitle();
    }
    
    void putInDeck(const Deed& d) {
        deck.push_back(d);
    }
    void putInDiscard(const Deed& d) {
        discardPile.push_back(d);
    }
    void putInHand(const Deed& d) {
        hand.push_back(d);
    }
    void restartAndShuffle() {
        deck.insert(deck.end(), discardPile.begin(), discardPile.end());
        discardPile.clear();
        deck.insert(deck.end(), hand.begin(), hand.end());
        hand.clear();
        deck.insert(deck.end(), inPlay.begin(), inPlay.end());
        inPlay.clear();
        inPlay.update();
        deck.insert(deck.end(), inSideways.begin(), inSideways.end());
        inSideways.clear();
        inSideways.update();
        inTrash.clear();
        random_shuffle(deck.begin(), deck.end());

    }
    void transferFromHand (size_t slot) {
        if (slot >= hand.size()) {
            return;
        }
        switch(_activeWindow) {
        case PLAY:
            inPlay.push_back(hand[slot]);
            break;
        case SIDEWAYS:
            inSideways.push_back(hand[slot]);
            break;
        case TRASH:
            inTrash.push_back(hand[slot]);
            break;
        }
        hand.erase(slot);
        hand.update();

    }
    void transferFromInPlay (size_t slot) {
        if (slot >= inPlay.size()) {
            return;
        }
        hand.push_back(inPlay[slot]);
        inPlay.erase(slot);
    }
    void transferFromSideways (size_t slot) {
        if (slot >= inSideways.size()) {
            return;
        }
        hand.push_back(inSideways[slot]);
        inSideways.erase(slot);
    }
    void transferFromInTrash (size_t slot) {
        if (slot >= inTrash.size()) {
            return;
        }
        hand.push_back(inTrash[slot]);
        inTrash.erase(slot);
    }

    void takeWound() {
        hand.push_back(Deed());
    }
    void healFromDiscard() {
        for (auto i = discardPile.begin(); i != discardPile.end(); ++i) {
            if (i->isWound()) {
                discardPile.erase(i);
				_discardViewer.setDeckToView(discardPile);
                return;
            }
        }
    }
    void healFromDeck() {
        for (auto i = deck.begin(); i != deck.end(); ++i) {
            if (i->isWound()) {
                deck.erase(i);
                return;
            }
        }
        updateTitle();
    }
    void healFromHand() {
        for (auto i = 0; i < hand.size(); ++i) {
            if (hand[i].isWound()) {
                hand.erase(i);
                return;
            }
        }
    }
    //don't forget Knock Out
    
   
	void sendToTop() {
		//has to be done one at a time for button updates
		while (!inPlay.empty()) {
	        deck.push_back(inPlay.front());
			inPlay.erase(0);
		}
		updateTitle();
	}
	void sendToBottom() {
		//has to be done one at a time for button updates
		while (!inPlay.empty()) {
	        deck.insert(deck.begin(), inPlay.front());
			inPlay.erase(0);
		}
		updateTitle(); 
	}

    void shuffleDeck() {
		random_shuffle(deck.begin(), deck.end());
		updateTitle();
	}

    void cycleWindow() {
        _activeWindow = ActiveWindow(_cyclebutton.getState());
        if (_activeWindow == TRASH) {
            _mainTable.remove(bottomCenterScroll);
            _mainTable.attach(bottomRightScroll, 1, 2, 3, 4, Gtk::SHRINK, Gtk::SHRINK);
            
        }
        if (_activeWindow == PLAY)  {
            _mainTable.remove(bottomRightScroll);
            _mainTable.attach(bottomCenterScroll, 1, 2, 3, 4, Gtk::SHRINK, Gtk::SHRINK);
        }
    }

	void openDeedPile() {
		_deedPileViewer.setDeckToView(deck);
        if (!_deedPileViewer.window.get_visible()) {
            _deedPileViewer.window.show_all();
        }
        else {
            _deedPileViewer.window.hide();
        }

    }

    void openDiscardPile() {
        _discardViewer.setDeckToView(discardPile);
        if (!_discardViewer.window.get_visible()) {
            _discardViewer.window.show_all();
        }
        else {
            _discardViewer.window.hide();
        }

    }

	bool sendToHandFromDeckViewer(GdkEventButton* e) {
        if (deck.empty())
            return false;
		if (e->type == GDK_2BUTTON_PRESS && e->button == 1 && -1 < _deedPileViewer.get_current_page() && _deedPileViewer.get_current_page() < deck.size()) {
			size_t page = deck.size() - 1 - _deedPileViewer.get_current_page();
            hand.push_back(deck[page]);
            deck.erase(deck.begin() + page);
            _deedPileViewer.remove_page(_deedPileViewer.get_current_page());
        }
		ostringstream oss;
        oss << "Player Hand -- Cards remaining: " << deck.size();
		if (!sparingPowerPile.empty()) {
			oss << "    Sparing Power cards: " << sparingPowerPile.size();
		}
		set_title(oss.str());
		return true;
    }

    bool sendToHandFromDiscardViewer(GdkEventButton* e) {
        if (discardPile.empty())
            return false;
        if (e->type == GDK_2BUTTON_PRESS && e->button == 1 && -1 < _discardViewer.get_current_page() && _discardViewer.get_current_page() < discardPile.size()) {
			size_t page = discardPile.size() - 1 - _discardViewer.get_current_page();
            hand.push_back(discardPile[page]);
            discardPile.erase(discardPile.begin() + page);
			_discardViewer.remove_page(_discardViewer.get_current_page());
        }
		return true;
    }

    void proceed();

    void saveState();
    void restoreState();

    void updateTitle() {
        ostringstream oss;
        oss << "Player Hand -- Cards remaining: " << deck.size();
		if (!sparingPowerPile.empty()) {
			oss << "    Sparing Power cards: " << sparingPowerPile.size();
		}
        set_title(oss.str());
		updateDeckViewer(); 
	}

	void updateDeckViewer() {
		if (_deedPileViewer.window.get_visible())
			_deedPileViewer.setDeckToView(deck);
	}

	void updateDiscardViewer() {
		if (_discardViewer.window.get_visible())
			_discardViewer.setDeckToView(discardPile);
	} 
	
    void updateAll() {
        hand.update();
        inPlay.update();
        inSideways.update();
        inTrash.update();
        updateTitle();
    }

    bool updateMinihand(GdkEventAny* e) { 
        miniHandBox.foreach(sigc::mem_fun(&Gtk::Image::hide));
        switch (hand.size()) { //all statements fallthrough (no break)
        case 10: mini09.set(hand[9].getChildPixbuf()->scale_simple(167, 240, Gdk::INTERP_HYPER)); mini09.show_all();
        case 9: mini08.set(hand[8].getChildPixbuf()->scale_simple(167, 240, Gdk::INTERP_HYPER)); mini08.show_all();
        case 8: mini07.set(hand[7].getChildPixbuf()->scale_simple(167, 240, Gdk::INTERP_HYPER)); mini07.show_all();
        case 7: mini06.set(hand[6].getChildPixbuf()->scale_simple(167, 240, Gdk::INTERP_HYPER)); mini06.show_all();
        case 6: mini05.set(hand[5].getChildPixbuf()->scale_simple(167, 240, Gdk::INTERP_HYPER)); mini05.show_all();
        case 5: mini04.set(hand[4].getChildPixbuf()->scale_simple(167, 240, Gdk::INTERP_HYPER)); mini04.show_all();
        case 4: mini03.set(hand[3].getChildPixbuf()->scale_simple(167, 240, Gdk::INTERP_HYPER)); mini03.show_all();
        case 3: mini02.set(hand[2].getChildPixbuf()->scale_simple(167, 240, Gdk::INTERP_HYPER)); mini02.show_all();
        case 2: mini01.set(hand[1].getChildPixbuf()->scale_simple(167, 240, Gdk::INTERP_HYPER)); mini01.show_all();
        case 1: mini00.set(hand[0].getChildPixbuf()->scale_simple(167, 240, Gdk::INTERP_HYPER)); mini00.show_all();
        }
        return false;
    }

    bool updateMinihand2(GdkEvent* e) { 
        miniHandBox.foreach(sigc::mem_fun(&Gtk::Image::hide));
        switch (hand.size()) { //all statements fallthrough (no break)
        case 10: mini09.set(hand[9].getChildPixbuf()->scale_simple(167, 240, Gdk::INTERP_HYPER)); mini09.show_all();
        case 9: mini08.set(hand[8].getChildPixbuf()->scale_simple(167, 240, Gdk::INTERP_HYPER)); mini08.show_all();
        case 8: mini07.set(hand[7].getChildPixbuf()->scale_simple(167, 240, Gdk::INTERP_HYPER)); mini07.show_all();
        case 7: mini06.set(hand[6].getChildPixbuf()->scale_simple(167, 240, Gdk::INTERP_HYPER)); mini06.show_all();
        case 6: mini05.set(hand[5].getChildPixbuf()->scale_simple(167, 240, Gdk::INTERP_HYPER)); mini05.show_all();
        case 5: mini04.set(hand[4].getChildPixbuf()->scale_simple(167, 240, Gdk::INTERP_HYPER)); mini04.show_all();
        case 4: mini03.set(hand[3].getChildPixbuf()->scale_simple(167, 240, Gdk::INTERP_HYPER)); mini03.show_all();
        case 3: mini02.set(hand[2].getChildPixbuf()->scale_simple(167, 240, Gdk::INTERP_HYPER)); mini02.show_all();
        case 2: mini01.set(hand[1].getChildPixbuf()->scale_simple(167, 240, Gdk::INTERP_HYPER)); mini01.show_all();
        case 1: mini00.set(hand[0].getChildPixbuf()->scale_simple(167, 240, Gdk::INTERP_HYPER)); mini00.show_all();
        }
        return false;
    }


    void selectPlayerPrompt() {
        
        Gtk::Image norowasImage;
        Gtk::Image goldyxImage;
        Gtk::Image arytheaImage;
        Gtk::Image tovakImage;
        norowasImage.set(Gdk::Pixbuf::create_from_file(RootDirectory + "Norowas_full.jpg", 240, 300, true));
        goldyxImage .set(Gdk::Pixbuf::create_from_file(RootDirectory + "Goldyx_full.jpg" , 240, 300, true));
        arytheaImage.set(Gdk::Pixbuf::create_from_file(RootDirectory + "Arythea_full.jpg", 240, 300, true));
        tovakImage  .set(Gdk::Pixbuf::create_from_file(RootDirectory + "Tovak_full.jpg"  , 240, 300, true));
        Gtk::Button norowasButton;    norowasButton.add(norowasImage);
        Gtk::Button goldyxButton;     goldyxButton .add(goldyxImage);
        Gtk::Button arytheaButton;    arytheaButton.add(arytheaImage);
        Gtk::Button tovakButton;      tovakButton  .add(tovakImage);
        Gtk::Table table(1, 4, true);
        table.attach(norowasButton, 0, 1, 0, 1);
        table.attach(goldyxButton , 1, 2, 0, 1);
        table.attach(arytheaButton, 2, 3, 0, 1);
        table.attach(tovakButton  , 3, 4, 0, 1);
        selectPlayerWindow.add(table);
        selectPlayerWindow.set_title("Select your mage knight");
        selectPlayerWindow.set_icon(Gdk::Pixbuf::create_from_file(RootDirectory + "blank_unit_button.png"));
		selectPlayerWindow.set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        selectPlayerWindow.show_all();
        selectPlayerWindow.set_modal();

        norowasButton .signal_clicked().connect(sigc::bind<KnightName> (sigc::mem_fun(*this, &HandViewer::onKnightSelection), NOROWAS));
        goldyxButton  .signal_clicked().connect(sigc::bind<KnightName> (sigc::mem_fun(*this, &HandViewer::onKnightSelection), GOLDYX )); 
        arytheaButton .signal_clicked().connect(sigc::bind<KnightName> (sigc::mem_fun(*this, &HandViewer::onKnightSelection), ARYTHEA));
        tovakButton   .signal_clicked().connect(sigc::bind<KnightName> (sigc::mem_fun(*this, &HandViewer::onKnightSelection), TOVAK  )); 
        Gtk::Main::run();

    }

    void onKnightSelection(KnightName n) {
        _knightName = n;
        switch (n) {
        case NOROWAS:
            for (auto i = deck.begin(); i != deck.end(); ++i) {
                if (i->getName() == "Promise") {
                    *i = Deed ("Noble Manners", Deed::WHITE, Deed::BASIC_ACTION);
                    break;
                }
            }
            break;
        case GOLDYX:
            for (auto i = deck.begin(); i != deck.end(); ++i) {
                if (i->getName() == "Concentration") {
                    *i = Deed ("Will Focus", Deed::GREEN, Deed::BASIC_ACTION);
                    break;
                }
            }
            break;
        case ARYTHEA:
            for (auto i = deck.begin(); i != deck.end(); ++i) {
                if (i->getName() == "Rage") {
                    *i = Deed ("Battle Versatility", Deed::RED, Deed::BASIC_ACTION);
                    break;
                }
            }
            break;
        case TOVAK:
            for (auto i = deck.begin(); i != deck.end(); ++i) {
                if (i->getName() == "Determination") {
                    *i = Deed ("Cold Toughness", Deed::WHITE, Deed::BASIC_ACTION);
                    break;
                }
            }
            break;
        default:
            cout << "Invalid player selection. Exiting program.\n";
            exit(EXIT_FAILURE);
        }
        random_shuffle(deck.begin(), deck.end());
        drawToTargetSize(5);
        selectPlayerWindow.hide();
        updateAll();
		Gtk::Main::quit();
	}

	void longNight() {
		for (size_t counter = 3; counter > 0 && !discardPile.empty(); --counter) {
			size_t random = rand() % discardPile.size();
			deck.push_back(discardPile[random]);
			discardPile.erase(discardPile.begin() + random);
		}
		_discardViewer.setDeckToView(discardPile);
		updateTitle();
	}

	void SparingPowerGiveOne() {
		if (!deck.empty()) {
			sparingPowerPile.push_back(deck.back());
			deck.pop_back();
			updateTitle();
		}
	}
	void SparingPowerTakeAll() {
		while (!sparingPowerPile.empty()) {
			hand.push_back(sparingPowerPile.back());
			sparingPowerPile.pop_back();
		}
		updateTitle();
	}
	void SparingPowerClear() {
		while (!sparingPowerPile.empty()) {
			deck.push_back(sparingPowerPile.back());
			sparingPowerPile.pop_back();
		}
		updateTitle();
	}

	void meditation() {
		for (size_t counter = 2; counter > 0 && !discardPile.empty(); --counter) {
			size_t random = rand() % discardPile.size();
			hand.push_back(discardPile[random]);
			discardPile.erase(discardPile.begin() + random);
		}
		_discardViewer.setDeckToView(discardPile);
		updateTitle();
	}

	void findAndEraseBanner(const Deed& banner) {
		for (auto i = 0; i != hand.size(); ++i) {
			if (hand.at(i).getName() == banner.getName()) {
				hand.erase(i);
				updateAll();
				break;
			}
		}
	}

};

HandViewer::HandViewer()
    : _activeWindow(PLAY)
{
    set_title("Player Hand");
    set_keep_above(true);
    set_resizable(false);
    
    hand.Gtk::Table::resize(1, 10);
    inPlay.Gtk::Table::resize(3, 3);
    inSideways.Gtk::Table::resize(3, 3);
    inTrash.Gtk::Table::resize(3, 3);

    double currentScale = .4;
    hand.setScale      (currentScale * 1.25);
    inPlay.setScale    (currentScale);
    inSideways.setScale(currentScale);
    inTrash.setScale   (currentScale);

    hand.attach(hand.deed00_button, 0, 1, 0, 1, Gtk::SHRINK);
    hand.attach(hand.deed01_button, 1, 2, 0, 1, Gtk::SHRINK);
    hand.attach(hand.deed02_button, 2, 3, 0, 1, Gtk::SHRINK);
    hand.attach(hand.deed03_button, 3, 4, 0, 1, Gtk::SHRINK);
    hand.attach(hand.deed04_button, 4, 5, 0, 1, Gtk::SHRINK);
    hand.attach(hand.deed05_button, 5, 6, 0, 1, Gtk::SHRINK);
    hand.attach(hand.deed06_button, 6, 7, 0, 1, Gtk::SHRINK);
    hand.attach(hand.deed07_button, 7, 8, 0, 1, Gtk::SHRINK);
    hand.attach(hand.deed08_button, 8, 9, 0, 1, Gtk::SHRINK);
    hand.attach(hand.deed09_button, 9, 10, 0, 1, Gtk::SHRINK);

    inPlay.attach(inPlay.deed00_button, 0, 1, 0, 1, Gtk::SHRINK);
    inPlay.attach(inPlay.deed01_button, 1, 2, 0, 1, Gtk::SHRINK);
    inPlay.attach(inPlay.deed02_button, 2, 3, 0, 1, Gtk::SHRINK);
    inPlay.attach(inPlay.deed03_button, 0, 1, 1, 2, Gtk::SHRINK);
    inPlay.attach(inPlay.deed04_button, 1, 2, 1, 2, Gtk::SHRINK);

    inSideways.attach(inSideways.deed00_button, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
    inSideways.attach(inSideways.deed01_button, 1, 2, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
    inSideways.attach(inSideways.deed02_button, 0, 1, 1, 2, Gtk::SHRINK, Gtk::SHRINK);
    inSideways.attach(inSideways.deed03_button, 1, 2, 1, 2, Gtk::SHRINK, Gtk::SHRINK);
    inSideways.attach(inSideways.deed04_button, 0, 1, 2, 3, Gtk::SHRINK, Gtk::SHRINK);

    inTrash.attach(inTrash.deed00_button, 0, 1, 0, 1, Gtk::SHRINK);
    inTrash.attach(inTrash.deed01_button, 1, 2, 0, 1, Gtk::SHRINK);
    inTrash.attach(inTrash.deed02_button, 2, 3, 0, 1, Gtk::SHRINK);
    inTrash.attach(inTrash.deed03_button, 0, 1, 1, 2, Gtk::SHRINK);
    inTrash.attach(inTrash.deed04_button, 1, 2, 1, 2, Gtk::SHRINK);
	  
    
    _cyclebutton.addCycleLabel("Play Area");
    _cyclebutton.addCycleLabel("Sideways");
    _cyclebutton.addCycleLabel("Trash");
    _cyclebutton.setToSlot(0);
    _cyclebutton.signal_clicked().connect(sigc::mem_fun(*this, &HandViewer::cycleWindow));

    string fontsizeNormal = "14336";
    proceedLabel.set_markup("<span font_family = \"Sakkal Majalla\" size=\"" + fontsizeNormal + "\">Proceed</span>"); 
    proceedButton.add(proceedLabel);
    proceedButton.signal_clicked().connect(sigc::mem_fun(*this, &HandViewer::proceed));

    undoLabel.set_markup("<span font_family = \"Sakkal Majalla\" size=\"" + fontsizeNormal + "\">Undo last turn</span>"); 
    undoButton.add(undoLabel);
    undoButton.signal_clicked().connect(sigc::mem_fun(*this, &HandViewer::restoreState));

    drawCardLabel.set_markup("<span font_family = \"Sakkal Majalla\" size=\"" + fontsizeNormal + "\">Draw card</span>"); 
    drawCardButton.add(drawCardLabel);
    drawCardButton.signal_clicked().connect(sigc::bind<size_t>(sigc::mem_fun(*this, &HandViewer::drawCards), 1));
    
    takeWoundLabel.set_markup("<span font_family = \"Sakkal Majalla\" size=\"" + fontsizeNormal + "\">Draw a wound</span>"); 
    takeWoundButton.add(takeWoundLabel);
    takeWoundButton.signal_clicked().connect(sigc::mem_fun(*this, &HandViewer::takeWound));

    healWoundLabel. set_markup("<span font_family = \"Sakkal Majalla\" size=\"" + fontsizeNormal + "\">Heal from discard pile</span>"); 
    healWoundButton.add(healWoundLabel);
    healWoundButton.signal_clicked().connect(sigc::mem_fun(*this, &HandViewer::healFromDiscard));

    sendToTopOfDeckLabel .set_markup("<span font_family = \"Sakkal Majalla\" size=\"" + fontsizeNormal + "\">Send to top of deck</span>"); 
    sendToTopOfDeckButton.add(sendToTopOfDeckLabel);
    sendToTopOfDeckButton.signal_clicked().connect(sigc::mem_fun(*this, &HandViewer::sendToTop));

    sendToBottomOfDeckLabel .set_markup("<span font_family = \"Sakkal Majalla\" size=\"" + fontsizeNormal + "\">Send to bottom of deck</span>"); 
    sendToBottomOfDeckButton.add(sendToBottomOfDeckLabel);
    sendToBottomOfDeckButton.signal_clicked().connect(sigc::mem_fun(*this, &HandViewer::sendToBottom));

    shuffleLabel .set_markup("<span font_family = \"Sakkal Majalla\" size=\"" + fontsizeNormal + "\">Shuffle draw deck</span>");  
    shuffleButton.add(shuffleLabel);
    shuffleButton.signal_clicked().connect(sigc::mem_fun(*this, &HandViewer::shuffleDeck));

	openDeckLabel.set_markup("<span font_family = \"Sakkal Majalla\" size=\"" + fontsizeNormal + "\">View deck</span>");  
    openDeckButton.add(openDeckLabel);
	openDeckButton.signal_clicked().connect(sigc::mem_fun(*this, &HandViewer::openDeedPile));

    openDiscardLabel.set_markup("<span font_family = \"Sakkal Majalla\" size=\"" + fontsizeNormal + "\">View discard pile</span>");  
    openDiscardButton.add(openDiscardLabel);
    openDiscardButton.signal_clicked().connect(sigc::mem_fun(*this, &HandViewer::openDiscardPile));

	_deedPileViewer.signal_button_press_event().connect(sigc::mem_fun(*this, &HandViewer::sendToHandFromDeckViewer));
    _discardViewer.signal_button_press_event().connect(sigc::mem_fun(*this, &HandViewer::sendToHandFromDiscardViewer));
    
    //_centerButtonBox.set_child_ipadding_x(10);
    _centerTopButtonBox   .set_spacing(5);
    _centerBottomButtonBox.set_spacing(5);
    _centerTopButtonBox.set_layout(Gtk::BUTTONBOX_CENTER);
    _centerBottomButtonBox.set_layout(Gtk::BUTTONBOX_CENTER);
    _centerTopButtonBox.set_child_min_width(160);
    _centerBottomButtonBox.set_child_min_width(160);
    
    _centerTopButtonBox.pack_start(drawCardButton);
    _centerTopButtonBox.pack_start(takeWoundButton);
    _centerTopButtonBox.pack_start(healWoundButton);
    _centerTopButtonBox.pack_start(sendToTopOfDeckButton); 
	_centerTopButtonBox.pack_start(openDeckButton); 

    _centerBottomButtonBox.pack_start(proceedButton); 
    _centerBottomButtonBox.pack_start(shuffleButton);
    _centerBottomButtonBox.pack_start(_cyclebutton);
    _centerBottomButtonBox.pack_start(sendToBottomOfDeckButton);
    _centerBottomButtonBox.pack_start(openDiscardButton);
   
   
    topScroll.add(hand);
    topScroll.set_shadow_type(Gtk::SHADOW_IN);
    topScroll.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_NEVER);
    topScroll.set_size_request(FULLSIZE_CARD_WIDTH * hand.getScale() * 5.5);
    bottomLeftScroll.add(inPlay);
    bottomLeftScroll.set_shadow_type(Gtk::SHADOW_IN);
    bottomLeftScroll.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    bottomLeftScroll.set_size_request(FULLSIZE_CARD_WIDTH * inPlay.getScale() * 3.3, FULLSIZE_CARD_HEIGHT * inPlay.getScale() * 1.1);
    bottomCenterScroll.add(inSideways);
    bottomCenterScroll.set_shadow_type(Gtk::SHADOW_IN);
    bottomCenterScroll.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    bottomCenterScroll.set_size_request(FULLSIZE_CARD_WIDTH * inPlay.getScale() * 3.3, FULLSIZE_CARD_HEIGHT * inPlay.getScale() * 1.1);
    _eventBox.modify_bg(Gtk::StateType::STATE_NORMAL, Gdk::Color("#000000"));
    _eventBox.add(inTrash);
    bottomRightScroll.add(_eventBox);
    bottomRightScroll.set_shadow_type(Gtk::SHADOW_ETCHED_OUT);
    bottomRightScroll.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    bottomRightScroll.set_size_request(FULLSIZE_CARD_WIDTH * inTrash.getScale() * 3.3, FULLSIZE_CARD_HEIGHT * inTrash.getScale() * 1.1);

    _mainTable.resize(4, 3);
    _mainTable.attach(topScroll,              0, 3, 0, 1, Gtk::FILL, Gtk::SHRINK);
    _mainTable.attach(_centerTopButtonBox,    0, 3, 1, 2, Gtk::SHRINK, Gtk::SHRINK);
    _mainTable.attach(_centerBottomButtonBox, 0, 3, 2, 3, Gtk::SHRINK, Gtk::SHRINK);
    _mainTable.attach(bottomLeftScroll,       0, 1, 3, 4, Gtk::SHRINK, Gtk::SHRINK);
    _mainTable.attach(bottomCenterScroll,     1, 2, 3, 4, Gtk::SHRINK, Gtk::SHRINK);
    //_mainTable.attach(bottomRightScroll,    2, 3, 2, 3, Gtk::SHRINK, Gtk::SHRINK);

    
    for (int i = 0; i < 10; ++i)
    {
        hand._cardButtonPtrs[i]->IDsignal.connect(sigc::mem_fun(*this, &HandViewer::transferFromHand));
        inPlay._cardButtonPtrs[i]->IDsignal.connect(sigc::mem_fun(*this, &HandViewer::transferFromInPlay));
        inSideways._cardButtonPtrs[i]->IDsignal.connect(sigc::mem_fun(*this, &HandViewer::transferFromSideways));
        inTrash._cardButtonPtrs[i]->IDsignal.connect(sigc::mem_fun(*this, &HandViewer::transferFromInTrash));
        inSideways._cardButtonPtrs[i]->rotatePic();
    }
    
    deck.push_back(Deed ("Rage",               Deed::RED,   Deed::BASIC_ACTION));
    deck.push_back(Deed ("Rage",               Deed::RED,   Deed::BASIC_ACTION));
    deck.push_back(Deed ("Determination",      Deed::BLUE,  Deed::BASIC_ACTION));
    deck.push_back(Deed ("Swiftness",          Deed::WHITE, Deed::BASIC_ACTION));
    deck.push_back(Deed ("Swiftness",          Deed::WHITE, Deed::BASIC_ACTION));
    deck.push_back(Deed ("March",              Deed::GREEN, Deed::BASIC_ACTION));
    deck.push_back(Deed ("March",              Deed::GREEN, Deed::BASIC_ACTION));
    deck.push_back(Deed ("Stamina",            Deed::BLUE , Deed::BASIC_ACTION));
    deck.push_back(Deed ("Stamina",            Deed::BLUE , Deed::BASIC_ACTION));
    deck.push_back(Deed ("Tranquility",        Deed::GREEN, Deed::BASIC_ACTION));
    deck.push_back(Deed ("Promise",            Deed::WHITE, Deed::BASIC_ACTION));
    deck.push_back(Deed ("Threaten",           Deed::RED,   Deed::BASIC_ACTION));
    deck.push_back(Deed ("Crystallize",        Deed::BLUE,  Deed::BASIC_ACTION));
    deck.push_back(Deed ("Mana Draw",          Deed::WHITE, Deed::BASIC_ACTION));
    deck.push_back(Deed ("Concentration",      Deed::GREEN, Deed::BASIC_ACTION));
    deck.push_back(Deed ("Improvisation",      Deed::RED,   Deed::BASIC_ACTION));
    
    savedVectors1.deck        = deck; 
    savedVectors1.hand      .vector<Deed>::operator=(hand);
    savedVectors1.inPlay    .vector<Deed>::operator=(inPlay);
    savedVectors1.inSideways.vector<Deed>::operator=(inSideways);
    savedVectors1.inTrash   .vector<Deed>::operator=(inTrash);
    savedVectors1.discardPile = discardPile;

    saveState();
    add(_mainTable);
    bottomRightScroll.show_all();
    
    inPlay.update();
    inSideways.update();
    inTrash.update();

    //Glib::RefPtr<Gdk::Pixmap> pmap;
    //Glib::RefPtr<Gdk::Bitmap> bmap;
    //Glib::RefPtr<Gdk::Pixbuf> pixb = Gdk::Pixbuf::create_from_file(RootDirectory + "tactics_night_back.jpg");
    //pixb->render_pixmap_and_mask(pmap, bmap, 125);
    //get_style()->set_bg_pixmap(Gtk::STATE_NORMAL, pmap);
    //hand.get_style()->set_bg_pixmap(Gtk::STATE_NORMAL, pmap);
    //inTrash.set_style(inTrash.get_style()->copy());
    //inTrash.get_style()->set_bg_pixmap(Gtk::STATE_NORMAL, pmap);
  
    
    miniHandBox.set_spacing(0);
    miniHandBox.pack_start(mini00, Gtk::PackOptions::PACK_SHRINK);
    miniHandBox.pack_start(mini01, Gtk::PackOptions::PACK_SHRINK);
    miniHandBox.pack_start(mini02, Gtk::PackOptions::PACK_SHRINK);
    miniHandBox.pack_start(mini03, Gtk::PackOptions::PACK_SHRINK);
    miniHandBox.pack_start(mini04, Gtk::PackOptions::PACK_SHRINK);
    miniHandBox.pack_start(mini05, Gtk::PackOptions::PACK_SHRINK);
    miniHandBox.pack_start(mini06, Gtk::PackOptions::PACK_SHRINK);
    miniHandBox.pack_start(mini07, Gtk::PackOptions::PACK_SHRINK);
    miniHandBox.pack_start(mini08, Gtk::PackOptions::PACK_SHRINK);
    miniHandBox.pack_start(mini09, Gtk::PackOptions::PACK_SHRINK);

	_deedPileViewer.window.set_icon(Gdk::Pixbuf::create_from_file(RootDirectory + "blank_unit_button.png"));
    _deedPileViewer.window.set_title("Double-click card name to place back in hand.");
    _deedPileViewer.window.set_transient_for(*this);
    _deedPileViewer.window.set_keep_above();

    _discardViewer.window.set_icon(Gdk::Pixbuf::create_from_file(RootDirectory + "blank_unit_button.png"));
    _discardViewer.window.set_title("Double-click card name to place back in hand.");
    _discardViewer.window.set_transient_for(*this);
    _discardViewer.window.set_keep_above();
    //signal_delete_event().connect(sigc::mem_fun(*this, &HandViewer::updateMinihand));
   //signal_event().connect(sigc::mem_fun(*this, &HandViewer::updateMinihand2));
}

void HandViewer::proceed() {
    while (!inPlay.empty()) {
        discardPile.push_back(inPlay.back());
        inPlay.pop_back();
    }
    while (!inSideways.empty()) {
        discardPile.push_back(inSideways.back());
        inSideways.pop_back();
    }
    while (!inTrash.empty()) {
        inTrash.pop_back();
    }
    _discardViewer.setDeckToView(discardPile);
    saveState();
}

void HandViewer::saveState() {
  
    savedVectors2.deck        =  savedVectors1.deck; 
	savedVectors2.hand.      vector<Deed>::operator=(savedVectors1.hand);
	savedVectors2.inPlay.    vector<Deed>::operator=(savedVectors1.inPlay);
    savedVectors2.inSideways.vector<Deed>::operator=(savedVectors1.inSideways);
    savedVectors2.inTrash.   vector<Deed>::operator=(savedVectors1.inTrash);
    savedVectors2.discardPile = savedVectors1.discardPile;

    savedVectors1.deck        = deck; 
    savedVectors1.hand      .vector<Deed>::operator=(hand);
    savedVectors1.inPlay    .vector<Deed>::operator=(inPlay);
    savedVectors1.inSideways.vector<Deed>::operator=(inSideways);
    savedVectors1.inTrash   .vector<Deed>::operator=(inTrash);
    savedVectors1.discardPile = discardPile;

  }

void HandViewer::restoreState() {

   deck = savedVectors2.deck; 
   hand.vector<Deed>::operator=(savedVectors2.hand);
   inPlay.vector<Deed>::operator=(savedVectors2.inPlay);
   inSideways.vector<Deed>::operator=(savedVectors2.inSideways);
   inTrash.vector<Deed>::operator=(savedVectors2.inTrash);
   discardPile = savedVectors2.discardPile;

   savedVectors1.deck        = deck; 
   savedVectors1.hand      .vector<Deed>::operator=(hand);
   savedVectors1.inPlay    .vector<Deed>::operator=(inPlay);
   savedVectors1.inSideways.vector<Deed>::operator=(inSideways);
   savedVectors1.inTrash   .vector<Deed>::operator=(inTrash);
   savedVectors1.discardPile = discardPile;

   hand.update();
   inPlay.update();
   inSideways.update();
   inTrash.update();
   updateTitle();
}

class DummyPlayer
    :public vector<Deed>
{
    list<Deed> _discards;
    vector<size_t> _crystals;

    Gtk::Window splashWindow;
    Gtk::Label splashLabel;
    Gtk::Frame splashFrame;
    size_t _previousTurnAmount;
public:
	Gtk::Button* gameEndTurnButtonPtr;
    DummyPlayer();
    
    size_t getCardsRemaining() const {return size();}
    size_t getApproximateTurnsRemaining() const {return size() / 3;};

    KnightName selectRandomDummyVersus(KnightName player);

    void playCards(size_t amount) { 
        _previousTurnAmount = 0;
        while (!empty() && amount > 0) {
             _discards.push_back(back());
             pop_back();
             --amount;
             ++_previousTurnAmount;
        }
    }
    void takeTurn() {
		endOfRoundCheck();
        playCards(3);
		playCards(_crystals[_discards.back().getColor()]);
	}

    void undoTurn() {
        for (int i = _previousTurnAmount; i < _previousTurnAmount && !_discards.empty(); ++i) {
            push_back(_discards.back());
            _discards.pop_back();
        }
    }

    void startNewRound(const Deed& advancedAction, const Deed& spell) {
        if (advancedAction.getType() != Deed::ADVANCED_ACTION || spell.getType() != Deed::SPELL) {
            cout << "Wrong cards passed to startNewRound()\n";
            return;
        }
        push_back(advancedAction);
        ++_crystals[spell.getColor()];
        insert(end(), _discards.begin(), _discards.end()); 
        _discards.clear();
        random_shuffle(begin(), end());
    }

    void endOfRoundCheck() { //could assign this to Game window (as transient) at start
        if (!empty()) 
            return;
		gameEndTurnButtonPtr->set_sensitive(false);
        splashWindow.set_skip_taskbar_hint(true);
        splashWindow.show_all();
        splashWindow.add_modal_grab();
        Glib::signal_timeout().connect_once(sigc::mem_fun(splashWindow, &Gtk::Window::remove_modal_grab), 2250);
        Glib::signal_timeout().connect_once(sigc::mem_fun(splashWindow, &Gtk::Window::hide), 2250);
    }

    void finalTurnforPlayerWarning() {
        splashLabel.set_markup("<span font_family = \"Sakkal Majalla\" size=\"30720\">With both cities conquered, this will be your final turn.</span>");
        splashWindow.set_skip_taskbar_hint(true);
        splashWindow.show_all();
        splashWindow.add_modal_grab();
        Glib::signal_timeout().connect_once(sigc::mem_fun(splashWindow, &Gtk::Window::remove_modal_grab), 2250);
        Glib::signal_timeout().connect_once(sigc::mem_fun(splashWindow, &Gtk::Window::hide), 2250);

    }

    void setParentWindow(Gtk::Window& win) {splashWindow.set_transient_for(win);}

};

DummyPlayer::DummyPlayer() {

    push_back(Deed ("Rage",               Deed::RED,   Deed::BASIC_ACTION));
    push_back(Deed ("Rage",               Deed::RED,   Deed::BASIC_ACTION));
    push_back(Deed ("Determination",      Deed::BLUE,  Deed::BASIC_ACTION));
    push_back(Deed ("Swiftness",          Deed::WHITE, Deed::BASIC_ACTION));
    push_back(Deed ("Swiftness",          Deed::WHITE, Deed::BASIC_ACTION));
    push_back(Deed ("March",              Deed::GREEN, Deed::BASIC_ACTION));
    push_back(Deed ("March",              Deed::GREEN, Deed::BASIC_ACTION));
    push_back(Deed ("Stamina",            Deed::BLUE , Deed::BASIC_ACTION));
    push_back(Deed ("Stamina",            Deed::BLUE , Deed::BASIC_ACTION));
    push_back(Deed ("Tranquility",        Deed::GREEN, Deed::BASIC_ACTION));
    push_back(Deed ("Promise",            Deed::WHITE, Deed::BASIC_ACTION));
    push_back(Deed ("Threaten",           Deed::RED,   Deed::BASIC_ACTION));
    push_back(Deed ("Crystallize",        Deed::BLUE,  Deed::BASIC_ACTION));
    push_back(Deed ("Mana Draw",          Deed::WHITE, Deed::BASIC_ACTION));
    push_back(Deed ("Concentration",      Deed::GREEN, Deed::BASIC_ACTION));
    push_back(Deed ("Improvisation",      Deed::RED,   Deed::BASIC_ACTION));

    splashFrame.set_shadow_type(Gtk::SHADOW_OUT);
    splashLabel.set_markup("<span font_family = \"Sakkal Majalla\" size=\"30720\">You have one turn remaining this round.</span>");
    splashLabel.set_alignment(0.5, 0.5);
    splashWindow.set_decorated(false);
    splashFrame.add(splashLabel);
    splashWindow.add(splashFrame);
    splashWindow.set_title("");
    splashWindow.set_icon(Gdk::Pixbuf::create_from_file(RootDirectory + "transparent.png", 80, 80, false));
    splashWindow.set_position(Gtk::WindowPosition::WIN_POS_CENTER_ALWAYS);
    splashWindow.set_border_width(0);
    
}

KnightName DummyPlayer::selectRandomDummyVersus(KnightName player)  {
    size_t n;
    do {
        n = rand() % 4;
    } while (player == n);
    _crystals.resize(crystal::WHITE + 1);
    random_shuffle(begin(), end());
    switch (n) {
    case NOROWAS:
        for (auto i = begin(); i != end(); ++i) {
            if (i->getName() == "Promise") {
                *i = Deed ("Noble Manners", Deed::WHITE, Deed::BASIC_ACTION);
                break;
            }
        }
        _crystals[crystal::WHITE] = 2;
        _crystals[crystal::GREEN] = 1;
        return NOROWAS;
    case GOLDYX:
        for (auto i = begin(); i != end(); ++i) {
            if (i->getName() == "Concentration") {
                *i = Deed ("Will Focus", Deed::GREEN, Deed::BASIC_ACTION);
                break;
            }
        }
        _crystals[crystal::GREEN] = 2;
        _crystals[crystal::BLUE ] = 1;
        return GOLDYX;
    case ARYTHEA:
        for (auto i = begin(); i != end(); ++i) {
            if (i->getName() == "Rage") {
                *i = Deed ("Battle Versatility", Deed::RED, Deed::BASIC_ACTION);
                break;
            }
        }
        _crystals[crystal::RED  ] = 2;
        _crystals[crystal::WHITE] = 1;
        return ARYTHEA;
    case TOVAK:
        for (auto i = begin(); i != end(); ++i) {
            if (i->getName() == "Determination") {
                *i = Deed ("Cold Toughness", Deed::WHITE, Deed::BASIC_ACTION);
                break;
            }
        }
        _crystals[crystal::BLUE] = 2;
        _crystals[crystal::RED ] = 1;
        return TOVAK;
    }
}

class OfferRowButton
    :public Gtk::RadioButton 
{
    Gtk::Image _childImage;
    size_t _slot;
    double _scale;
    //Glib::RefPtr<Gdk::Pixbuf> _selectionBorderPixbuf;

public:
    deque<Deed>* ptr;
    OfferRowButton(size_t slot, double scale);
    
    void update() {
        if (ptr != NULL && _slot < ptr->size()) { 
            _childImage.set(ptr->at(_slot).getScaledChildPixbuf(_scale));
            //if (get_active()) {
            //    Glib::RefPtr<Gdk::Pixbuf> temp = _childImage.get_pixbuf()->copy();
            //    _selectionBorderPixbuf->scale_simple(FULLSIZE_CARD_WIDTH * _scale, FULLSIZE_CARD_HEIGHT * _scale, Gdk::INTERP_HYPER)->
            //    composite(temp, 0, 0, FULLSIZE_CARD_WIDTH * _scale, FULLSIZE_CARD_HEIGHT * _scale, 0, 0, 1, 1, Gdk::INTERP_HYPER, 255);
            //    _childImage.set(temp);
            //}
            show();
        }
        else {
            hide();
        }
    }
    void setScale (double s) {
        _scale = s;
        update();
    }
    void setSlot (size_t s) {
        if (_slot < ptr->size()) 
        _slot = s;
    }
    bool onMouseOver(int, int, bool, const Glib::RefPtr<Gtk::Tooltip>& tooltip) {
        if (ptr == NULL || _slot >= ptr->size()) 
            return false;
        
        tooltip->set_icon(ptr->at(_slot).getChildPixbuf());
        return true;
    }
};

OfferRowButton::OfferRowButton(size_t slot, double scale = 1)
    :_slot(slot), _scale(scale), ptr(NULL)
{
    set_mode(false);
    set_can_focus(false);
    add(_childImage);
    signal_clicked().connect(sigc::mem_fun(*this, &OfferRowButton::update));
    //_selectionBorderPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "selectionBorder.png");
    //_selectionBorderPixbuf = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, FULLSIZE_CARD_WIDTH * _scale, FULLSIZE_CARD_HEIGHT * _scale);
    //_selectionBorderPixbuf->fill(0x00000000);
    set_has_tooltip();
    signal_query_tooltip().connect(sigc::mem_fun(*this, &OfferRowButton::onMouseOver));


}

class OfferRow //be sure to assign all to one radio group
    :public deque<Deed>, public Gtk::HButtonBox
{
    double _scale;
    size_t _targetNumberOfCards;
    bool _monasteryRow;
public:
    deque<Deed>* _drawPilePointer;
    OfferRowButton _button00;
    OfferRowButton _button01;
    OfferRowButton _button02;
    OfferRowButton _button03;
    vector<OfferRowButton*> buttonPtrs;
    
    Gtk::RadioButton _dummyButton;
    Gtk::Button sendToDrawDeckButton;
    Gtk::Button sendToHandButton;
    Gtk::VButtonBox _vButtonBox;

    Gtk::HBox _mainHBox;

    OfferRow(double _scale,  bool isMonasteryRow);

    size_t getTargetNumberOfCards() const {return _targetNumberOfCards;}
    void setTargetNumberOfCards(size_t n) {_targetNumberOfCards = n;}

    void monasteryFound()  {++_targetNumberOfCards;}
    void monasteryBurned() {--_targetNumberOfCards;}

    void addMoreToOffer(size_t numberToAdd) {
        while (numberToAdd > 0 && _drawPilePointer->size() > 0) {
            push_front(_drawPilePointer->back());
            _drawPilePointer->pop_back();
            --numberToAdd;
        }
    }
    
    void setScale(double s) {
        _scale = s;
        for (int i = 0; i < buttonPtrs.size(); ++i)
            buttonPtrs[i]->setScale(_scale);
    }
    double getScale() const {return _scale;}

    void update() { 
        for (int i = 0; i < buttonPtrs.size(); ++i)
            buttonPtrs[i]->update();
        sendToDrawDeckButton.set_sensitive(!empty());
        sendToHandButton    .set_sensitive(!empty());
    }
    void push_back(const Deed& d) {
        deque<Deed>::push_back(d);
        buttonPtrs[size() - 1]->update();
    }
    void pop_back() {
        deque<Deed>::pop_back();
        buttonPtrs[size()]->update();
    }
    void push_front(const Deed& d) {
        deque<Deed>::push_front(d);
        update();
    }
    void pop_front() {
        deque<Deed>::pop_front();
        update();
    }
    void erase(size_t pos) {
        if (pos >= size())
            return;
        deque<Deed>::erase(this->begin() + pos);
        update();
    }

    void replenish() {
        while (size() != _targetNumberOfCards && !_drawPilePointer->empty()) {
            push_front(_drawPilePointer->back());
            _drawPilePointer->pop_back();
        }
        update();
    }

    void startNewRound() {
        if (!empty()) {
            _drawPilePointer->push_front(back());
            pop_back();
        }
        replenish();
    }
    void clearAndStartNewRound() { //for monastery
        while (!empty()) {
            _drawPilePointer->push_front(back());
            pop_back();
        }
        replenish();
    }
    
    void unclick() {
        _dummyButton.set_active(true);
    }

    void sendSelectedToPile(vectorDeedTableOfButtons& v) {
        for (int i = 0; i < buttonPtrs.size(); ++i) {
            if (buttonPtrs[i]->get_active()) {
                v.push_back(at(i));
                erase(i);
                if (!_monasteryRow)
                    replenish();
                update();
                unclick();
                return;
            }
        }
    }
    void sendSelectedToStandardVector(vector<Deed>& v) {
        for (int i = 0; i < buttonPtrs.size(); ++i) {
            if (buttonPtrs[i]->get_active()) {
                v.push_back(at(i));
                erase(i);
                if (!_monasteryRow)
                    replenish();
                update();
                unclick();
                return;
            }
        }
    }

};

OfferRow::OfferRow(double _scale = 1, bool isMonasteryRow = false)
    :_button00(0, _scale),
     _button01(1, _scale),
     _button02(2, _scale),
     _button03(3, _scale),
     _monasteryRow(isMonasteryRow)
{
    buttonPtrs.push_back(&_button00);
    buttonPtrs.push_back(&_button01);
    buttonPtrs.push_back(&_button02);
    buttonPtrs.push_back(&_button03);
    set_layout(Gtk::ButtonBoxStyle::BUTTONBOX_START);
    for (int i = 0; i < buttonPtrs.size(); ++i) {
        pack_start(*buttonPtrs[i], Gtk::PACK_EXPAND_WIDGET, 30); 
        buttonPtrs[i]->ptr = this;
        buttonPtrs[i]->setSlot(i);
        buttonPtrs[i]->setScale(_scale);
        if (i != 0)
            buttonPtrs[i]->set_group(buttonPtrs[0]->get_group());
    }
    
    _dummyButton.set_group(buttonPtrs[0]->get_group());
    sendToDrawDeckButton.set_label("Send to Deed Pile");
    sendToHandButton.set_label("Take into Hand");
    _vButtonBox.pack_start(sendToDrawDeckButton, Gtk::PackOptions::PACK_SHRINK, 10);
    _vButtonBox.pack_start(sendToHandButton, Gtk::PackOptions::PACK_SHRINK, 10);
    _vButtonBox.set_layout(Gtk::ButtonBoxStyle::BUTTONBOX_SPREAD);
    _mainHBox.pack_start(*this);
    _mainHBox.pack_start(_vButtonBox);
    
    show_all();

}

class OfferWindow
    :public Gtk::Window
{
    Gtk::VBox _mainVBox;
    double _scale;
public:
    deque<Deed> artifactDeck; //offers 2, choose 1 -- also another "click" funtion needed on artifacts
    deque<Deed> advActionDeck;
    deque<Deed> spellDeck;
    deque<Unit> silverDeck;
    deque<Unit> goldDeck;
    bool _coreTileRevealed;

    OfferRow advActionOffer;
    OfferRow spellOffer;
    OfferRow monasteryActionOffer;
    deque<Unit> unitOfferDeck; //should have room for five (no, infinity!)
    deque<Deed> artifactOffer;
    DummyPlayer* _dummyPlayerPtr;

    UnitRadioButton unitOfferButton0;
    UnitRadioButton unitOfferButton1;
    UnitRadioButton unitOfferButton2;
    Gtk::RadioButton dummyUnitButton;
    Gtk::HButtonBox unitOfferHButtonBox;
    Gtk::HBox unitOfferMainHBox;
    Gtk::ScrolledWindow unitOfferScrolledWindow; 
    vector<UnitRadioButton*> unitOfferButtonPointers;
    vector<Gtk::Image*> unitPlaceholderImagePointers;

    Gtk::VButtonBox sendToRetinueVBox;
    Gtk::Button addRegularUnitToOfferButton;
    Gtk::Button addEliteUnitToOfferButton;
    Gtk::Button sendToRetinueButton;

    Gtk::Window monasteryActionsWindow;
    Gtk::Button takeMonasteryActionsButton;
    Gtk::Window artifactChoiceWindow;
    
    list<Deed> tempChosenList;

    Gtk::Button takeArtifactButton;

    OfferWindow(double scale);

    void coreTileRevealed() {_coreTileRevealed = true;}

    void refreshUnitOffer();

    void openOfferWindow() {
        if (!get_visible()) {
            show();
            show_all_children();
            advActionOffer.update();
            spellOffer.update();
            updateUnitRow();
            resetFocus();
        }
        else
            hide();
    }

    void openMonasteryWindow() {
        if (!monasteryActionsWindow.get_visible()) {
            monasteryActionsWindow.show();
            monasteryActionsWindow.show_all_children();
            monasteryActionOffer.update();
            resetFocus();
        }
        else
            monasteryActionsWindow.hide();
    }

    void sendRecruitedUnitTo(Retinue& r, bool bonded = false) {
        for (int i = 0; i < unitOfferButtonPointers.size(); ++i) {
            if (unitOfferButtonPointers[i]->get_active()) {
				if (!bonded)
					r.push_back(unitOfferDeck[i]);
				else
					r.addBondedUnit(unitOfferDeck[i]);
                unitOfferDeck.erase(unitOfferDeck.begin() + i);
            }
        }
        delete unitOfferButtonPointers.back();
        unitOfferButtonPointers.pop_back();
        if (unitOfferButtonPointers.size() + unitPlaceholderImagePointers.size() < 3) { 
            unitPlaceholderImagePointers.push_back(new Gtk::Image);
            unitPlaceholderImagePointers.back()->set(Gdk::Pixbuf::create_from_file(RootDirectory + "elite_back.jpg", FULLSIZE_CARD_WIDTH * _scale, FULLSIZE_CARD_HEIGHT * _scale, true));
            unitPlaceholderImagePointers.back()->get_pixbuf()->saturate_and_pixelate(unitPlaceholderImagePointers.back()->get_pixbuf(), 0, 1);
            unitOfferHButtonBox.pack_start(*unitPlaceholderImagePointers.back(), Gtk::PACK_EXPAND_WIDGET, 30);
            unitPlaceholderImagePointers.back()->show_all();
        }
        //if (unitOfferButton0.get_active()) {r.push_back(unitOfferDeck[0]); unitOfferDeck.erase(unitOfferDeck.begin() + 0);}
        resetFocus();
        updateUnitRow();
        
    }

    void updateUnitRow() {
        for (int i = 0; i < unitOfferButtonPointers.size(); ++i) 
            unitOfferButtonPointers[i]->update();
    }

    void resetFocus() {
        dummyUnitButton                  .clicked();
        advActionOffer._dummyButton      .clicked();
        spellOffer._dummyButton          .clicked();
        monasteryActionOffer._dummyButton.clicked();
        dummyUnitButton                  .set_active();
        advActionOffer._dummyButton      .set_active();
        spellOffer._dummyButton          .set_active();
        monasteryActionOffer._dummyButton.set_active();
    }

    void endOfRound() {
       _dummyPlayerPtr->startNewRound(advActionOffer.back(), spellOffer.back());  
        advActionOffer.startNewRound(); 
        advActionDeck.pop_front();     //deleted since it's in dummy player's deck now  
        spellOffer.startNewRound(); 
        refreshUnitOffer();
        monasteryActionOffer.clearAndStartNewRound();
    }

    void createArtifactOffer(size_t number);

    bool onArtifactWindowDeleted(GdkEventAny* e) {
        artifactChoiceWindow.remove();
        while (!artifactOffer.empty()) { 
            artifactDeck.push_front(artifactOffer.back());
            artifactOffer.pop_back();
        }
        Gtk::Main::quit();
        return false;
    }
    
    void takeArtifact(size_t choice) {
        tempChosenList.push_back(artifactOffer[choice]);
        artifactOffer.erase(artifactOffer.begin() + choice);
        Gtk::Main::quit();
    }

    list<Deed> getChosenArtifacts(size_t numberAwarded = 1) {
        tempChosenList.clear();
        list<Deed> result;
        if (artifactDeck.size() < numberAwarded + 1) {
            result.assign(artifactDeck.begin(), artifactDeck.end());
            artifactDeck.clear();
            return result;
        }
        createArtifactOffer(numberAwarded + 1); 
        result.assign(tempChosenList.begin(), tempChosenList.end());
        return result;
    }

    void addToUnitOffer(bool elite = true) {
        deque<Unit>& targetDeck = (elite ? goldDeck : silverDeck);
        if (targetDeck.empty())
            return;
        unitOfferButtonPointers.push_back(new UnitRadioButton(&unitOfferDeck, unitOfferDeck.size(), _scale));
        unitOfferDeck.push_back(targetDeck.back());
        targetDeck.pop_back();
        if (!unitPlaceholderImagePointers.empty() && unitOfferButtonPointers.size() + unitPlaceholderImagePointers.size() >= 3) {
            delete unitPlaceholderImagePointers.back();
            unitPlaceholderImagePointers.pop_back();
        }
        unitOfferButtonPointers.back()->set_group(dummyUnitButton.get_group());
        unitOfferButtonPointers.back()->set_mode(false);
        unitOfferButtonPointers.back()->set_can_focus(false);
        unitOfferHButtonBox.pack_start(*unitOfferButtonPointers.back(), Gtk::PACK_EXPAND_WIDGET, 30);
        unitOfferButtonPointers.back()->signal_clicked().connect(sigc::bind<bool>(sigc::mem_fun(sendToRetinueButton, &Gtk::Button::set_sensitive), true), false);
        unitOfferButtonPointers.back()->update();
    }

};

OfferWindow::OfferWindow(double scale = 1.0)
    :_coreTileRevealed(false), _scale(scale), unitOfferButton0(&unitOfferDeck, 0, _scale), unitOfferButton1(&unitOfferDeck, 1, _scale), unitOfferButton2(&unitOfferDeck, 2, _scale), 
    _dummyPlayerPtr(NULL), monasteryActionOffer(_scale, true)
    
{
    // unitOfferButton3(&unitOfferDeck, 3, _scale), unitOfferButton4(&unitOfferDeck, 4, _scale)
    //
    set_icon(Gdk::Pixbuf::create_from_file(RootDirectory + "offer.png"));
    set_title("Offers");
    set_keep_above();
    set_resizable(false);

    artifactDeck.push_back(Deed ("Amulet of Darkness",      Deed::NO_COLOR, Deed::ARTIFACT));
    artifactDeck.push_back(Deed ("Amulet of Sun",           Deed::NO_COLOR, Deed::ARTIFACT));
    artifactDeck.push_back(Deed ("Banner of Courage",       Deed::NO_COLOR, Deed::ARTIFACT));
    artifactDeck.push_back(Deed ("Banner of Fear",          Deed::NO_COLOR, Deed::ARTIFACT));
    artifactDeck.push_back(Deed ("Banner of Glory",         Deed::NO_COLOR, Deed::ARTIFACT));
    artifactDeck.push_back(Deed ("Banner of Protection",    Deed::NO_COLOR, Deed::ARTIFACT));
    artifactDeck.push_back(Deed ("Book of Wisdom",          Deed::NO_COLOR, Deed::ARTIFACT));
    artifactDeck.push_back(Deed ("Diamond Ring",            Deed::NO_COLOR, Deed::ARTIFACT));
    artifactDeck.push_back(Deed ("Emerald Ring",            Deed::NO_COLOR, Deed::ARTIFACT));
    artifactDeck.push_back(Deed ("Endless Bag of Gold",     Deed::NO_COLOR, Deed::ARTIFACT));
    artifactDeck.push_back(Deed ("Endless Gem Pouch",       Deed::NO_COLOR, Deed::ARTIFACT));
    artifactDeck.push_back(Deed ("Golden Grail",            Deed::NO_COLOR, Deed::ARTIFACT));
    artifactDeck.push_back(Deed ("Horn of Wrath",           Deed::NO_COLOR, Deed::ARTIFACT));
    artifactDeck.push_back(Deed ("Ruby Ring",               Deed::NO_COLOR, Deed::ARTIFACT));
    artifactDeck.push_back(Deed ("Sapphire Ring",           Deed::NO_COLOR, Deed::ARTIFACT));
    artifactDeck.push_back(Deed ("Sword of Justice",        Deed::NO_COLOR, Deed::ARTIFACT));
    random_shuffle(artifactDeck.begin(), artifactDeck.end());    

    advActionDeck.push_back(Deed ("Agility",         Deed::WHITE, Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Ambush",          Deed::GREEN, Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Blood Rage",      Deed::RED,   Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Blood Ritual",    Deed::RED,   Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Crushing Bolt",   Deed::GREEN, Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Crystal Mastery", Deed::BLUE,  Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Decompose",       Deed::RED,   Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Diplomacy",       Deed::WHITE, Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Fire Bolt",       Deed::RED,   Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Frost Bridge",    Deed::BLUE,  Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Heroic Tale",     Deed::WHITE, Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Ice Bolt",        Deed::BLUE,  Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Ice Shield",      Deed::BLUE,  Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Intimidate",      Deed::RED,   Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Into the Heat",   Deed::RED,   Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("In Need",         Deed::WHITE, Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Learning",        Deed::WHITE, Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Magic Talent",    Deed::BLUE,  Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Mana Storm",      Deed::WHITE, Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Maximal Effect",  Deed::RED,   Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Path Finding",    Deed::GREEN, Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Pure Magic",      Deed::BLUE,  Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Refreshing Walk", Deed::GREEN, Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Regeneration",    Deed::GREEN, Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Song of Wind",    Deed::WHITE, Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Steady Tempo",    Deed::BLUE,  Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Swift Bolt",      Deed::WHITE, Deed::ADVANCED_ACTION));
    advActionDeck.push_back(Deed ("Training",        Deed::GREEN, Deed::ADVANCED_ACTION));
    random_shuffle(advActionDeck.begin(), advActionDeck.end());
    
    advActionOffer._drawPilePointer = &advActionDeck;
    advActionOffer.setTargetNumberOfCards(3);
    advActionOffer.setScale(_scale);
    advActionOffer.replenish();
    
    monasteryActionOffer._drawPilePointer = &advActionDeck;
    monasteryActionOffer.setTargetNumberOfCards(0); //need function to check number of monasteries
    monasteryActionOffer.setScale(_scale);
    monasteryActionOffer.replenish();
   
    spellDeck.push_back(Deed ("Burning Shield",      Deed::RED,    Deed::SPELL));
    spellDeck.push_back(Deed ("Call to Arms",        Deed::WHITE,  Deed::SPELL));
    spellDeck.push_back(Deed ("Chill",               Deed::BLUE,   Deed::SPELL));
    spellDeck.push_back(Deed ("Demolish",            Deed::RED,    Deed::SPELL));
    spellDeck.push_back(Deed ("Expose",              Deed::WHITE,  Deed::SPELL));
    spellDeck.push_back(Deed ("Fireball",            Deed::RED,    Deed::SPELL));
    spellDeck.push_back(Deed ("Flame Wall",          Deed::RED,    Deed::SPELL));
    spellDeck.push_back(Deed ("Mana Bolt",           Deed::BLUE,   Deed::SPELL));
    spellDeck.push_back(Deed ("Meditation",          Deed::GREEN,  Deed::SPELL));
    spellDeck.push_back(Deed ("Restoration",         Deed::GREEN,  Deed::SPELL));
    spellDeck.push_back(Deed ("Snowstorm",           Deed::BLUE,   Deed::SPELL));
    spellDeck.push_back(Deed ("Space Bending",       Deed::BLUE,   Deed::SPELL));
    spellDeck.push_back(Deed ("Tremor",              Deed::GREEN,  Deed::SPELL));
    spellDeck.push_back(Deed ("Underground Travel",  Deed::GREEN,  Deed::SPELL));
    spellDeck.push_back(Deed ("Whirlwind",           Deed::WHITE,  Deed::SPELL));
    spellDeck.push_back(Deed ("Wings of Wind",       Deed::WHITE,  Deed::SPELL));
    random_shuffle(spellDeck.begin(), spellDeck.end());

    spellOffer._drawPilePointer = &spellDeck;
    spellOffer.setTargetNumberOfCards(3);
    spellOffer.setScale(_scale);
    spellOffer.replenish();
    
    silverDeck.push_back(Unit("Peasants",         1, 3, 4, Unit::VILLAGE, 0)); 
    silverDeck.push_back(Unit("Peasants",         1, 3, 4, Unit::VILLAGE, 0)); 
    silverDeck.push_back(Unit("Peasants",         1, 3, 4, Unit::VILLAGE, 0)); 
    silverDeck.push_back(Unit("Herbalists",       1, 2, 3, Unit::VILLAGE | Unit::MONASTERY, 0)); 
    silverDeck.push_back(Unit("Herbalists",       1, 2, 3, Unit::VILLAGE | Unit::MONASTERY, 0)); 
    silverDeck.push_back(Unit("Foresters",        1, 0, 0, 0, 0)); 
    silverDeck.push_back(Unit("Foresters",        1, 0, 0, 0, 0)); 
    silverDeck.push_back(Unit("Utem Crossbowmen", 2, 0, 0, 0, 0)); 
    silverDeck.push_back(Unit("Utem Crossbowmen", 2, 0, 0, 0, 0)); 
    silverDeck.push_back(Unit("Utem Guardsmen",   2, 0, 0, 0, 0)); 
    silverDeck.push_back(Unit("Utem Guardsmen",   2, 0, 0, 0, 0)); 
    silverDeck.push_back(Unit("Utem Swordsmen",   2, 0, 0, 0, 0)); 
    silverDeck.push_back(Unit("Utem Swordsmen",   2, 0, 0, 0, 0)); 
    silverDeck.push_back(Unit("Guardian Golems",  2, 0, 0, 0, 0)); 
    silverDeck.push_back(Unit("Guardian Golems",  2, 0, 0, 0, 0)); 
    silverDeck.push_back(Unit("Illusionists",     2, 0, 0, 0, 0)); 
    silverDeck.push_back(Unit("Illusionists",     2, 0, 0, 0, 0)); 
    silverDeck.push_back(Unit("Red Cape Monks",   2, 0, 0, 0, 0)); 
    silverDeck.push_back(Unit("Northern Monks",   2, 0, 0, 0, 0)); 
    silverDeck.push_back(Unit("Savage Monks",     2, 0, 0, 0, 0)); 
        
    goldDeck.push_back(Unit("Fire Golems",      3, 0, 0, 0, 0));
    goldDeck.push_back(Unit("Fire Golems",      3, 0, 0, 0, 0));
    goldDeck.push_back(Unit("Ice Golems",       3, 0, 0, 0, 0));
    goldDeck.push_back(Unit("Ice Golems",       3, 0, 0, 0, 0));
    goldDeck.push_back(Unit("Fire Mages",       3, 0, 0, 0, 0));
    goldDeck.push_back(Unit("Fire Mages",       3, 0, 0, 0, 0));
    goldDeck.push_back(Unit("Ice Mages",        3, 0, 0, 0, 0));
    goldDeck.push_back(Unit("Ice Mages",        3, 0, 0, 0, 0));
    goldDeck.push_back(Unit("Amotep Gunners",   3, 0, 0, 0, 0));
    goldDeck.push_back(Unit("Amotep Gunners",   3, 0, 0, 0, 0));
    goldDeck.push_back(Unit("Amotep Freezers",  3, 0, 0, 0, 0));
    goldDeck.push_back(Unit("Amotep Freezers",  3, 0, 0, 0, 0));
    goldDeck.push_back(Unit("Catapults",        3, 0, 0, 0, 0));
    goldDeck.push_back(Unit("Catapults",        3, 0, 0, 0, 0));
    goldDeck.push_back(Unit("Catapults",        3, 0, 0, 0, 0));
    goldDeck.push_back(Unit("Altem Mages",      4, 0, 0, 0, 0));
    goldDeck.push_back(Unit("Altem Mages",      4, 0, 0, 0, 0));
    goldDeck.push_back(Unit("Altem Guardians",  4, 0, 0, 0, 0));
    goldDeck.push_back(Unit("Altem Guardians",  4, 0, 0, 0, 0));
    goldDeck.push_back(Unit("Altem Guardians",  4, 0, 0, 0, 0));

    random_shuffle(silverDeck.begin(), silverDeck.end());
    random_shuffle(goldDeck.begin(), goldDeck.end());

    addEliteUnitToOfferButton.set_label("Add Elite Unit");
    addRegularUnitToOfferButton.set_label("Add Regular Unit");
    sendToRetinueButton.set_label("Send to Retinue");

    //unitOfferButtonPointers.push_back(&unitOfferButton0);
    //unitOfferButtonPointers.push_back(&unitOfferButton1);
    //unitOfferButtonPointers.push_back(&unitOfferButton2);
    /*
    for (int i = 0; i < unitOfferButtonPointers.size(); ++i) {
        unitOfferButtonPointers[i]->set_group(dummyUnitButton.get_group());
        unitOfferButtonPointers[i]->set_mode(false);
        unitOfferButtonPointers[i]->set_can_focus(false);
        unitOfferButtonPointers[i]->signal_clicked().connect(sigc::bind<bool>(sigc::mem_fun(sendToRetinueButton, &Gtk::Button::set_sensitive), true), false);
    }
    */
    refreshUnitOffer();
    dummyUnitButton.signal_clicked().connect(sigc::bind<bool>(sigc::mem_fun(sendToRetinueButton, &Gtk::Button::set_sensitive), false), false);
    dummyUnitButton.clicked();
    
   
    unitOfferScrolledWindow.add(unitOfferHButtonBox);
    unitOfferScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_NEVER);
    unitOfferScrolledWindow.set_shadow_type(Gtk::SHADOW_OUT);
    unitOfferHButtonBox.set_layout(Gtk::BUTTONBOX_START);
    //unitOfferHButtonBox.pack_start(unitOfferButton0, Gtk::PACK_EXPAND_WIDGET, 30);
    //unitOfferHButtonBox.pack_start(unitOfferButton1, Gtk::PACK_EXPAND_WIDGET, 30);
    //unitOfferHButtonBox.pack_start(unitOfferButton2, Gtk::PACK_EXPAND_WIDGET, 30);
    sendToRetinueVBox.set_layout(Gtk::BUTTONBOX_SPREAD);
    sendToRetinueVBox.pack_start(addEliteUnitToOfferButton,   Gtk::PACK_SHRINK, 10);
    sendToRetinueVBox.pack_start(addRegularUnitToOfferButton, Gtk::PACK_SHRINK, 10);
    sendToRetinueVBox.pack_start(sendToRetinueButton,         Gtk::PACK_SHRINK, 10);



    addEliteUnitToOfferButton.signal_clicked().connect(sigc::bind<bool>(sigc::mem_fun(*this, &OfferWindow::addToUnitOffer), true));
    addRegularUnitToOfferButton.signal_clicked().connect(sigc::bind<bool>(sigc::mem_fun(*this, &OfferWindow::addToUnitOffer), false));

    unitOfferMainHBox.pack_start(unitOfferScrolledWindow, Gtk::PACK_EXPAND_WIDGET);

    //unitOfferHButtonBox.set_homogeneous(false);
    unitOfferMainHBox.pack_start(sendToRetinueVBox, Gtk::PackOptions::PACK_SHRINK);
   // unitOfferMainHBox.set_child_secondary(sendToRetinueVBox); not an option for non-button boxes

    _mainVBox.pack_start(advActionOffer._mainHBox, Gtk::PACK_EXPAND_PADDING, 20);
    _mainVBox.pack_start(spellOffer._mainHBox, Gtk::PACK_EXPAND_PADDING, 20);
    _mainVBox.pack_start(unitOfferMainHBox, Gtk::PACK_EXPAND_PADDING, 20);

    add(_mainVBox);

    monasteryActionsWindow.add(monasteryActionOffer._mainHBox); //could have different formatting -- will have its own window
    monasteryActionsWindow.set_title("Choose your training");
    monasteryActionsWindow.set_position(Gtk::WIN_POS_CENTER_ALWAYS);
    monasteryActionsWindow.set_icon(Gdk::Pixbuf::create_from_file(RootDirectory + "adv_action_learning.jpg"));

    artifactChoiceWindow.set_position(Gtk::WIN_POS_CENTER_ALWAYS);
    artifactChoiceWindow.set_title("Choose among these artifacts");
    artifactChoiceWindow.set_icon(Gdk::Pixbuf::create_from_file(RootDirectory + "artifact_golden_grail.jpg"));
    artifactChoiceWindow.set_modal(true);
    artifactChoiceWindow.signal_delete_event().connect(sigc::mem_fun(*this, &OfferWindow::onArtifactWindowDeleted), false);
    
}

void OfferWindow::createArtifactOffer(size_t number) { 
    if (artifactDeck.empty()) {
        cout << "No cards in artifact deck.\n";
        return;
    }
    artifactChoiceWindow.remove();
    vector<Gtk::Button*> buttonPointers(number);
    vector<Gtk::Image*>  imagePointers(number);
    while (artifactDeck.size() > 0 && number > 0) {
        artifactOffer.push_front(artifactDeck.back());
        artifactDeck.pop_back();
        --number;
    }
    
    Gtk::HButtonBox* _hbox = Gtk::manage(new Gtk::HButtonBox);
    for (size_t upForChoice = 0; upForChoice < artifactOffer.size(); ++upForChoice) {
        buttonPointers[upForChoice] = (Gtk::manage(new Gtk::Button));
        _hbox->pack_start(*buttonPointers[upForChoice]);  
        buttonPointers[upForChoice]->signal_clicked().connect(sigc::bind<size_t> (sigc::mem_fun(*this, &OfferWindow::takeArtifact), upForChoice));
        imagePointers[upForChoice] = Gtk::manage(new Gtk::Image);
        imagePointers[upForChoice]->set((artifactOffer.begin() + upForChoice)->getScaledChildPixbuf(.75));
        buttonPointers[upForChoice]->add(*imagePointers[upForChoice]);
    }
    artifactChoiceWindow.add(*_hbox);
    artifactChoiceWindow.show_all();

    while (artifactOffer.size() > 1) {
        Gtk::Main::run();
        int i = 0;
        for (; i < artifactOffer.size(); ++i) {
            imagePointers[i]->set(artifactOffer[i].getScaledChildPixbuf(.75));
        }
        buttonPointers[i]->hide();
        if (!artifactChoiceWindow.get_visible()) 
            return;
        artifactChoiceWindow.resize(artifactOffer[0].getScaledChildPixbuf(.75)->get_width() * artifactOffer.size(), artifactOffer[0].getScaledChildPixbuf(.75)->get_height());
    }
    artifactChoiceWindow.hide();
    if (artifactOffer.size() > 0) {
        artifactDeck.push_front(artifactOffer.back());
        artifactOffer.pop_back();
    }
}

void OfferWindow::refreshUnitOffer() {
    while(!unitOfferDeck.empty()) {
        if (unitOfferDeck.back().isGold()) {
            goldDeck.push_front(unitOfferDeck.back());
        }
        else if (unitOfferDeck.back().isSilver()) {
            silverDeck.push_front(unitOfferDeck.back());
        }
        unitOfferDeck.pop_back();
    }
    for (int i = 0; i < unitOfferButtonPointers.size(); ++i) {
        delete unitOfferButtonPointers[i];
    }
    for (int i = 0; i < unitPlaceholderImagePointers.size(); ++i) {
        delete unitPlaceholderImagePointers[i];
    }
    unitOfferButtonPointers.clear();
    unitPlaceholderImagePointers.clear();
    addToUnitOffer(_coreTileRevealed);
    addToUnitOffer(false);
    addToUnitOffer(_coreTileRevealed);

    updateUnitRow();

}



/*
If an effect tells you to gain a new Deed card (Advanced Action,
Spell, or Artifact) during or after your turn, the new card is
placed on top of your Deed deck unless stated otherwise.

a. Whenever you gain a card from the Spell offer or Advanced
Action offer, replenish the offer immediately by shifting the
remaining cards down and adding a new card to the top slot
of the offer.

b. If you take a card from the Unit offer (either a Unit or an
Advanced Action that you learned in a monastery), do not
replenish the offer. It will be replenished at the start of the
next Round.
*/



class statReadout
    :public Gtk::SpinButton
{
   
public:
    
    statReadout();

    void setLabel(const string& s) {set_text(s);}
    void increaseBy(int n) {
        set_value(get_value() + n); 
        checkNewValue();
    }
    void decreaseBy(int n) {
        set_value(get_value() - n);
        checkNewValue();
    }
    string getValueAsString() const {
        ostringstream oss;
        oss << get_value_as_int();
        return oss.str();
    }
    void checkNewValue() {
        if (get_value())
            set_visibility(true);
        else
            set_visibility(false);
    }
    bool stopRightClick(GdkEventButton* e) {
        if (e->button == 3) {        
            return true;
        }
        return false;
    }
};

statReadout::statReadout()
{
    Pango::FontDescription myLabelFont;
    Pango::FontDescription myNumberFont;
    myLabelFont.set_family("Sakkal Majalla");
    myNumberFont.set_family("Sakkal Majalla");
    myLabelFont.set_size(PANGO_SCALE * 16);
    myNumberFont.set_size(PANGO_SCALE * 24);
    modify_font(myLabelFont);
    set_can_focus(false);
    modify_font(myNumberFont);
    set_editable(false);
    set_alignment(Gtk::ALIGN_CENTER);
    set_increments(1, 1);
    set_numeric();
    set_range(0, 30);
    
    set_invisible_char(0);
    signal_changed().connect(sigc::mem_fun(*this, &statReadout::checkNewValue));
    signal_button_press_event().connect(sigc::mem_fun(*this, &statReadout::stopRightClick), false);
    set_value(0);
    set_visibility(false);
    set_events(Gdk::EventMask::ALL_EVENTS_MASK);
    set_increments(1, 1);
    //get_adjustment()->configure(0, 0, 30, 1, 2, 0);
    //set_adjustment(adj);
    //pack_start(_stat);
    //pack_start(_minusButton);
    //pack_start(_textBox);
    //set_size_request(40, 48);
    
    show_all();
}

//Sieged
//Ranged (Sieged + Ranged)
//Attack
class TextConsole_OLD
    :public Gtk::TextView
{
    Pango::FontDescription _font;
public:
    TextConsole_OLD();
    void print(const string& s, bool withNewLine = false) {
        get_buffer()->set_text(s + (withNewLine ? "\n" : ""));
    }
    void insert(const string& s, bool withNewLine = false) {
        get_buffer()->insert_at_cursor(s + (withNewLine ? "\n" : ""));
    }
};

TextConsole_OLD::TextConsole_OLD() {
    _font.set_family("Sakkal Majalla");
    _font.set_size(PANGO_SCALE * 20);
    modify_font(_font);
    set_editable(false);
    set_cursor_visible(false);
    set_wrap_mode(Gtk::WrapMode::WRAP_WORD);
    //modify_base(Gtk::STATE_NORMAL, Gdk::Color("#F0F0F0"));
    modify_base(Gtk::STATE_NORMAL, Gdk::Color("#FFFFFF"));
    modify_text(Gtk::STATE_NORMAL, Gdk::Color("#000000"));
    set_justification(Gtk::JUSTIFY_CENTER);
    set_border_width(0);
}

class TextConsole
    :public Gtk::Label
{
    Pango::FontDescription _font;
    
public:
    Gtk::EventBox _eventBox;
    Gtk::HBox _hBox;
    Gtk::Image _placeholderImage;

    TextConsole();
    void print(const string& s) {
        set_text(s);
    }
    //void insert(const string& s, bool withNewLine = false) {
    //    get_buffer()->insert_at_cursor(s + (withNewLine ? "\n" : ""));
    //}
};

TextConsole::TextConsole() {
    _font.set_family("Sakkal Majalla");
    _font.set_size(PANGO_SCALE * 20);
    modify_font(_font);
    
    _placeholderImage.set(Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, 20, 40));
    _placeholderImage.get_pixbuf()->fill(0xF0F0F000);
    set_alignment(0.5, 0.0);
    _hBox.pack_start(_eventBox);
    _hBox.pack_start(_placeholderImage);
    _eventBox.add(*this);
    //modify_base(Gtk::STATE_NORMAL, Gdk::Color("#F0F0F0"));
    _eventBox.modify_bg(Gtk::STATE_NORMAL, Gdk::Color("#000000"));
    modify_text(Gtk::STATE_NORMAL, Gdk::Color("#E1F200"));
    modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#E1F200"));
    modify_text(Gtk::STATE_ACTIVE, Gdk::Color("#E1F200"));
	modify_fg(Gtk::STATE_ACTIVE, Gdk::Color("#E1F200"));
    modify_text(Gtk::STATE_INSENSITIVE, Gdk::Color("#E1F200"));
    modify_fg(Gtk::STATE_INSENSITIVE, Gdk::Color("#E1F200"));
    
	

}

class CombatCalculator
    :public Gtk::Table
{
    Gtk::SpinButton _coldfireSiegeButton;
    Gtk::SpinButton _fireSiegeButton;
    Gtk::SpinButton _iceSiegeButton;
    Gtk::SpinButton _physicalSiegeButton;
    Gtk::TextView   _totalSiegeView;

    Gtk::SpinButton _coldfireRangedButton;
    Gtk::SpinButton _fireRangedButton;
    Gtk::SpinButton _iceRangedButton;
    Gtk::SpinButton _physicalRangedButton;
    Gtk::TextView   _totalRangedView;

    Gtk::SpinButton _coldfireMeleeButton;
    Gtk::SpinButton _fireMeleeButton;
    Gtk::SpinButton _iceMeleeButton;
    Gtk::SpinButton _physicalMeleeButton;
    Gtk::TextView   _totalMeleeView;

    Gtk::TextView   _coldfireTotalView;
    Gtk::TextView   _fireTotalView;
    Gtk::TextView   _iceTotalView;
    Gtk::TextView   _physicalTotalView;
    Gtk::TextView   _totalTotalView;

    Gtk::Label _coldfireLabel;
    Gtk::Label _fireLabel;
    Gtk::Label _iceLabel;
    Gtk::Label _physicalLabel;
    Gtk::Label _siegeLabel;
    Gtk::Label _rangedLabel;
    Gtk::Label _meleeLabel;
    Gtk::Label _totalColumnLabel;
    Gtk::Label _totalRowLabel;

    Gtk::Image _cornerImage;
    Pango::FontDescription _font;
    vector< vector<Gtk::Widget*> > widgets;
    vector< vector<Gtk::SpinButton*> > spinbuttons;
public:
    CombatCalculator();

    void updateValues();

    size_t getSiegeTotal() {
        return
        (_coldfireSiegeButton.get_value_as_int() + 
        _fireSiegeButton.get_value_as_int() + 
        _iceSiegeButton.get_value_as_int() + 
        _physicalSiegeButton.get_value_as_int());
    }

    size_t getRangedTotal() {
        return
        (_coldfireRangedButton.get_value_as_int() + 
         _fireRangedButton.get_value_as_int() + 
         _iceRangedButton.get_value_as_int() + 
         _physicalRangedButton.get_value_as_int());
    }
    size_t getMeleeTotal() {
        return
        (_coldfireMeleeButton.get_value_as_int() + 
         _fireMeleeButton.get_value_as_int() + 
         _iceMeleeButton.get_value_as_int() + 
         _physicalMeleeButton.get_value_as_int());
    }
    size_t getColdfireTotal() {
        return
        (_coldfireSiegeButton.get_value_as_int() + 
         _coldfireRangedButton.get_value_as_int() + 
         _coldfireMeleeButton.get_value_as_int());
    }
    size_t getFireTotal() {
        return
        (_fireSiegeButton.get_value_as_int() + 
         _fireRangedButton.get_value_as_int() + 
         _fireMeleeButton.get_value_as_int());
    }
    size_t getIceTotal() {
        return
        (_iceSiegeButton.get_value_as_int() + 
         _iceRangedButton.get_value_as_int() + 
         _iceMeleeButton.get_value_as_int());
    }
    size_t getPhysicalTotal() {
        return
        (_physicalSiegeButton.get_value_as_int() + 
         _physicalRangedButton.get_value_as_int() + 
         _physicalMeleeButton.get_value_as_int());
    }
    size_t getTotalTotal() {
        if (getSiegeTotal() + getRangedTotal() + getMeleeTotal() ==
           (getColdfireTotal() + getFireTotal() + getIceTotal() + getPhysicalTotal()))
        return getColdfireTotal() + getFireTotal() + getIceTotal() + getPhysicalTotal();
        else
            return 0;
    }
};

CombatCalculator::CombatCalculator() {
    
    
    widgets.resize(5);
    spinbuttons.resize(5);
    for (int i = 0; i < widgets.size(); ++i)
    {
        widgets[i].resize(6);
        spinbuttons[i].resize(6);
    }
    int r = 0; 
    int c = 0;
    widgets[r][c] = &_cornerImage;      spinbuttons[r][c] = NULL;   ++c;                                  
    widgets[r][c] = &_coldfireLabel;    spinbuttons[r][c] = NULL;   ++c;                                  
    widgets[r][c] = &_fireLabel;        spinbuttons[r][c] = NULL;   ++c;                                  
    widgets[r][c] = &_iceLabel;         spinbuttons[r][c] = NULL;   ++c;                                  
    widgets[r][c] = &_physicalLabel;    spinbuttons[r][c] = NULL;   ++c;                                  
    widgets[r][c] = &_totalColumnLabel; spinbuttons[r][c] = NULL;   c = 0; ++r;
                                                                    
    widgets[r][c] = &_siegeLabel;          spinbuttons[r][c] = NULL;                   ++c;                              
    widgets[r][c] = &_coldfireSiegeButton; spinbuttons[r][c] = &_coldfireSiegeButton;  ++c;                      
    widgets[r][c] = &_fireSiegeButton;     spinbuttons[r][c] = &_fireSiegeButton;      ++c;                    
    widgets[r][c] = &_iceSiegeButton;      spinbuttons[r][c] = &_iceSiegeButton;       ++c;                  
    widgets[r][c] = &_physicalSiegeButton; spinbuttons[r][c] = &_physicalSiegeButton;  ++c;
    widgets[r][c] = &_totalSiegeView;      spinbuttons[r][c] = NULL;                   c = 0; ++r; 
                                                                  
    widgets[r][c] = &_rangedLabel;           spinbuttons[r][c] = NULL;  ++c;                    
    widgets[r][c] = &_coldfireRangedButton;  spinbuttons[r][c] = &_coldfireRangedButton; ++c;                           
    widgets[r][c] = &_fireRangedButton;      spinbuttons[r][c] = &_fireRangedButton;     ++c;                            
    widgets[r][c] = &_iceRangedButton;       spinbuttons[r][c] = &_iceRangedButton;      ++c;
    widgets[r][c] = &_physicalRangedButton;  spinbuttons[r][c] = &_physicalRangedButton; ++c;                                 
    widgets[r][c] = &_totalRangedView;       spinbuttons[r][c] = NULL;  c = 0; ++r;                           
                                                                   
    widgets[r][c] = &_meleeLabel;           spinbuttons[r][c] = NULL;    ++c;                              
    widgets[r][c] = &_coldfireMeleeButton;  spinbuttons[r][c] = &_coldfireMeleeButton;  ++c;                           
    widgets[r][c] = &_fireMeleeButton;      spinbuttons[r][c] = &_fireMeleeButton;      ++c;
    widgets[r][c] = &_iceMeleeButton;       spinbuttons[r][c] = &_iceMeleeButton;       ++c;
    widgets[r][c] = &_physicalMeleeButton;  spinbuttons[r][c] = &_physicalMeleeButton;  ++c;
    widgets[r][c] = &_totalMeleeView;       spinbuttons[r][c] = NULL;    c = 0; ++r;
    
    widgets[r][c] = &_totalRowLabel;      spinbuttons[r][c] = NULL;  ++c;
    widgets[r][c] = &_coldfireTotalView;  spinbuttons[r][c] = NULL;  ++c;
    widgets[r][c] = &_fireTotalView;      spinbuttons[r][c] = NULL;  ++c;
    widgets[r][c] = &_iceTotalView;       spinbuttons[r][c] = NULL;  ++c;
    widgets[r][c] = &_physicalTotalView;  spinbuttons[r][c] = NULL;  ++c;
    widgets[r][c] = &_totalTotalView;     spinbuttons[r][c] = NULL;
    _font.set_family("Sakkal Majalla");
    _font.set_size(PANGO_SCALE * 20);

    resize(5, 6);
    set_homogeneous(true);

    for (int i = 0; i < widgets.size(); ++i) {
        for (int j = 0; j < widgets[i].size(); ++j)
        {
            widgets[i][j]->modify_font(_font);
            attach(*widgets[i][j], j, j + 1, i, i + 1);
            if (spinbuttons[i][j] != NULL) {
                spinbuttons[i][j]->set_alignment(Gtk::ALIGN_CENTER);
                spinbuttons[i][j]->set_range(0, 40);
                spinbuttons[i][j]->set_increments(1, 1);
                spinbuttons[i][j]->signal_value_changed().connect(sigc::mem_fun(*this, &CombatCalculator::updateValues));
                spinbuttons[i][j]->set_value(0);
                spinbuttons[i][j]->set_visibility(false);
                spinbuttons[i][j]->set_invisible_char(0);
            }
        }
    }
    _totalSiegeView.set_justification(Gtk::JUSTIFY_CENTER);
    _totalRangedView.set_justification(Gtk::JUSTIFY_CENTER);
    _totalMeleeView.set_justification(Gtk::JUSTIFY_CENTER);
    _coldfireTotalView.set_justification(Gtk::JUSTIFY_CENTER);
    _fireTotalView.set_justification(Gtk::JUSTIFY_CENTER);
    _iceTotalView.set_justification(Gtk::JUSTIFY_CENTER);
    _physicalTotalView.set_justification(Gtk::JUSTIFY_CENTER);
    _totalTotalView.set_justification(Gtk::JUSTIFY_CENTER);

    _totalSiegeView.   modify_base(Gtk::STATE_NORMAL, Gdk::Color("#F0F0F0"));
    _totalRangedView.  modify_base(Gtk::STATE_NORMAL, Gdk::Color("#F0F0F0"));
    _totalMeleeView.   modify_base(Gtk::STATE_NORMAL, Gdk::Color("#F0F0F0"));
    _coldfireTotalView.modify_base(Gtk::STATE_NORMAL, Gdk::Color("#F0F0F0"));
    _fireTotalView.    modify_base(Gtk::STATE_NORMAL, Gdk::Color("#F0F0F0"));
    _iceTotalView.     modify_base(Gtk::STATE_NORMAL, Gdk::Color("#F0F0F0"));
    _physicalTotalView.modify_base(Gtk::STATE_NORMAL, Gdk::Color("#F0F0F0"));
    _totalTotalView.   modify_base(Gtk::STATE_NORMAL, Gdk::Color("#F0F0F0"));

    _totalSiegeView.   set_editable(false);
    _totalRangedView.  set_editable(false);
    _totalMeleeView.   set_editable(false);
    _coldfireTotalView.set_editable(false);
    _fireTotalView.    set_editable(false);
    _iceTotalView.     set_editable(false);
    _physicalTotalView.set_editable(false);
    _totalTotalView.   set_editable(false);

    _totalSiegeView.   set_sensitive(false);
    _totalRangedView.  set_sensitive(false);
    _totalMeleeView.   set_sensitive(false);
    _coldfireTotalView.set_sensitive(false);
    _fireTotalView.    set_sensitive(false);
    _iceTotalView.     set_sensitive(false);
    _physicalTotalView.set_sensitive(false);
    _totalTotalView.   set_sensitive(false);

    _siegeLabel.set_text("Siege");
    _rangedLabel.set_text("Ranged");
    _meleeLabel.set_text("Normal");
    _coldfireLabel.set_text("Coldfire ");
    _fireLabel.set_text("Fire ");
    _iceLabel.set_text("Ice ");
    _physicalLabel.set_text("Physical ");
    _totalRowLabel.set_text("Total");
    _totalColumnLabel.set_text("Total");

     _siegeLabel      .set_alignment(0,0);
     _rangedLabel     .set_alignment(0,0);
     _meleeLabel      .set_alignment(0,0);
     _totalRowLabel   .set_alignment(0,0);
     


    show_all();
}
void CombatCalculator::updateValues() {
    ostringstream oss; 
    oss << getSiegeTotal();
    _totalSiegeView.get_buffer()->set_text(oss.str());
    oss.str("");
    oss << getRangedTotal();
    _totalRangedView.get_buffer()->set_text(oss.str());
    oss.str("");
    oss << getMeleeTotal();
    _totalMeleeView.get_buffer()->set_text(oss.str());
    oss.str("");

    oss << getColdfireTotal();
    _coldfireTotalView.get_buffer()->set_text(oss.str());
    oss.str("");
    oss << getFireTotal();
    _fireTotalView.get_buffer()->set_text(oss.str());
    oss.str("");
    oss << getIceTotal();
    _iceTotalView.get_buffer()->set_text(oss.str());
    oss.str("");
    oss << getPhysicalTotal();
    _physicalTotalView.get_buffer()->set_text(oss.str());
    oss.str("");

    oss << getTotalTotal();
    _totalTotalView.get_buffer()->set_text(oss.str());
    oss.str("");
    
    for (int i = 0; i < spinbuttons.size(); ++i) {
        for (int j = 0; j < spinbuttons[i].size(); ++j) {
            if (spinbuttons[i][j] == NULL)
                continue;
            if (spinbuttons[i][j]->get_value_as_int()) 
                spinbuttons[i][j]->set_visibility(true);
            else
                spinbuttons[i][j]->set_visibility(false);
        }
    }
}

class DayNightIndicator
    :public Gtk::Image
{
    Glib::RefPtr<Gdk::Pixbuf> _daypix;
    Glib::RefPtr<Gdk::Pixbuf> _nightpix;
    bool _day;
public:
    DayNightIndicator()                                                                   //original size 200 x 200
        :_daypix  (Gdk::Pixbuf::create_from_file(RootDirectory + "sun.png") ->scale_simple(150, 150, Gdk::INTERP_HYPER)),
         _nightpix(Gdk::Pixbuf::create_from_file(RootDirectory + "moon.png")->scale_simple(150, 150, Gdk::INTERP_HYPER)),
         _day(true)
    {
        modify_bg(Gtk::STATE_NORMAL, Gdk::Color("#FFFFFF"));
        set(_daypix); 
        set_alignment(0.5, 0.75);
    }
    bool isDay()   const {return  _day;}
    bool isNight() const {return !_day;}
    void flip() {
        _day = !_day; 
        set(_day ? _daypix : _nightpix);
        modify_bg(Gtk::STATE_NORMAL, Gdk::Color(_day ? "#FFFFFF" : "#000000")); //may need event box
    }
};

class superScaler //labels? more "robust"?
    :public Gtk::VScale

{
public:
    Pango::FontDescription _font;
    superScaler() { 
        set_draw_value(true);
        set_value_pos(Gtk::POS_RIGHT);
        _font.set_family("Sakkal Majalla");
        modify_font(_font);  
        set_increments(1, 1);          
        set_inverted();                
        set_digits(0);                 
        set_can_focus(false); 

    }
    bool on_scroll_event(GdkEventScroll *e) {
        if (e->direction == GDK_SCROLL_UP) {
            set_value(get_value() + 1);
            return true;
        }
        if (e->direction == GDK_SCROLL_DOWN) {
            set_value(get_value() - 1);
            return true; 
        }
        else
            return false;
    }
};

class FameBar
    :public Gtk::Frame
{
    typedef pair<size_t, size_t> minmax;
    size_t _level;
    std::map<size_t, minmax> _fameLevels;
    Pango::FontDescription _font;
public:
    superScaler _scale;
    Gtk::Label _advanceLabel;
    Gtk::Button _advanceButton; //can link to retinue / offer / skills windows
    FameBar();
    
    size_t getFame()  { return size_t(_scale.get_value());}
    size_t getLevel() { return _level;}
    void updateLabel() {
        ostringstream oss;
        oss << "Level " << _level << ": " << _scale.get_value() << "/" << _fameLevels[_level].second + 1;
        //_advanceLabel.set_use_markup(true);
        //_advanceLabel.set_markup("<span font_family = \"Sakkal Majalla\" size=\"12288\" background=\"#FBCC0D\">" + oss.str() + "</span>");
        _advanceLabel.set_text(oss.str());
    }
    void on_value_increase() {
        updateLabel();
        if (_level == 10)
            return; 
        _advanceButton.set_sensitive (_scale.get_value() == _fameLevels[_level].second + 1);
    }
    void levelUp() {
        ++_level;
        _scale.set_range(_fameLevels[_level].first, _fameLevels[_level].second + 1);
        _scale.set_value(_fameLevels[_level].first);
        _advanceButton.set_sensitive(false);
        updateLabel();
    }
    

};

FameBar::FameBar()
    :_level(1)
{
    _scale._font.set_size(PANGO_SCALE * 16);
    _advanceLabel.modify_font(_scale._font);
    _advanceButton.set_relief(Gtk::ReliefStyle::RELIEF_HALF);
    _advanceButton.add(_advanceLabel);
    _advanceButton.set_sensitive(false);
    add(_scale);
    _fameLevels[1] = minmax(0,  2);
    _fameLevels[2] = minmax(3,  7);
    _fameLevels[3] = minmax(8,  14);
    _fameLevels[4] = minmax(15, 23);
    _fameLevels[5] = minmax(24, 34);
    _fameLevels[6] = minmax(35, 47);
    _fameLevels[7] = minmax(48, 62);
    _fameLevels[8] = minmax(63, 79);
    _fameLevels[9] = minmax(80, 98);
    _fameLevels[10] = minmax(99, 500);
    _scale.set_range(_fameLevels[_level].first, _fameLevels[_level].second + 1);
    _scale.set_value(_fameLevels[_level].first);
    set_label_align(0.0, 0.0);
    set_shadow_type(Gtk::SHADOW_NONE);
    set_label_widget(_advanceButton);
    updateLabel();
    _scale.signal_value_changed().connect(sigc::mem_fun(*this, &FameBar::on_value_increase));
    _advanceButton.signal_clicked().connect(sigc::mem_fun(*this, &FameBar::levelUp));
    show_all();
}

class ReputationBar
    :public Gtk::Frame
{
   
    std::map<int, string> _influenceBonuses;
    Gtk::Label _label;
public:
    superScaler _scale;
    ReputationBar();

    void updateLabel() {
        _label.set_text("Influence Bonus: " + _influenceBonuses[int(_scale.get_value())]);
    }
};

ReputationBar::ReputationBar() 
{
    add(_scale);
    _scale._font.set_size(PANGO_SCALE * 16);
    _label.modify_font(_scale._font);
    
    set_label_widget(_label);
    set_label_align(0, 0);
    set_shadow_type(Gtk::SHADOW_NONE);
    _scale.set_range(-7, 7);
    _influenceBonuses.insert(pair<int, string>(-7, " X"));
    _influenceBonuses.insert(pair<int, string>(-6, "-5"));
    _influenceBonuses.insert(pair<int, string>(-5, "-3"));
    _influenceBonuses.insert(pair<int, string>(-4, "-2"));
    _influenceBonuses.insert(pair<int, string>(-3, "-1"));
    _influenceBonuses.insert(pair<int, string>(-2, "-1"));
    _influenceBonuses.insert(pair<int, string>(-1, " 0"));
    _influenceBonuses.insert(pair<int, string>( 0, " 0"));
    _influenceBonuses.insert(pair<int, string>( 1, " 0"));
    _influenceBonuses.insert(pair<int, string>( 2, " 1"));
    _influenceBonuses.insert(pair<int, string>( 3, " 1"));
    _influenceBonuses.insert(pair<int, string>( 4, " 2"));
    _influenceBonuses.insert(pair<int, string>( 5, " 2"));
    _influenceBonuses.insert(pair<int, string>( 6, " 3"));
    _influenceBonuses.insert(pair<int, string>( 7, " 5"));
    _scale.set_value(0);
    for (auto i = _influenceBonuses.begin(); i != _influenceBonuses.end(); ++i) {
        _scale.add_mark(i->first, Gtk::PositionType::POS_RIGHT, "");
    }
    updateLabel();
    _scale.signal_value_changed().connect(sigc::mem_fun(*this, &ReputationBar::updateLabel));
    _scale.set_draw_value(false);

}

class Tactic
    :public Gtk::RadioButton
{
    Gtk::Image _childImage;
    bool _faceUp;
public:
    Glib::RefPtr<Gdk::Pixbuf> _childPixbuf;
    Glib::RefPtr<Gdk::Pixbuf> _facedownPixbuf;
    
    Tactic(bool isDay);

    bool isFaceUp() const {return _faceUp;}

    void flipUp() {
        _faceUp = true;
        _childImage.set(_childPixbuf);
    }
    void flipDown() {
        _faceUp = false;
        _childImage.set(_facedownPixbuf);
    }
   
    bool showBigPixbuf (int x, int y, bool ,const Glib::RefPtr<Gtk::Tooltip>& tooltip) {
        tooltip->set_icon(_childPixbuf);
        return true;
    }
};

Tactic::Tactic(bool isDay) 
    : _faceUp(false)
{
    int w = 250;
    int h = 350;
    double scale = .8;
    add(_childImage);
    if (isDay)
        _facedownPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "tactics_day_back.jpg",   w * scale,  h * scale, true);
    else
        _facedownPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "tactics_night_back.jpg", w * scale,  h * scale, true);
    show_all();
    set_mode(false);
    //set_has_tooltip(true);
    //signal_query_tooltip().connect(sigc::mem_fun(*this, &Tactic::showBigPixbuf));
}

class TacticWindow
    :public Gtk::Window
{
    Tactic day1;
    Tactic day2;
    Tactic day3;
    Tactic day4;
    Tactic day5;
    Tactic day6;
    Tactic night1;
    Tactic night2;
    Tactic night3;
    Tactic night4;
    Tactic night5;
    Tactic night6;
    Gtk::RadioButton _dummyButton;
    Gtk::Button _selectButton;
    Gtk::HBox _dayBox;
    Gtk::HBox _nightBox;
    vector<Tactic*> dayVector;
    vector<Tactic*> nightVector;
    Gtk::Table _table;
    bool _dummyFirst;
	size_t _currentTactic;
	bool _currentTacticFaceUp;

public:
    Gtk::Image currentTacticImage;

    TacticWindow();
    void onSelection();
    size_t dummyPicksDay();
    size_t dummyPicksNight();
	size_t getCurrentTactic() const {return _currentTactic;}

    bool dummyGoesFirst() const { return _dummyFirst; }

    void offerDayTactic() {
        show_all();
        for (int i = 0; i < dayVector.size(); ++i) {
            dayVector[i]->set_sensitive(dayVector[i]->isFaceUp());
            nightVector[i]->set_sensitive(false);
            _dummyButton.set_active();
        }
        Gtk::Main::run();
    }

    void offerNightTactic() {
        show_all();
        for (int i = 0; i < nightVector.size(); ++i) {
            dayVector[i]->set_sensitive(false);
            nightVector[i]->set_sensitive(nightVector[i]->isFaceUp());
            _dummyButton.set_active();
        }
        Gtk::Main::run();
    }

    void freeChoice(size_t choice) {
        dayVector[choice]->set_active(true);
        onSelection();
    }

    void onClick() {
        if (_dummyButton.get_active()) {
            _selectButton.set_sensitive(false);
            return;
        }
        for (int i = 0; i < dayVector.size(); ++i) {
            if (dayVector[i]->get_active()) {
                _selectButton.set_sensitive(dayVector[i]->isFaceUp());
                break;
            }
            if (nightVector[i]->get_active()) {
                _selectButton.set_sensitive(nightVector[i]->isFaceUp());
                break;
            }
        }
    }

	void flipCurrentTacticDown() {
		currentTacticImage.set(Gdk::Pixbuf::create_from_file(RootDirectory + "tactics_night_back.jpg", 171,  239, true));
		_currentTacticFaceUp = false;
	}

	bool currentTacticIsFaceUp() const {return _currentTacticFaceUp;}

};

TacticWindow::TacticWindow() 
    :day1(true), day2(true), day3(true), day4(true), day5(true), day6(true),
     night1(false), night2(false), night3(false), night4(false), night5(false), night6(false),
	 _dummyFirst(false), _currentTacticFaceUp(true)
{
    int w = 250;
    int h = 350;
    double scale = .8;
	set_position(Gtk::WIN_POS_CENTER_ALWAYS);
    dayVector.push_back(&day1);  nightVector.push_back(&night1);
    dayVector.push_back(&day2);  nightVector.push_back(&night2);
    dayVector.push_back(&day3);  nightVector.push_back(&night3);
    dayVector.push_back(&day4);  nightVector.push_back(&night4);
    dayVector.push_back(&day5);  nightVector.push_back(&night5);
    dayVector.push_back(&day6);  nightVector.push_back(&night6);
    day1._childPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "tactics_day (1).jpg",   w * scale,  h * scale, true);     
    day2._childPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "tactics_day (2).jpg",   w * scale,  h * scale, true);     
    day3._childPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "tactics_day (3).jpg",   w * scale,  h * scale, true);     
    day4._childPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "tactics_day (4).jpg",   w * scale,  h * scale, true);     
    day5._childPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "tactics_day (5).jpg",   w * scale,  h * scale, true);     
    day6._childPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "tactics_day (6).jpg",   w * scale,  h * scale, true);     
    night1._childPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "tactics_night_1.jpg", w * scale,  h * scale, true);   
    night2._childPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "tactics_night_2.jpg", w * scale,  h * scale, true);   
    night3._childPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "tactics_night_3.jpg", w * scale,  h * scale, true);   
    night4._childPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "tactics_night_4.jpg", w * scale,  h * scale, true);   
    night5._childPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "tactics_night_5.jpg", w * scale,  h * scale, true);   
    night6._childPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "tactics_night_6.jpg", w * scale,  h * scale, true);   

  
    for (int i = 0; i < dayVector.size(); ++i) {
        dayVector[i]->flipUp(); 
        nightVector[i]->flipUp(); 
        dayVector[i]->set_group(_dummyButton.get_group());
        nightVector[i]->set_group(_dummyButton.get_group());
        dayVector[i]->signal_clicked().connect(sigc::mem_fun(*this, &TacticWindow::onClick));
        nightVector[i]->signal_clicked().connect(sigc::mem_fun(*this, &TacticWindow::onClick));
        _dayBox.pack_start(*dayVector[i], Gtk::PACK_SHRINK);
        _nightBox.pack_start(*nightVector[i], Gtk::PACK_SHRINK);
    }
    
    _dummyButton.signal_clicked().connect(sigc::mem_fun(*this, &TacticWindow::onClick));
    _dummyButton.clicked();
    _selectButton.set_label("Select Tactic");
    _selectButton.signal_clicked().connect(sigc::mem_fun(*this, &TacticWindow::onSelection));
	_selectButton.set_size_request(410, 45);
    _table.resize(3, 6);
    _table.attach(_dayBox,   0, 6, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
    _table.attach(_nightBox, 0, 6, 1, 2, Gtk::SHRINK, Gtk::SHRINK, 0, 20);
    _table.attach(_selectButton, 2, 4, 2, 3, Gtk::SHRINK, Gtk::SHRINK, 0, 20);
    add(_table);
    set_title("Tactics");
    set_icon(Gdk::Pixbuf::create_from_file(RootDirectory + "splitTactic.png"));
	currentTacticImage.set(day1._facedownPixbuf->scale_simple(171,  239, Gdk::INTERP_HYPER));
    set_modal(true);
    set_deletable(false);

    
}


void TacticWindow::onSelection() {
    int i = 0;
    for ( ; i < dayVector.size(); ++i) {
        if (dayVector[i]->get_active() && dayVector[i]->isFaceUp()) {
            currentTacticImage.set(dayVector[i]->_childPixbuf->scale_simple(171, 239, Gdk::INTERP_HYPER));  //scale to current image size?
			_currentTactic = i + 1;
			dayVector[i]->flipDown();
            _dummyFirst = i > dummyPicksDay(); 
            break;
        }
        if (nightVector[i]->get_active() && nightVector[i]->isFaceUp()) {
            currentTacticImage.set(nightVector[i]->_childPixbuf->scale_simple(171, 239, Gdk::INTERP_HYPER));//scale to current image size?
            nightVector[i]->flipDown();
			_currentTactic = i + 1;
			_dummyFirst = i > dummyPicksNight();
            break;
        }
    }
	_currentTacticFaceUp = true;
    hide();
    Gtk::Main::quit();
}


size_t TacticWindow::dummyPicksDay() {
    size_t r;
    do {
        r = rand() % dayVector.size();
    } while (!dayVector[r]->isFaceUp());
    dayVector[r]->flipDown(); 
    return r;
}

size_t TacticWindow::dummyPicksNight() {
    size_t r;
    do {
        r = rand() % nightVector.size();
    } while (!nightVector[r]->isFaceUp());
    nightVector[r]->flipDown();
    return r;
}

class EnemyButton
	:public Gtk::ToggleButton 
{
	Gtk::Image _childImage;

public:
	Gtk::SpinButton blockPhysicalSpinButton;
	Gtk::SpinButton blockFireSpinButton;
	Gtk::SpinButton blockIceSpinButton;
	Gtk::SpinButton blockColdFireSpinButton;
	EnemyButton() {
		add(_childImage);
		set_relief(Gtk::RELIEF_NONE);
        set_can_focus(false);
	}
public:
	void setImage() {

	}
};


class EnemyViewer
    :public Gtk::Window
{
    //Gtk::HButtonBox _HBox;
    Gtk::ToggleButton button00;
    Gtk::ToggleButton button01;
    Gtk::ToggleButton button02;
    Gtk::ToggleButton button03;
    Gtk::ToggleButton button04;
    vector<Gtk::ToggleButton*> _toggleButtonPointer;
    Glib::RefPtr<Gdk::Pixbuf> _crossOutPixbuf;

    Gtk::Image enemyImage00;
    Gtk::Image enemyImage01;
    Gtk::Image enemyImage02;
    Gtk::Image enemyImage03;
    Gtk::Image enemyImage04;
    vector<Gtk::Image*> _imagePointer;
    
    Gtk::VButtonBox _VButtonBox;
    
    Gtk::Button cancelButton;

    Gtk::Window _calculatorWindow;
    CombatCalculator _combatCalculator;
    Gtk::ToggleButton _calculatorShowToggle;

	Gtk::Button _summonButton;
	

    Gtk::HBox _mainHBox;
    Gtk::Table _mainTable;
    vector<Enemy> _childVector;

public:
	sigc::signal<void, size_t> fameTotalSignal;

    EnemyViewer();
    Gtk::Button confirmButton; //public for signal connection
    Gtk::Button revealAllButton;
    void setViewer(const vector<Enemy>& v) {
        _childVector = v;
        setAllInactive();
        updateAll();
    }
    void setViewer(const Space& s) {
        _childVector = s.getEnemies();
        setAllInactive();
        updateAll();
    }

    void updateAll() {
        for (int i = 0; i < _imagePointer.size(); ++i) {
            if (i >= _childVector.size()) {
                _imagePointer[i]->clear();
                _toggleButtonPointer[i]->set_sensitive(false);
            }
            else {
                _toggleButtonPointer[i]->set_sensitive(true);
                _imagePointer[i]->set(_childVector[i].getChildPixbuf());
                if (_toggleButtonPointer[i]->get_active()) {
                    double offset = .1464 * _childVector[i].getChildPixbuf()->get_width();
                    _crossOutPixbuf->composite(_imagePointer[i]->get_pixbuf(), offset, offset, _crossOutPixbuf->get_width(), _crossOutPixbuf->get_height(), offset, offset, 1, 1, Gdk::INTERP_HYPER, 255);
                    _imagePointer[i]->set(_imagePointer[i]->get_pixbuf());
                }
            }
        }
    }

    void setAllInactive() {
        for (int i = 0; i < _toggleButtonPointer.size(); ++i) {
            _toggleButtonPointer[i]->set_active(false);
        }
        updateAll();
    }

    void onOpenCalculator() {
        if (_calculatorShowToggle.get_active()) {
            _calculatorWindow.show_all();
            _calculatorShowToggle.set_label("Hide Calculator");
        }
        else {
            _calculatorWindow.hide();
            _calculatorShowToggle.set_label("Show Calculator");
        }
    }

    void killSelected() {
		size_t fameTotal = 0;
        if (_childVector.empty()) 
            return;
        for (int i = _toggleButtonPointer.size() - 1; i >= 0; --i) {//start from back so erasing doesn't mess up indexing
            if (_toggleButtonPointer[i]->get_active()) {
				fameTotal += _childVector[i].getFame(); //TODO: if not summoned;
                _childVector.erase(_childVector.begin() + i);
			}
        }
		fameTotalSignal.emit(fameTotal);
        setAllInactive();
        updateAll();
    }

    void onSelectEnemyToggle(size_t slot) {
        if (slot >= _childVector.size()) {
            _imagePointer[slot]->clear();
            _toggleButtonPointer[slot]->set_sensitive(false);
            return;
        }
        _toggleButtonPointer[slot]->set_sensitive(true);
        if (!_toggleButtonPointer[slot]->get_active()) {
            _imagePointer[slot]->set(_childVector[slot].getChildPixbuf());
        }
        else {
            double offset = .1464 * _childVector[slot].getChildPixbuf()->get_width();
            _crossOutPixbuf->composite(_imagePointer[slot]->get_pixbuf(), offset, offset,  _crossOutPixbuf->get_width(), _crossOutPixbuf->get_height(), offset, offset, 1, 1, Gdk::INTERP_HYPER, 255);
            _imagePointer[slot]->set(_imagePointer[slot]->get_pixbuf());
        }
    }

    const vector<Enemy>& getVector() const {return _childVector;}

	 
};

EnemyViewer::EnemyViewer() 
    :_crossOutPixbuf(Gdk::Pixbuf::create_from_file(RootDirectory + "Xkill.png"))
{
    set_title("Choose enemies to discard");
    set_icon(Gdk::Pixbuf::create_from_file(RootDirectory + "enemy_marauding_orcs_back.png"));
    set_deletable(false);
    set_keep_above(true);
    move(0,0);
    _calculatorShowToggle.set_label("Show calculator");
    confirmButton.set_label("Confirm");
    cancelButton.set_label("Cancel");
    revealAllButton.set_label("Reveal all enemies");
    
	
    _VButtonBox.set_layout(Gtk::ButtonBoxStyle::BUTTONBOX_EDGE);
    
    _VButtonBox.pack_start(confirmButton);
    _VButtonBox.pack_start(revealAllButton);
    _VButtonBox.set_spacing(confirmButton.get_allocation().get_height()); 
    _VButtonBox.pack_start(cancelButton);
    _VButtonBox.pack_start(_calculatorShowToggle);
    _VButtonBox.set_child_secondary(_calculatorShowToggle);
    
    _toggleButtonPointer.push_back(&button00);
    _toggleButtonPointer.push_back(&button01);
    _toggleButtonPointer.push_back(&button02);
    _toggleButtonPointer.push_back(&button03);
    _toggleButtonPointer.push_back(&button04);

    _imagePointer.push_back(&enemyImage00);
    _imagePointer.push_back(&enemyImage01);
    _imagePointer.push_back(&enemyImage02);
    _imagePointer.push_back(&enemyImage03);
    _imagePointer.push_back(&enemyImage04);

    _mainTable.resize(2, 3);
    _mainTable.set_homogeneous(true);

    for (int i = 0; i < _toggleButtonPointer.size(); ++i) {
        _toggleButtonPointer[i]->add(*_imagePointer[i]);
       // _HBox.pack_start(*_toggleButtonPointer[i]);
        _toggleButtonPointer[i]->set_relief(Gtk::RELIEF_NONE);
        _toggleButtonPointer[i]->signal_toggled().connect(sigc::bind<size_t>(sigc::mem_fun(*this, &EnemyViewer::onSelectEnemyToggle), i));
        _toggleButtonPointer[i]->set_can_focus(false);
    }
    _mainTable.attach(button00,    2, 3, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
    _mainTable.attach(button01,    1, 2, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
    _mainTable.attach(button02,    0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
    _mainTable.attach(button03,    1, 2, 1, 2, Gtk::SHRINK, Gtk::SHRINK);
    _mainTable.attach(button04,    0, 1, 1, 2, Gtk::SHRINK, Gtk::SHRINK);
    _mainTable.attach(_VButtonBox, 2, 3, 1, 2, Gtk::SHRINK, Gtk::SHRINK);

   // _mainBox.pack_start(_HBox);
   // _mainBox.pack_start(_VButtonBox);

    _calculatorShowToggle.signal_toggled().connect(sigc::mem_fun(*this, &EnemyViewer::onOpenCalculator));
    confirmButton.signal_clicked().connect(sigc::mem_fun(*this, &EnemyViewer::killSelected), false);
    confirmButton.signal_clicked().connect(sigc::mem_fun(*this, &EnemyViewer::hide), false);

    cancelButton.signal_clicked().connect(sigc::mem_fun(*this,  &EnemyViewer::setAllInactive), false);
    cancelButton.signal_clicked().connect(sigc::mem_fun(_childVector, &vector<Enemy>::clear), false);
    cancelButton.signal_clicked().connect(sigc::mem_fun(*this,  &EnemyViewer::hide), false);
    
    add(_mainTable);
    updateAll();

    _calculatorWindow.add(_combatCalculator);
    _calculatorWindow.set_title("Combat Calculator");
    _calculatorWindow.set_transient_for(*this);
    _calculatorWindow.set_icon(Gdk::Pixbuf::create_from_file(RootDirectory + "iconColdFire.png"));
    
}


class Game
    :public Gtk::Window
{
    Coordinate origin;
    DummyPlayer _dummyPlayer;
    size_t _roundNumber;

    Pango::FontDescription _font;
    Gtk::Table _mainTable;
 
    Glib::RefPtr<Gdk::Pixbuf> backgroundPixbuf; //all black
    Glib::RefPtr<Gdk::Pixbuf> screen; // the actual pixbuf (all composites) on screen
    Gtk::Image screenImage;
    Gtk::EventBox screenEventBox;
    Gtk::ScrolledWindow mainScrollWindow;
    Coordinate rightClickedHex; //last hex menu popped up from -- passed as parameter to many functions

    EnemyViewer _enemyViewer;
    Retinue _retinue;
    
    Board _board; //bigmap,etc
    std::map<Coordinate, Tile> _tilemap;
    Glib::RefPtr<Gdk::Pixbuf> playerOnePixbuf; 
    Glib::RefPtr<Gdk::Pixbuf> playerOneShieldPixbuf; 

    TacticWindow _tacticWindow;

    Gtk::HBox _statHbox;
    Gtk::VBox _statLabelVBox;
    Gtk::Label _moveLabel;
    Gtk::Label _influenceLabel;
    Gtk::Label _healLabel;
    Gtk::VBox _statReadoutBox;
    statReadout _movePoints;
    statReadout _influencePoints;
    statReadout _healPoints;

    Gtk::Frame _crystalFrame;
    Gtk::Label _crystalLabel;
    Gtk::Table _crystalTable;

    CrystalIndicator blueCrystals;
    CrystalIndicator redCrystals;
    CrystalIndicator greenCrystals;
    CrystalIndicator whiteCrystals;
    
    ManaTokenIndicator blackMana;
    ManaTokenIndicator goldMana;
    ManaTokenIndicator blueMana;
    ManaTokenIndicator redMana;
    ManaTokenIndicator greenMana;
    ManaTokenIndicator whiteMana;

    Gtk::HBox _manaTokenBoxTop;
    Gtk::HBox _manaTokenBoxBottom;
    Gtk::VBox _manaTokenBoxAll;
    Gtk::HBox _crystalManaHBox;

    Source _source;
    DayNightIndicator _dayNightImage;
    Gtk::VBox _sourceDayBox;
	Gtk::EventBox _tacticEventBox;

	Glib::RefPtr<Gtk::ActionGroup> _tacticActionGroup;
    Glib::RefPtr<Gtk::UIManager> _tacticUIManager;
    Gtk::Menu* _tacticMenuPopup;
    
    Gtk::Table _upperRightTable;

    FameBar _fame;
    ReputationBar _reputation;
    Gtk::HBox _sliderHBox;
    Gtk::Table _sliderArmorTable;
    Gtk::Label _armorLabel;
    Gtk::Label _handsizeLabel;
    string _armorMarkupString;
    string _handsizeMarkupString;

    Gtk::VButtonBox _vbuttonBox1;
    
    Gtk::Button _endTurnButton;
    Gtk::Button _showHandButton;
    Gtk::Button _showOfferButton;
    Gtk::Button _endRoundButton;
    Gtk::Button _awardArtifactButton;
    Gtk::Button _awardSkillButton;
    sigc::connection _endTurnSignal;
    
    SkillTokenDeckWindow _skillTokenDeck;
	
    Tile startA;
    Tile countryside01;
    Tile countryside02;
    Tile countryside03;
    Tile countryside04;
    Tile countryside05;
    Tile countryside06;
    Tile countryside07;
    Tile countryside08;
    Tile countryside09;
    Tile countryside10;
    Tile countryside11;
    Tile core01;
    Tile core02;
    Tile core03;
    Tile core04;
    Tile core05;
    Tile core06;
    Tile core07;
    Tile core08;

    vector<Tile> countrysideTileDeck;
    vector<Tile> coreTileDeck;
    vector<Tile> tileDrawDeck;
                                
    HandViewer _handViewer;
    OfferWindow _offerWindow;
	
    Gtk::VBox _offerVBox;
    Gtk::ScrolledWindow _offerScrolledWindow;

	Glib::RefPtr<Gtk::ActionGroup> _handActionGroup;
    Glib::RefPtr<Gtk::UIManager> _handUIManager;
    Gtk::Menu* _handMenuPopup;
        
    EnemyDecks _enemyDecks;
    RuinDeck _ruinDeck;
	    
    TextConsole _console;
    Gtk::ScrolledWindow _consolewindow;

    Glib::RefPtr<Gtk::ActionGroup> _actionGroup;
    Glib::RefPtr<Gtk::UIManager> _UIManager;
    Gtk::Menu* _menuPopup;

    InfoWindow _infoWindow;

    Gtk::ToggleButton _combatViewerToggle; //may just attach solely to right-click menu

    std::map<Space::Feature, std::map<size_t, list<Enemy::Type> > > cityEnemyGuides;

	Glib::RefPtr<Gtk::ActionGroup> testActionGroup;
	Glib::RefPtr<Gtk::UIManager> testUIManager;
	Gtk::MenuBar* testMenuBar;
	Gtk::Menu*   testMenu;
	Gtk::VBox mainMainOuterBox;
   
public:
    void testRedraw() {
        //_upperRightTable.hide();
        _upperRightTable.set_border_width(_upperRightTable.get_border_width() + 5);
        if (_upperRightTable.get_border_width() > 50)
            _upperRightTable.set_border_width(0);
      //  _upperRightTable.show_all();
    }
    Game(); 

    bool showKey(GdkEventKey *e) { 
        cout << hex << e->keyval << endl;
        return true;
    }
   
    unsigned int getMovementCost(Space s);

    bool isDay()   { return _dayNightImage.isDay();}
    bool isNight() { return _dayNightImage.isNight();}

    bool showSpaceInfoTooltip(int, int, bool, const Glib::RefPtr<Gtk::Tooltip>& tooltip);
    bool restoreTooltip()                 { screenEventBox.set_has_tooltip(true); return true;}
    bool removeTooltip(GdkEventMotion* e) { screenEventBox.set_has_tooltip(false); return true;}

    bool hasSkill(SkillName n) {
        return (_skillTokenDeck._playerSkillNames.find(n) != _skillTokenDeck._playerSkillNames.end());
    }

    //can just tweak these to output to debug console

    bool convertToValidHex (const Coordinate& _clicked, Coordinate& _storedCoordinate) {
        for (Board::iterator i = _board.begin(); i != _board.end(); ++i) {
                if (_clicked.isWithinHexAt(i->first)) {
                    _storedCoordinate = i->first;
                    return true;
                }
        }
        return false;
    }

    bool movePlayer(GdkEventButton* e);

	void renderTilesOnGDKScreen()
	{
		for (auto t = _tilemap.begin(); t != _tilemap.end(); ++t) {
			screenEventBox.get_window()->draw_pixbuf(t->second.getScaledChildPixbuf(), 0, 0, t->first.x, t->first.y, t->second.getScaledChildPixbuf()->get_width(), t->second.getScaledChildPixbuf()->get_height(), Gdk::RGB_DITHER_MAX, 0, 0);

			Coordinate targetHex = _board.centerHexOfTileAnchor(t->first);

			vector<Coordinate> coordinatePtr;
			coordinatePtr.push_back(targetHex);
			coordinatePtr.push_back(_board.northWestOf(targetHex));
			coordinatePtr.push_back(_board.northEastOf(targetHex));
			coordinatePtr.push_back(_board.southWestOf(targetHex));
			coordinatePtr.push_back(_board.southEastOf(targetHex));
			coordinatePtr.push_back(_board.eastOf(targetHex)     );
			coordinatePtr.push_back(_board.westOf(targetHex)     );


			for (auto i = coordinatePtr.begin(); i != coordinatePtr.end(); ++i) {
				if (_board[*i].hasRuin()) { 
					screenEventBox.get_window()->draw_pixbuf(_board[*i].getRuin().getChildPixbuf(), 0, 0, i->ruinAnchor().x, i->ruinAnchor().y, RUIN_WIDTH, RUIN_HEIGHT, Gdk::RGB_DITHER_MAX, 0, 0);
				}
				if (!_board[*i]._enemies.empty()) {
					screenEventBox.get_window()->draw_pixbuf(_board[*i]._enemies.back().getChildPixbuf()->scale_simple(SCALED_ENEMY_DIAMETER, SCALED_ENEMY_DIAMETER, Gdk::INTERP_HYPER), 0, 0, 
						i->enemyAnchor().x, i->enemyAnchor().y, SCALED_ENEMY_DIAMETER, SCALED_ENEMY_DIAMETER,  Gdk::RGB_DITHER_MAX, 0, 0);
				}
				if (_board[*i]._shields > 0) {
					screenEventBox.get_window()->draw_pixbuf(playerOneShieldPixbuf, 0, 0, i->x - (playerOneShieldPixbuf->get_width() / 2), i->y - (playerOneShieldPixbuf->get_height() / 2), playerOneShieldPixbuf->get_width(), playerOneShieldPixbuf->get_height(), Gdk::RGB_DITHER_MAX, 0, 0);
				}
				if (_board[*i]._shields > 1) {
					screenEventBox.get_window()->draw_pixbuf(playerOneShieldPixbuf, 0, 0,  i->x, i->y, playerOneShieldPixbuf->get_width(), playerOneShieldPixbuf->get_height(), Gdk::RGB_DITHER_MAX, 0, 0);
				}
				//other doodads drawn here
				if (_board.playerAt() == *i ) {
					screenEventBox.get_window()->draw_pixbuf(playerOnePixbuf, 0, 0, i->playerAnchor().x, i->playerAnchor().y, playerOnePixbuf->get_width(), playerOnePixbuf->get_height(), Gdk::RGB_DITHER_MAX, 0, 0);
				}
			}
		}

		//this will need more detail (shields, tokens, etc)
		//maybe Space will have a function to give info formatted
		//also check copy() functions for pixbufs

	}

    void renderTileOnScreen(const Tile& _tile, const Coordinate& _tileAnchorPoint)
    {
        _tile.getScaledChildPixbuf()->composite(screen, _tileAnchorPoint.x, _tileAnchorPoint.y, _tile.getScaledChildPixbuf()->get_width(), _tile.getScaledChildPixbuf()->get_height(), _tileAnchorPoint.x, _tileAnchorPoint.y, 1, 1, Gdk::INTERP_HYPER, 255);
        Coordinate targetHex = _board.centerHexOfTileAnchor(_tileAnchorPoint);

        vector<Coordinate> coordinatePtr;
        coordinatePtr.push_back(targetHex);
        coordinatePtr.push_back(_board.northWestOf(targetHex));
        coordinatePtr.push_back(_board.northEastOf(targetHex));
        coordinatePtr.push_back(_board.southWestOf(targetHex));
        coordinatePtr.push_back(_board.southEastOf(targetHex));
        coordinatePtr.push_back(_board.eastOf(targetHex)     );
        coordinatePtr.push_back(_board.westOf(targetHex)     );
        

        for (auto i = coordinatePtr.begin(); i != coordinatePtr.end(); ++i) {
            if (_board[*i].hasRuin()) 
                 _board[*i].getRuin().getChildPixbuf()->composite(screen, i->ruinAnchor().x, i->ruinAnchor().y, RUIN_WIDTH, RUIN_HEIGHT, i->ruinAnchor().x, i->ruinAnchor().y, 1, 1, Gdk::INTERP_HYPER, 255);
            if (!_board[*i]._enemies.empty()) {
                _board[*i]._enemies.back().getChildPixbuf()->scale_simple(SCALED_ENEMY_DIAMETER, SCALED_ENEMY_DIAMETER, Gdk::INTERP_HYPER)
                    ->composite(screen, i->enemyAnchor().x, i->enemyAnchor().y, SCALED_ENEMY_DIAMETER, SCALED_ENEMY_DIAMETER, 
                    i->enemyAnchor().x, i->enemyAnchor().y, 1, 1, Gdk::INTERP_HYPER, 255);

            }
            if (_board[*i]._shields > 0) {
                playerOneShieldPixbuf->composite(screen, i->x - (playerOneShieldPixbuf->get_width() / 2), i->y - (playerOneShieldPixbuf->get_height() / 2), playerOneShieldPixbuf->get_width(), playerOneShieldPixbuf->get_height(),
                    i->x - (playerOneShieldPixbuf->get_width() / 2), i->y - (playerOneShieldPixbuf->get_height() / 2), 1, 1, Gdk::INTERP_HYPER, 255);
            }
            if (_board[*i]._shields > 1) {
                playerOneShieldPixbuf->composite(screen, i->x, i->y, playerOneShieldPixbuf->get_width(), playerOneShieldPixbuf->get_height(), i->x, i->y, 1, 1, Gdk::INTERP_HYPER, 255);
            }
            //other doodads drawn here
            if (_board.playerAt() == *i ) 
                renderPlayer(*i);
        }
        
        screenImage.set(screen);
        //this will need more detail (shields, tokens, etc)
        //maybe Space will have a function to give info formatted
        //also check copy() functions for pixbufs

	}

	bool onExpose(GdkEventExpose* e) {
		renderTilesOnGDKScreen();
		//screenEventBox.queue_draw_area(e->area.x, e->area.y, e->area.width, e->area.height);
		//screenEventBox.queue_draw();
		
		return false;
	}

    void renderAndUpdateTileForHex(const Coordinate& _anyCoordinate) {
        std::map<Coordinate, Tile>::iterator target = _tilemap.find(_board.tileAnchorforAnyHex(_anyCoordinate));
        renderTileOnScreen(target->second, target->first);
    }

    void renderPlayer(const Coordinate& _centerOfHex) {
        playerOnePixbuf->composite(screen, _centerOfHex.playerAnchor().x, _centerOfHex.playerAnchor().y, playerOnePixbuf->get_width(), playerOnePixbuf->get_height(), 
            _centerOfHex.playerAnchor().x, _centerOfHex.playerAnchor().y,  1, 1, Gdk::INTERP_HYPER, 255);
    }

    void layNewTile(const Tile& _tile, const Coordinate& _outerUnexploredHex);

    void checkPlayerAdjacencyForEnemyReveals() {
        vector<Coordinate> adj;
        if (_board.isValidDirection(_board.playerAt(), NORTHWEST)) adj.push_back(_board.northWestOf(_board.playerAt()));
        if (_board.isValidDirection(_board.playerAt(), NORTHEAST)) adj.push_back(_board.northEastOf(_board.playerAt()));
        if (_board.isValidDirection(_board.playerAt(), SOUTHWEST)) adj.push_back(_board.southWestOf(_board.playerAt()));
        if (_board.isValidDirection(_board.playerAt(), SOUTHEAST)) adj.push_back(_board.southEastOf(_board.playerAt()));
        if (_board.isValidDirection(_board.playerAt(), EAST))      adj.push_back(_board.eastOf(_board.playerAt()));
        if (_board.isValidDirection(_board.playerAt(), WEST))      adj.push_back(_board.westOf(_board.playerAt()));

        for (auto i = adj.begin(); i != adj.end(); ++i) {
            if (_board[*i].getTerrain() == CITY) {
                _board[*i].revealAllEnemies();
                renderAndUpdateTileForHex(*i);
            }
            if (isDay() && (_board[*i].getFeature() == Space::KEEP || _board[*i].getFeature() == Space::MAGE_TOWER)) {
                _board[*i].revealAllEnemies(); 
                renderAndUpdateTileForHex(*i);
            }
        }
        _board[_board.playerAt()].flipRuinUp();
    }

    void toggleHandViewer() {
        if (!_handViewer.get_visible()) {
            _handViewer.show_all();
            _handViewer.updateAll();
        }
        else
            _handViewer.hide();
        _handViewer.raise();
        _handViewer.deiconify();
    }

    void toggleOfferWindow() {
        if (!_offerWindow.get_visible()) {
            _offerWindow.show_all();
        }
        else
            _offerWindow.hide();
        _offerWindow.raise();
        _offerWindow.deiconify();
    }

    void recruit() {
        size_t validSlots = (_fame.getLevel() + 1) / 2;
        
        if (_retinue.size() >= validSlots) {
            _console.print("You need to disband a unit before you may recruit another.");
            return;
        }
        _offerWindow.sendRecruitedUnitTo(_retinue);
    }

	bool playerIsNearConqueredKeep() {
		if (_board[_board.playerAt()].isKeepAndIsConquered())
			return true;
		vector<Coordinate> adj;
		if (_board.isValidDirection(_board.playerAt(), NORTHWEST)) adj.push_back(_board.northWestOf(_board.playerAt()));
		if (_board.isValidDirection(_board.playerAt(), NORTHEAST)) adj.push_back(_board.northEastOf(_board.playerAt()));
		if (_board.isValidDirection(_board.playerAt(), SOUTHWEST)) adj.push_back(_board.southWestOf(_board.playerAt()));
		if (_board.isValidDirection(_board.playerAt(), SOUTHEAST)) adj.push_back(_board.southEastOf(_board.playerAt()));
		if (_board.isValidDirection(_board.playerAt(), EAST))      adj.push_back(_board.eastOf(_board.playerAt()));
		if (_board.isValidDirection(_board.playerAt(), WEST))      adj.push_back(_board.westOf(_board.playerAt()));

		for (auto i = adj.begin(); i != adj.end(); ++i) {
			if (_board[*i].isKeepAndIsConquered())
				return true;
		}
		return false;
	}

	bool playerIsNearConqueredCity() {
		if (_board[_board.playerAt()].isCityAndIsConquered())
			return true;
		vector<Coordinate> adj;
		if (_board.isValidDirection(_board.playerAt(), NORTHWEST)) adj.push_back(_board.northWestOf(_board.playerAt()));
		if (_board.isValidDirection(_board.playerAt(), NORTHEAST)) adj.push_back(_board.northEastOf(_board.playerAt()));
		if (_board.isValidDirection(_board.playerAt(), SOUTHWEST)) adj.push_back(_board.southWestOf(_board.playerAt()));
		if (_board.isValidDirection(_board.playerAt(), SOUTHEAST)) adj.push_back(_board.southEastOf(_board.playerAt()));
		if (_board.isValidDirection(_board.playerAt(), EAST))      adj.push_back(_board.eastOf(_board.playerAt()));
		if (_board.isValidDirection(_board.playerAt(), WEST))      adj.push_back(_board.westOf(_board.playerAt()));

		for (auto i = adj.begin(); i != adj.end(); ++i) {
			if (_board[*i].isCityAndIsConquered())
				return true;
		}
		return false;
	}
	
	size_t getKeepsOwned() {
		size_t result = 0;
		for (auto i = _board.begin(); i != _board.end(); ++i) {
			if (i->second.isKeepAndIsConquered())
               ++result;
		}
		return result;
	}

	size_t getPlayerDrawBonus() {
		size_t keepBonus = 0;
		size_t cityBonus = 0;
		if (playerIsNearConqueredKeep())
			keepBonus = getKeepsOwned();
		if (playerIsNearConqueredCity())
			cityBonus = 2;
		return max(keepBonus, cityBonus);
	}
	
    void endTurn() {
         if (isEndOfGame()) {
             _dummyPlayer.finalTurnforPlayerWarning();
             _endTurnSignal.disconnect();
             _endTurnButton.signal_clicked().connect(sigc::mem_fun(*this, &Game::showScore));
			 _endTurnButton.set_sensitive(true);
             _endRoundButton.set_sensitive(false);
         }
         else {
			 _dummyPlayer.takeTurn(); 
			 ostringstream oss;
			 oss << "Dummy cards remaining: " << _dummyPlayer.getCardsRemaining();
			 _console.print(oss.str());
		 }
		 bool planningTacticActive = isDay() && _tacticWindow.getCurrentTactic() == 4 && _handViewer.hand.size() >= 2;
		 _handViewer.drawToTargetSize(getMaxHandSize() + getPlayerDrawBonus() + (planningTacticActive ? 1 : 0));

		_source.rollSelectedDice_EndOfTurn(); 
        _movePoints.set_value(0);
        _influencePoints.set_value(0);
        _healPoints.set_value(0);
        _skillTokenDeck.playerSkillButton0.endOfTurn();
        _skillTokenDeck.playerSkillButton1.endOfTurn();
        _skillTokenDeck.playerSkillButton2.endOfTurn();
        _skillTokenDeck.playerSkillButton3.endOfTurn();
        _skillTokenDeck.playerSkillButton4.endOfTurn();
        blueMana.setTokens(0);
        redMana.setTokens(0);
        greenMana.setTokens(0);
        whiteMana.setTokens(0);
        goldMana.setTokens(0);
        blackMana.setTokens(0);

		SparingPowerPrompt();
    }

    void endRound() {
		_endTurnButton.set_sensitive(true);
        if (_roundNumber == 6 || isEndOfGame()) {
            showScore();
            return;
        }
        ++_roundNumber;
        _handViewer.restartAndShuffle();
		
        _offerWindow.endOfRound();   //takes care of resetting dummy player as well
        _source.rollForEndOfRound(); 
        _movePoints.set_value(0);
        _influencePoints.set_value(0);
        _healPoints.set_value(0);
        _skillTokenDeck.playerSkillButton0.endOfRound();
        _skillTokenDeck.playerSkillButton1.endOfRound();
        _skillTokenDeck.playerSkillButton2.endOfRound();
        _skillTokenDeck.playerSkillButton3.endOfRound();
        _skillTokenDeck.playerSkillButton4.endOfRound();
        _retinue.endOfRound();
		blueMana.setTokens(0);
        redMana.setTokens(0);
        greenMana.setTokens(0);
        whiteMana.setTokens(0);
        goldMana.setTokens(0);
        blackMana.setTokens(0);
        _dayNightImage.flip();
        if (isDay()) {
            for (auto i = _board.begin(); i != _board.end(); ++i) {
                if (i->second.hasFacedownRuin()) {
                    i->second.flipRuinUp();
                    renderAndUpdateTileForHex(i->first);
                }
            }
        }
        _handViewer.hide();
		_handViewer._deedPileViewer.window.hide();
		_handViewer._discardViewer.window.hide();
        _offerWindow.hide();
        if (isDay())
            _tacticWindow.offerDayTactic();
        else
            _tacticWindow.offerNightTactic();

        if (_tacticWindow.dummyGoesFirst())
            _dummyPlayer.takeTurn();

		_handViewer.drawToTargetSize(getMaxHandSize() + getPlayerDrawBonus() + (isDay() && _tacticWindow.getCurrentTactic() == 5 ? 2 : 0));

		ostringstream oss;
		oss << "Dummy cards remaining: " << _dummyPlayer.getCardsRemaining();
		_console.print(oss.str());

		checkPlayerAdjacencyForEnemyReveals();
		SparingPowerPrompt();
	}

    bool isEndOfGame() {
        size_t conqueredCities = 0;
        for (auto i = _board.begin(); i != _board.end(); ++i) {
			if (i->second.getTerrain() == CITY && (i->second.getEnemies().empty())) { 
                ++conqueredCities;
            }
        }
        return conqueredCities >= 2;
    }

    void showScore() {
        size_t spells = 0;
        size_t advActions = 0;
        size_t artifacts = 0;
        size_t totalCrystals = 0;
        size_t totalUnitLevels = 0;
        size_t nonCityShields = 0;
        size_t wounds = 0;
        size_t conqueredCities = 0;
        size_t conqueredCitiesFameValue = 0;
        size_t dummyCardsRemaining = 0;
        bool endOfRoundAnnounced = false;
        size_t unplayedRounds = 0;

        _handViewer.restartAndShuffle();

        for (auto i = _handViewer.deck.begin(); i != _handViewer.deck.end(); ++i) {
            if (i->getType() == Deed::SPELL)           ++spells;
            if (i->getType() == Deed::ADVANCED_ACTION) ++advActions;
            if (i->getType() == Deed::ARTIFACT)        ++artifacts;
            if (i->isWound())                          ++wounds;
        }
        totalCrystals = blueCrystals. getCrystals() +
                        redCrystals.  getCrystals() +
                        greenCrystals.getCrystals() +
                        whiteCrystals.getCrystals();
            
        for (auto i = _retinue.begin(); i != _retinue.end(); ++i) {
            totalUnitLevels += (i->getLevel() / (i->getWounds() > 0 ? 2 : 1 ));
			if (i->hasBanner())
				++artifacts;
        }
		for (auto i = _retinue.bondedUnitVector.begin(); i != _retinue.bondedUnitVector.end(); ++i) {
            totalUnitLevels += (i->getLevel() / (i->getWounds() > 0 ? 2 : 1 ));
			if (i->hasBanner())
				++artifacts;
        }
        for (auto i = _board.begin(); i != _board.end(); ++i) {
            if (i->second.getTerrain() != Terrain::CITY)
                nonCityShields += i->second.getShields();
            else if (i->second.getEnemies().empty()) 
                ++conqueredCities;
        }
        dummyCardsRemaining = _dummyPlayer.size();
        endOfRoundAnnounced = _dummyPlayer.empty();
        unplayedRounds = 6 - _roundNumber;

        ostringstream oss;
        oss << "Fame earned:            " << _fame.getFame() << endl;
        oss << "Total Spells:           " << spells              << "  ( " <<    spells * 2           << " Fame)" << endl;
        oss << "Total Advanced Actions: " << advActions          << "  ( " <<    advActions           << " Fame)" << endl;
        oss << "Total Artifacts:        " << artifacts           << "  ( " <<    artifacts * 2        << " Fame)" << endl;
        oss << "Total Crystals:         " << totalCrystals       << "  ( " <<    totalCrystals / 2    << " Fame)" << endl;
        oss << "Retinue value:          " << " "                 << "  ( " <<    totalUnitLevels      << " Fame)" << endl;
        oss << "Total Sites Conquered:  " << nonCityShields      << "  ( " <<    nonCityShields * 2   << " Fame)" << endl;
        oss << "Total Wounds:           " << wounds              << "  ( " <<    -(int(wounds) * 2)   << " Fame)" << endl;
        switch (conqueredCities) {
        case 2:
            oss << "You have conquered both cities. Well done. Fame awarded: 10 per city, plus victory bonus of 15. Total: 35\n";
            conqueredCitiesFameValue = 35;
            break;
        case 1:
            oss << "You have conquered one of the two cities. Not bad. Fame awarded: 10\n";
            conqueredCitiesFameValue = 10;
            break;
        case 0:
            oss << "You failed to conquer either city. No Fame awarded\n";
        }

        oss << "Opponent's Cards Remaining:   " << _dummyPlayer.size() << "  (" << _dummyPlayer.size() << " Fame)" << endl;
        oss << "Number of Unplayed Rounds: "    << unplayedRounds      << "  (" << unplayedRounds * 30 << " Fame)" << endl;
        if (_dummyPlayer.empty())
            oss << "End of round was announced.  (0 Fame)\n";
        else
            oss << "End of round was not announced.  (5 Fame)\n";
        size_t finalScore = 
            (_fame.getFame())        +
            (spells * 2        )     +
            (advActions        )     +
            (artifacts * 2     )     +
            (totalCrystals / 2 )     +
            (totalUnitLevels   )     +
            (nonCityShields * 2)     +
            (-(int(wounds) * 2))     +
            conqueredCitiesFameValue +
            (unplayedRounds * 30)    +
            (_dummyPlayer.size())    +
            (_dummyPlayer.empty() ? 0 : 5);
        oss << "\n*****  FINAL SCORE:  " << finalScore << "  *****\n";


        Gtk::Window scoreDisplayWindow;
        scoreDisplayWindow.set_transient_for(*this);
        scoreDisplayWindow.set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        scoreDisplayWindow.set_title("Your journey ends");
        scoreDisplayWindow.set_icon(playerOneShieldPixbuf);
        Gtk::TextView scoreView;
        scoreView.modify_font(_font);
        scoreView.set_editable(false);
        scoreDisplayWindow.add(scoreView);
        scoreView.get_buffer()->set_text(oss.str());
        scoreDisplayWindow.show_all();

        Gtk::Main::run(scoreDisplayWindow);
    }

    size_t getMaxHandSize() {
        if (_fame.getLevel() >= 9)
            return 7;
        if (_fame.getLevel() >= 5)
            return 6;
        return 5;
    }

    size_t getPlayerArmor() {
        if (_fame.getLevel() >= 7)
            return 4;
        if (_fame.getLevel() >= 3)
            return 3;
        return 2;
    }

    void levelUpCheck() {
		switch (_fame.getLevel()) {
        case 2:
            _skillTokenDeck.playerSkillButton0.setReadyForNewSkill();
			break;
        case 3:
            _retinue.unit1.show_all();
            _armorLabel.set_text("Armor: 3");
            break;
        case 4:
            _skillTokenDeck.playerSkillButton1.setReadyForNewSkill();
			break;
        case 5:
            _retinue.unit2.show_all();
            _handsizeLabel.set_text("Hand Size: 6"); 
            break;
        case 6:
            _skillTokenDeck.playerSkillButton2.setReadyForNewSkill();
			break;
        case 7:
            _retinue.unit3.show_all();
            _armorLabel.set_text("Armor: 4");
            break;
        case 8:
            _skillTokenDeck.playerSkillButton3.setReadyForNewSkill();
			break;
        case 9:
            _retinue.unit4.show_all();
            _handsizeLabel.set_text("Hand Size: 7"); 
            break;
        case 10:
            _skillTokenDeck.playerSkillButton4.setReadyForNewSkill();
			break;
        }
	}

	void makeBondsOfLoyaltyUsable() {
		_retinue.unitBonded.show_all();
		_offerWindow.addRegularUnitToOfferButton.clicked();
		_offerWindow.addRegularUnitToOfferButton.clicked();
	}

	void recruitBondedUnit() {
		if (_offerWindow.dummyUnitButton.get_active())
			return;
		if (!_retinue.bondedUnitVector.empty()) {
            _console.print("You may have only one bonded unit, and it can only leave you through a death in combat.");
            return;
        }

		_offerWindow.sendRecruitedUnitTo(_retinue, true);
		_offerWindow.dummyUnitButton.clicked();
	}

	bool on_tactic_right_click(GdkEventButton* e) {
		_tacticMenuPopup->hide_all();
		if (e->button != 3 || isDay()) 
			return false;
		switch(_tacticWindow.getCurrentTactic()) {
		case 1:
		case 3:
		case 5:
			return false;
		case 2:
			_tacticUIManager->get_widget("/Tactic/Long Night")->show_all();
			break;
		case 4:
			_tacticUIManager->get_widget("/Tactic/Midnight Meditation")->show_all();
			break;
		case 6:
			_tacticUIManager->get_widget("/Tactic/Sparing Power give")->show_all();
			_tacticUIManager->get_widget("/Tactic/Sparing Power take")->show_all();
			break;
		}
		_tacticMenuPopup->show();
		_tacticMenuPopup->popup(e->button, e->time);
		return true;
	}

	void longNight() {
		_handViewer.longNight();
		_tacticWindow.flipCurrentTacticDown();
	}

	void midnightMeditation() {
		_tacticWindow.flipCurrentTacticDown();
	}

	void SparingPowerTakeAll() {
		_handViewer.SparingPowerTakeAll();
		_tacticWindow.flipCurrentTacticDown();
	}

	void SparingPowerPrompt() {
		if (isNight() && _tacticWindow.getCurrentTactic() == 6 && _tacticWindow.currentTacticIsFaceUp()) {
			Gtk::MessageDialog SparingPowerWindow(*this, "Choose an option for Sparing Power", false, Gtk::MESSAGE_OTHER, Gtk::BUTTONS_OK, true);
			SparingPowerWindow.get_action_area()->set_layout(Gtk::BUTTONBOX_CENTER);
			SparingPowerWindow.run();
		}
	}

	bool on_hand_card_right_click(size_t slot, GdkEventButton* e) {
		_handMenuPopup->hide_all();
		if (e->button != 3) 
			return false;
		if (_handViewer.hand[slot].getName() == "Meditation") {
			//_handMenuPopup->show_all();
			//_handMenuPopup->popup(e->button, e->time);
			//return true;
			return false;
		}

		if (_handViewer.hand[slot].isWound()) {
			//_handViewer.hand.erase(slot);
			//_console.print("Wound healed.");
			return true;
		}
		else
			return false;
	}

	void modifyCardMenu(CardButton& cardButton) {
		cardButton._menuPopup->hide_all();

		if (cardButton.ptr->at(cardButton.getSlot()).isWound()) {
			_handViewer.hand.erase(cardButton.getSlot());
			_console.print("Wound healed.");
			cardButton.setPopupMenuNeeded(false);
			return;
		}

		if (cardButton.ptr->at(cardButton.getSlot()).getName() == "Meditation") {
			cardButton.setPopupMenuNeeded(true);
			dynamic_cast<Gtk::MenuItem*>(cardButton._UIManager->get_widget("/Special Actions/Meditation"))->show_all();
			return;
		}

		if (cardButton.ptr->at(cardButton.getSlot()).isBanner() && !(_retinue.empty() && _retinue.bondedUnitVector.empty())) {
			cardButton.setPopupMenuNeeded(true);
			Gtk::MenuItem* _cardMenuItem; 
			switch (_retinue.size()) {
			case 5:
				_cardMenuItem = dynamic_cast<Gtk::MenuItem*>(cardButton._UIManager->get_widget("/Special Actions/Attach Banner 4"));
				_cardMenuItem->show_all();
				_cardMenuItem->set_label("Attach to " + _retinue[4].getName());
				//fallthrough
			case 4: 
				_cardMenuItem = dynamic_cast<Gtk::MenuItem*>(cardButton._UIManager->get_widget("/Special Actions/Attach Banner 3"));
				_cardMenuItem->show_all();
				_cardMenuItem->set_label("Attach to " + _retinue[3].getName());
				//fallthrough
			case 3: 
				_cardMenuItem = dynamic_cast<Gtk::MenuItem*>(cardButton._UIManager->get_widget("/Special Actions/Attach Banner 2"));
				_cardMenuItem->show_all();
				_cardMenuItem->set_label("Attach to " + _retinue[2].getName());
				//fallthrough
			case 2: 
				_cardMenuItem = dynamic_cast<Gtk::MenuItem*>(cardButton._UIManager->get_widget("/Special Actions/Attach Banner 1"));
				_cardMenuItem->show_all();
				_cardMenuItem->set_label("Attach to " + _retinue[1].getName());
				//fallthrough
			case 1: 
				_cardMenuItem = dynamic_cast<Gtk::MenuItem*>(cardButton._UIManager->get_widget("/Special Actions/Attach Banner 0"));
				_cardMenuItem->show_all();
				_cardMenuItem->set_label("Attach to " + _retinue[0].getName());
			}
			if (!_retinue.bondedUnitVector.empty()) {
				_cardMenuItem = dynamic_cast<Gtk::MenuItem*>(cardButton._UIManager->get_widget("/Special Actions/Attach Banner 5"));
				_cardMenuItem->show_all();
				_cardMenuItem->set_label("Attach to " + _retinue.bondedUnitVector.front().getName());
			}
		}
		else {
			cardButton.setPopupMenuNeeded(false);
		}
	}

	void attachBannerToUnit(const Deed& banner, size_t slot) {
		if (_retinue[slot].hasBanner()) {
			_handViewer.discardPile.push_back(_retinue[slot].getBanner());
			_handViewer.updateDiscardViewer();
		}
		_retinue[slot].attachBanner(banner);
		_retinue.updateAll();
		_handViewer.findAndEraseBanner(banner);
		
	}

	void attachBannerToBondedUnit(const Deed& banner, size_t slot) {
		if (_retinue.bondedUnitVector[slot].hasBanner()) {
			_handViewer.discardPile.push_back(_retinue.bondedUnitVector[slot].getBanner());
			_handViewer.updateDiscardViewer();
		}
		_retinue.bondedUnitVector[slot].attachBanner(banner);
		_retinue.updateAll();
		_handViewer.findAndEraseBanner(banner);
	}

	void removeBannerFromUnit(const Deed& banner) {
		_handViewer.discardPile.push_back(banner);
		_handViewer.updateDiscardViewer();
	}


    bool on_right_click(GdkEventButton* e);



    /******************   Right-click and site commands   ******************/
    
    void retreat() {
        Coordinate source = _board.playerAt();  
        _board.movePlayerTo(rightClickedHex);
        checkPlayerAdjacencyForEnemyReveals();
        renderAndUpdateTileForHex(source);               
        renderAndUpdateTileForHex(rightClickedHex);
        _handViewer.takeWound();
        _console.print("In a desperate retreat, you have suffered 1 wound.");
    }

    void enterAdventureSite() {
        if (!_board[rightClickedHex].isAdventureSite())
            return;

    }

    void removeRuin() {
        _board[rightClickedHex].removeRuin();
        renderAndUpdateTileForHex(rightClickedHex);
    }

    void revealRuin() {
        _board[rightClickedHex].flipRuinUp();
        renderAndUpdateTileForHex(rightClickedHex);
    }

    void enterRuins(); 

    void plunderVillage() {
        _handViewer.drawCards(2);
        _reputation._scale.set_value(_reputation._scale.get_value() - 1);
        _console.print("You have plundered a village, and your reputation lowers by 1 point.");
    }

    void takeCrystalFromMine() {
        switch (_board[_board.playerAt()].getFeature()) {
        case Space::CRYSTAL_MINES_BLUE:  
            blueCrystals.gainOne();  
            break;
        case Space::CRYSTAL_MINES_RED:   
            redCrystals.gainOne();   
            break;
        case Space::CRYSTAL_MINES_GREEN: 
            greenCrystals.gainOne(); 
            break;
        case Space::CRYSTAL_MINES_WHITE: 
            whiteCrystals.gainOne(); 
            break;
        }
    }
    void takeFromMagicalGlade() {
        if (isDay())
            goldMana.increase();
        if (isNight())
            blackMana.increase();
    }

    void takeManaToken(ManaColor c) {
        switch (c) {
        case BLUE  : 
            blueMana.increase(); 
            break;
        case RED   : 
            redMana.increase();  
            break;
        case GREEN : 
            greenMana.increase(); 
            break;
        case WHITE : 
            whiteMana.increase(); 
            break;
        case GOLD  : 
            goldMana.increase(); 
            break;
        case BLACK : 
            blackMana.increase(); 
            break;
        }
    }

    void addEnemy(Enemy::Type type, bool faceup = true) {
		if (_enemyDecks.enemyTypeNotAvailable(type)) {
			_console.print("No more enemies of that type are available.");
			return;
		}
        _board[rightClickedHex].addEnemy(_enemyDecks.drawOne(type));
        if (faceup) 
            _board[rightClickedHex]._enemies.back().flipUp();
        renderAndUpdateTileForHex(rightClickedHex);
    }

    void removeSingleEnemy(const Coordinate& c) {
        _board[c].killEnemy(0);
    }

    void revealAllEnemiesInCurrentSpace() {
        _board[rightClickedHex].revealAllEnemies();
        _enemyViewer.setViewer(_board[rightClickedHex]);
        giveSpaceNewEnemies();
        _enemyViewer.updateAll();
    }

	void fight() {
        if (_enemyViewer.get_visible())
            return;
        _enemyViewer.setViewer(_board[rightClickedHex]);
        _enemyViewer.set_transient_for(*this);
        _enemyViewer.show_all();
        _enemyViewer.updateAll();
    }

	void printFameTotal(size_t total) {
		ostringstream oss;
		oss << "You have earned " << total << " fame, bringing your total up to " << _fame.getFame() + total;
		_console.print(oss.str());
	}

    void giveSpaceNewEnemies() {
        _board[rightClickedHex].setEnemies(_enemyViewer.getVector());
        renderAndUpdateTileForHex(rightClickedHex);
    }
    
    void placeShield () { 
        ++_board[rightClickedHex]._shields; 
        renderAndUpdateTileForHex(rightClickedHex);
    }
    void removeShield() {
        if (_board[rightClickedHex]._shields > 0) {
            --_board[rightClickedHex]._shields; 
            renderAndUpdateTileForHex(rightClickedHex);
        }
    }

    void openInfo() {
        _infoWindow.openToPage(_board[rightClickedHex].getFeature());
        _infoWindow.raise();
    }

    void openArtifactChest() {
        size_t awarded = numberPrompt();
        if (awarded == 0)
            return;
        list<Deed> won = _offerWindow.getChosenArtifacts(awarded);
        while (!won.empty()) {
            _handViewer.deck.push_back(won.front());
            won.pop_front();
        }
    }

    int numberPrompt() {
        Gtk::Window win;
        Gtk::SpinButton spin;
        spin.set_range(0, 5);
        spin.set_increments(1, 1);
        spin.modify_font(_font);
        spin.set_alignment(Gtk::ALIGN_CENTER);
        Gtk::Button ok("This many");
        ok.modify_font(_font);
        Gtk::VBox vbox;
        vbox.pack_start(spin);
        vbox.pack_start(ok);
        //_mainTable.attach(vbox, 2, 3, 2, 3, Gtk::SHRINK, Gtk::SHRINK);
        ok.signal_clicked().connect(sigc::ptr_fun(Gtk::Main::quit));
        win.add(vbox);
        win.set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        win.set_title("Number of artifacts won this turn?");
        win.set_icon(Gdk::Pixbuf::create_from_file(RootDirectory + "artifact_golden_grail.jpg"));
        win.set_modal();
        win.set_transient_for(*this);
        win.show_all();
        Gtk::Main::run();
        return spin.get_value();
    }

    void burnMonastery() {
        _board[rightClickedHex].burn();
        _offerWindow.monasteryActionOffer.monasteryBurned();
        renderAndUpdateTileForHex(rightClickedHex);
    }

    void takeAdvancedActionFromGreenCity() {
        if (!_offerWindow.advActionDeck.empty()) {
            _handViewer.putInDeck(_offerWindow.advActionDeck.back());
            _offerWindow.advActionDeck.pop_back();
        }
    }

    /*
    
    If you failed to defeat enemies drawn in a dungeon, tomb
or monastery, discard them. The next time a Hero attempts
to fight here, new enemies of the appropriate type will be
drawn.


If you failed to defeat the enemy in a monster den, or one or
both monsters in a spawning grounds or ruins, return any
remaining monsters face up on the space. Next time a Hero
attempts to fight here, they will fight the same enemies.

Ruins -- no replacements for defeated

Spawning grounds -- replace defeated with face down token -- while ( < 2) ...
--can auto replenish when enemy viewer closes
    
    */



    /************************************************************************/
    


    void print(std::map<Coordinate, Space>& m) {
        for (std::map<Coordinate, Space>::iterator i = m.begin(); i != m.end(); ++i) {
            if (i->second.getTerrain() != UNEXPLORED)
                cout << i->first << " " << i->second.getTerrainString() << endl ;
        }
    }
    void print(std::map<Coordinate, Tile>& m) {
        for (std::map<Coordinate, Tile>::iterator i = m.begin(); i != m.end(); ++i) {
            cout << i->first << " -- ";
            if (_board.find(i->first) != _board.end())
                cout << "On board.  " <<  _board[i->first].getTerrainString() << endl;
        }
    }

};

Game::Game()
    :  origin(Coordinate(SCALED_TILE_WIDTH + (HEX_WIDTH * 11 / 2 ), (SCALED_TILE_HEIGHT * 11) - (SCALED_TILE_HEIGHT / 2)  - 30)),//extra for a small leeway border
      _board(origin.x, origin.y),
      blueCrystals (crystal::BLUE,  .44),  //was 40
      redCrystals  (crystal::RED,   .44),  //was 40
      greenCrystals(crystal::GREEN, .44),  //was 40
      whiteCrystals(crystal::WHITE, .44),  //was 40
      blackMana(BLACK), goldMana(GOLD), blueMana(BLUE), redMana(RED), greenMana(GREEN), whiteMana(WHITE),
      _source(.59), _retinue(.45), _offerWindow(.40)
{
	set_keep_below(true);
	
    set_title("Mage Knight v0.1");
    set_icon(Gdk::Pixbuf::create_from_file(RootDirectory + "mainIcon.png"));
	//modify_bg(Gtk::STATE_NORMAL, Gdk::Color("#F8E9D3"));
    set_can_focus();
    grab_focus();
    add_events(Gdk::ALL_EVENTS_MASK);
    _infoWindow.set_transient_for(*this);
   
    _font.set_family("Sakkal Majalla");
    _font.set_size(PANGO_SCALE * 24);

    _crystalTable.resize(2, 2);
    _crystalTable.attach(blueCrystals , 0, 1, 0, 1, Gtk::EXPAND, Gtk::EXPAND);
    _crystalTable.attach(redCrystals  , 1, 2, 0, 1, Gtk::EXPAND, Gtk::EXPAND);
    _crystalTable.attach(greenCrystals, 0, 1, 1, 2, Gtk::EXPAND, Gtk::EXPAND);
    _crystalTable.attach(whiteCrystals, 1, 2, 1, 2, Gtk::EXPAND, Gtk::EXPAND);
    _crystalFrame.add(_crystalTable);
    _crystalFrame.set_label_align(0, 0.5);
    _crystalFrame.set_shadow_type(Gtk::SHADOW_NONE);
    _crystalLabel.set_markup("<span font_family = \"Sakkal Majalla\" size=\"18432\">Crystals</span>");
    _crystalFrame.set_label_widget(_crystalLabel);
  
    //or VBox size request ?
    _manaTokenBoxTop.pack_start   (blueMana , true, true);
    _manaTokenBoxTop.pack_start   (redMana  , true, true);
    _manaTokenBoxTop.pack_start   (goldMana , true, true);
    _manaTokenBoxBottom.pack_start(greenMana, true, true);
    _manaTokenBoxBottom.pack_start(whiteMana, true, true);
    _manaTokenBoxBottom.pack_start(blackMana, true, true);
    
    _manaTokenBoxAll.pack_start(_manaTokenBoxTop);
    _manaTokenBoxAll.pack_start(_manaTokenBoxBottom);
    _manaTokenBoxAll.set_size_request(84 * .57 * 3, 84 * .57 * 2); //fix with variables

	_source.setGameWindow(this);
    
    _sourceDayBox.set_homogeneous(false);
    //_sourceDayBox.set_spacing(15);
	
    _sourceDayBox.pack_start(_dayNightImage, false, false, 15);
	_sourceDayBox.pack_end(_source, false, false);
	
	
	_tacticEventBox.add(_tacticWindow.currentTacticImage);

	_upperRightTable.resize(2, 2);
    _upperRightTable.attach(_tacticEventBox,                  0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
    _upperRightTable.attach(_sourceDayBox,                    1, 2, 0, 1, Gtk::FILL, Gtk::FILL);   //was FILL
    _upperRightTable.attach(_manaTokenBoxAll,                 0, 1, 1, 2, Gtk::FILL, Gtk::FILL);   //was FILL
    _upperRightTable.attach(_crystalFrame,                    1, 2, 1, 2, Gtk::FILL, Gtk::FILL);   //was FILL

    Pango::FontDescription sliderFont = _font;
    sliderFont.set_size(PANGO_SCALE * 16);
    _fame._scale.modify_font(sliderFont);
    _reputation._scale.set_increments(1, 1); 
    _reputation._scale.set_inverted();
    _reputation._scale.set_digits(0);
    _reputation._scale.set_can_focus(false);
    _reputation._scale.modify_font(_font);
    _reputation._scale.add_mark(0, Gtk::POS_LEFT, "");
    _armorLabel.set_text("Armor: 2");
    _armorLabel.modify_font(sliderFont);
    _handsizeLabel.modify_font(sliderFont);
    _handsizeLabel.set_text("Hand Size: 5");
    _sliderHBox.pack_start(_fame);
    _sliderHBox.pack_start(_reputation);
    _sliderArmorTable.resize(2, 2);
    _sliderArmorTable.attach(_sliderHBox,    0, 2, 0, 1);
    _sliderArmorTable.attach(_armorLabel,    0, 1, 1, 2, Gtk::EXPAND,  Gtk::EXPAND, 25, 25);
    _sliderArmorTable.attach(_handsizeLabel, 1, 2, 1, 2, Gtk::EXPAND,  Gtk::EXPAND, 25, 25);
        
    _moveLabel.set_label("Move:");
    _moveLabel.set_alignment(0, 0.5);
    _influenceLabel.set_label("Influence:");
    _influenceLabel.set_alignment(0, 0.5);
    _healLabel.set_label("Heal:");
    _healLabel.set_alignment(0, 0.5);
    
    _moveLabel.modify_font(_font);                                 _movePoints.modify_font(_font);
    _influenceLabel.modify_font(_font);                            _influencePoints.modify_font(_font);
    _healLabel.modify_font(_font);                                 _healPoints.modify_font(_font);
                                                                   
    _statLabelVBox.set_spacing(0);                                 _statReadoutBox.set_spacing(0);
    _statLabelVBox.pack_start(_moveLabel, Gtk::PACK_EXPAND_PADDING);       _statReadoutBox.pack_start(_movePoints, Gtk::PACK_SHRINK);
    _statLabelVBox.pack_start(_influenceLabel, Gtk::PACK_EXPAND_PADDING);  _statReadoutBox.pack_start(_influencePoints, Gtk::PACK_SHRINK);
    _statLabelVBox.pack_start(_healLabel, Gtk::PACK_EXPAND_PADDING);       _statReadoutBox.pack_start(_healPoints, Gtk::PACK_SHRINK);

    _statHbox.set_spacing(0);
    _statHbox.pack_start(_statLabelVBox, Gtk::PACK_SHRINK);
    _statHbox.pack_start(_statReadoutBox, Gtk::PACK_SHRINK);
    
    
    //backgroundPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "background.gif", SCALED_TILE_WIDTH * 11, SCALED_TILE_HEIGHT * 11);
    backgroundPixbuf = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, SCALED_TILE_WIDTH * 11, SCALED_TILE_HEIGHT * 11);
    backgroundPixbuf->fill(0x000000FF);
    origin = Coordinate(SCALED_TILE_WIDTH + (HEX_WIDTH * 11 / 2 ), backgroundPixbuf->get_height() - (SCALED_TILE_HEIGHT / 2)  - 30); 
    
    screen = backgroundPixbuf->copy();
    screenEventBox.add(screenImage);
    screenEventBox.add_events(Gdk::ALL_EVENTS_MASK);
    screenEventBox.set_has_tooltip(true);
    mainScrollWindow.add(screenEventBox);

    screenEventBox.modify_bg(Gtk::STATE_NORMAL, Gdk::Color("#000000"));
    mainScrollWindow.modify_bg(Gtk::STATE_NORMAL, Gdk::Color("#000000"));
    mainScrollWindow.set_shadow_type(Gtk::SHADOW_NONE);
   
    screenEventBox.signal_query_tooltip().connect(sigc::mem_fun(*this, &Game::showSpaceInfoTooltip));
    screenEventBox.signal_button_press_event().connect(sigc::mem_fun(*this, &Game::movePlayer));
    screenEventBox.signal_button_press_event().connect(sigc::mem_fun(*this, &Game::on_right_click));

    Glib::signal_timeout().connect( sigc::mem_fun(*this, &Game::restoreTooltip), 50 );
    screenEventBox.signal_motion_notify_event().connect(sigc::mem_fun(*this, &Game::removeTooltip));

    _enemyViewer.confirmButton.signal_clicked().connect(sigc::mem_fun(*this, &Game::giveSpaceNewEnemies), false);
    _enemyViewer.revealAllButton.signal_clicked().connect(sigc::mem_fun(*this, &Game::revealAllEnemiesInCurrentSpace), false);
	_enemyViewer.fameTotalSignal.connect(sigc::mem_fun(*this, &Game::printFameTotal));

    _offerWindow._dummyPlayerPtr = &_dummyPlayer;
    _offerWindow.advActionOffer.sendToHandButton    .signal_clicked().connect
        (sigc::bind<vectorDeedTableOfButtons&>(sigc::mem_fun(_offerWindow.advActionOffer, &OfferRow::sendSelectedToPile), _handViewer.hand));
    _offerWindow.advActionOffer.sendToDrawDeckButton.signal_clicked().connect
        (sigc::bind<vector<Deed>&>            (sigc::mem_fun(_offerWindow.advActionOffer, &OfferRow::sendSelectedToStandardVector), _handViewer.deck));
	_offerWindow.advActionOffer.sendToDrawDeckButton.signal_clicked().connect(sigc::mem_fun(_handViewer, &HandViewer::updateTitle));

    _offerWindow.spellOffer.sendToHandButton    .signal_clicked().connect
        (sigc::bind<vectorDeedTableOfButtons&>(sigc::mem_fun(_offerWindow.spellOffer, &OfferRow::sendSelectedToPile), _handViewer.hand));
    _offerWindow.spellOffer.sendToDrawDeckButton.signal_clicked().connect
        (sigc::bind<vector<Deed>&>            (sigc::mem_fun(_offerWindow.spellOffer, &OfferRow::sendSelectedToStandardVector), _handViewer.deck));
	_offerWindow.spellOffer.sendToDrawDeckButton.signal_clicked().connect(sigc::mem_fun(_handViewer, &HandViewer::updateTitle));

    _offerWindow.monasteryActionOffer.sendToHandButton.signal_clicked().connect
        (sigc::bind<vectorDeedTableOfButtons&>(sigc::mem_fun(_offerWindow.monasteryActionOffer, &OfferRow::sendSelectedToPile), _handViewer.hand));
    _offerWindow.monasteryActionOffer.sendToDrawDeckButton.signal_clicked().connect
        (sigc::bind<vector<Deed>&>            (sigc::mem_fun(_offerWindow.monasteryActionOffer, &OfferRow::sendSelectedToStandardVector), _handViewer.deck));
	_offerWindow.monasteryActionOffer.sendToDrawDeckButton.signal_clicked().connect(sigc::mem_fun(_handViewer, &HandViewer::updateTitle));


    _offerWindow.sendToRetinueButton.signal_clicked().connect(sigc::mem_fun(*this, &Game::recruit));
   

    _actionGroup = Gtk::ActionGroup::create("Space");

    _actionGroup->add(Gtk::Action::create("Get info", "Get info"), sigc::mem_fun(*this, &Game::openInfo));

    _actionGroup->add(Gtk::Action::create("Engage enemies", "Engage enemies"), sigc::mem_fun(*this, &Game::fight));  
    _actionGroup->add(Gtk::Action::create("Enter site", "Enter ")); 
    _actionGroup->add(Gtk::Action::create("Retreat",    "Retreat to here (1 wound)"), sigc::mem_fun(*this, &Game::retreat));  

    _actionGroup->add(Gtk::Action::create("Add an enemy", "Add an enemy")); 
    _actionGroup->add(Gtk::Action::create("Orcs",         "Orcs"      ), sigc::bind<Enemy::Type, bool> (sigc::mem_fun(*this, &Game::addEnemy), Enemy::ORCS,       false));   
    _actionGroup->add(Gtk::Action::create("Draconum",     "Draconum"  ), sigc::bind<Enemy::Type, bool> (sigc::mem_fun(*this, &Game::addEnemy), Enemy::DRACONUM,   false));   
    _actionGroup->add(Gtk::Action::create("Keep",         "Keep"      ), sigc::bind<Enemy::Type, bool> (sigc::mem_fun(*this, &Game::addEnemy), Enemy::KEEP,       false));   
    _actionGroup->add(Gtk::Action::create("City",         "City"      ), sigc::bind<Enemy::Type, bool> (sigc::mem_fun(*this, &Game::addEnemy), Enemy::CITY,       false));   
    _actionGroup->add(Gtk::Action::create("Mage Tower",   "Mage Tower"), sigc::bind<Enemy::Type, bool> (sigc::mem_fun(*this, &Game::addEnemy), Enemy::MAGE_TOWER, false));   
    _actionGroup->add(Gtk::Action::create("Dungeon",      "Dungeon"   ), sigc::bind<Enemy::Type, bool> (sigc::mem_fun(*this, &Game::addEnemy), Enemy::DUNGEON,    false));

    _actionGroup->add(Gtk::Action::create("Recruit", "Recruit"),             sigc::mem_fun(_offerWindow, &OfferWindow::openOfferWindow)); 
    _actionGroup->add(Gtk::Action::create("Heal", "Heal"),                   sigc::mem_fun(_handViewer, &HandViewer::healFromHand)); 
    _actionGroup->add(Gtk::Action::create("Plunder", "Plunder village"),     sigc::mem_fun(*this, &Game::plunderVillage)); 

    _actionGroup->add(Gtk::Action::create("Buy spell", "Buy spell (7 Influence + 1 mana of same color)"), sigc::mem_fun(_offerWindow, &OfferWindow::openOfferWindow)); 
    _actionGroup->add(Gtk::Action::create("Buy artifact", "Buy artifact (12 Influence)"),                 sigc::mem_fun(*this, &Game::openArtifactChest));
    _actionGroup->add(Gtk::Action::create("Buy advanced action", "Buy advanced action (6 Influence)"),    sigc::mem_fun(_offerWindow, &OfferWindow::openOfferWindow)); 

    _actionGroup->add(Gtk::Action::create("Gain mana", "Gain mana"),           sigc::mem_fun(*this, &Game::takeFromMagicalGlade)); 
    _actionGroup->add(Gtk::Action::create("Gain crystal", "Gain crystal"),     sigc::mem_fun(*this, &Game::takeCrystalFromMine)); 
    _actionGroup->add(Gtk::Action::create("Heal from hand", "Heal from hand"), sigc::mem_fun(_handViewer, &HandViewer::healFromHand)); 
    _actionGroup->add(Gtk::Action::create("Heal from discard pile", "Heal from discard pile"), sigc::mem_fun(_handViewer, &HandViewer::healFromDiscard)); 
     
    _actionGroup->add(Gtk::Action::create("Add one elite", "Add one elite unit to offer (2 Influence)")); 
    _actionGroup->add(Gtk::Action::create("Buy advanced action from top of deck", "Buy advanced action from top of deck (6 Influence)"), sigc::mem_fun(*this, &Game::takeAdvancedActionFromGreenCity)); 

    _actionGroup->add(Gtk::Action::create("Train at monastery", "Train at monastery (6 Influence)"), sigc::mem_fun(_offerWindow, &OfferWindow::openMonasteryWindow));
    _actionGroup->add(Gtk::Action::create("Burn monastery", "Burn monastery"),     sigc::mem_fun(*this, &Game::burnMonastery));  

    _actionGroup->add(Gtk::Action::create("Place shield",  "Place shield" ), sigc::mem_fun(*this, &Game::placeShield)); 
    _actionGroup->add(Gtk::Action::create("Remove shield", "Remove shield"), sigc::mem_fun(*this, &Game::removeShield)); 

    _actionGroup->add(Gtk::Action::create("Remove ruin", "Remove ruin"), sigc::mem_fun(*this, &Game::removeRuin)); 
    _actionGroup->add(Gtk::Action::create("Reveal ruin", "Reveal ruin"), sigc::mem_fun(*this, &Game::revealRuin));
	
    

    _UIManager = Gtk::UIManager::create();
    _UIManager->insert_action_group(_actionGroup);
    
    Glib::ustring ui_info = 
        "<ui>"
        "  <popup name='Space'>"
        "    <menuitem action='Get info'/>"
        "    <separator/>"
        "    <menu action='Add an enemy'>"
        "      <menuitem action='Orcs'/>"
        "      <menuitem action='Draconum'/>"
        "      <menuitem action='Keep'/>"
        "      <menuitem action='City'/>"
        "      <menuitem action='Mage Tower'/>"
        "      <menuitem action='Dungeon'/>"
        "    </menu>"
        "    <separator/>"
        "    <menuitem action='Gain mana'/>"
        "    <menuitem action='Gain crystal'/>"
        "    <menuitem action='Heal from hand'/>"
        "    <menuitem action='Heal from discard pile'/>"
        "    <separator/>"
        "    <menuitem action='Engage enemies'/>"
        "    <menuitem action='Retreat'/>"
        "    <menuitem action='Enter site'/>"
        "    <menuitem action='Recruit'/>"
        "    <menuitem action='Heal'/>"
        "    <menuitem action='Train at monastery'/>"
        "    <menuitem action='Buy spell'/>"
        "    <menuitem action='Buy artifact'/>"
        "    <menuitem action='Buy advanced action'/>"
        "    <menuitem action='Buy advanced action from top of deck'/>"
        "    <menuitem action='Add one elite'/>"
        "    <separator/>"
        "    <separator/>"
        "    <menuitem action='Plunder'/>"
        "    <menuitem action='Burn monastery'/>"
        "    <separator/>"
        "    <menuitem action='Place shield'/>"
        "    <menuitem action='Remove shield'/>"
        "    <menuitem action='Remove ruin'/>"
        "    <menuitem action='Reveal ruin'/>"
		"  </popup>"
        "</ui>";
    try {  
        _UIManager->add_ui_from_string(ui_info);
    }
    catch(const Glib::Error& ex) {
        cout << "building menus failed: " <<  ex.what();
    }

    _menuPopup = dynamic_cast<Gtk::Menu*>(_UIManager->get_widget("/Space")); 
    _menuPopup->show_all();
    
    Gtk::MenuItem* orcItem       = dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Space/Add an enemy/Orcs"));
    Gtk::MenuItem* draconumItem  = dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Space/Add an enemy/Draconum"));
    Gtk::MenuItem* keepItem      = dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Space/Add an enemy/Keep"));
    Gtk::MenuItem* cityItem      = dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Space/Add an enemy/City"));
    Gtk::MenuItem* magetowerItem = dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Space/Add an enemy/Mage Tower"));
    Gtk::MenuItem* dungeonItem   = dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Space/Add an enemy/Dungeon"));
    Glib::RefPtr<Gdk::Pixmap> pmap;
    Glib::RefPtr<Gdk::Bitmap> bmap;

    Glib::RefPtr<Gdk::Pixbuf> tinyOrc       = _enemyDecks.at(Enemy::ORCS)      .back().getChildPixbuf()->scale_simple(30, 30, Gdk::INTERP_HYPER);
    Glib::RefPtr<Gdk::Pixbuf> tinyDrac      = _enemyDecks.at(Enemy::DRACONUM)  .back().getChildPixbuf()->scale_simple(30, 30, Gdk::INTERP_HYPER);
    Glib::RefPtr<Gdk::Pixbuf> tinyKeep      = _enemyDecks.at(Enemy::KEEP)      .back().getChildPixbuf()->scale_simple(30, 30, Gdk::INTERP_HYPER);
    Glib::RefPtr<Gdk::Pixbuf> tinyCity      = _enemyDecks.at(Enemy::CITY)      .back().getChildPixbuf()->scale_simple(30, 30, Gdk::INTERP_HYPER);
    Glib::RefPtr<Gdk::Pixbuf> tinyMagetower = _enemyDecks.at(Enemy::MAGE_TOWER).back().getChildPixbuf()->scale_simple(30, 30, Gdk::INTERP_HYPER);
    Glib::RefPtr<Gdk::Pixbuf> tinyDungeon   = _enemyDecks.at(Enemy::DUNGEON)   .back().getChildPixbuf()->scale_simple(30, 30, Gdk::INTERP_HYPER);
   
    tinyOrc->render_pixmap_and_mask(pmap, bmap, 255);
    orcItem->remove();
    orcItem->add_pixmap( pmap, bmap);
    
    tinyDrac->render_pixmap_and_mask(pmap, bmap, 255);
    draconumItem->remove();
    draconumItem->add_pixmap( pmap, bmap);

    tinyKeep->render_pixmap_and_mask(pmap, bmap, 255);
    keepItem->remove();
    keepItem->add_pixmap( pmap, bmap);
    
    tinyCity->render_pixmap_and_mask(pmap, bmap, 255);
    cityItem->remove();
    cityItem->add_pixmap( pmap, bmap);

    tinyMagetower->render_pixmap_and_mask(pmap, bmap, 255);
    magetowerItem->remove();
    magetowerItem->add_pixmap( pmap, bmap);
    
    tinyDungeon->render_pixmap_and_mask(pmap, bmap, 255);
    dungeonItem->remove();
    dungeonItem->add_pixmap( pmap, bmap);
    
	_tacticActionGroup = Gtk::ActionGroup::create("Tactic");
	_tacticActionGroup->add(Gtk::Action::create("Long Night", "Use Long Night"), sigc::mem_fun(*this, &Game::longNight));
	_tacticActionGroup->add(Gtk::Action::create("Midnight Meditation", "Use Midnight Meditation"), sigc::mem_fun(*this, &Game::midnightMeditation));
	_tacticActionGroup->add(Gtk::Action::create("Sparing Power give", "Give one card to Sparing Power"),    sigc::mem_fun(_handViewer, &HandViewer::SparingPowerGiveOne));
	_tacticActionGroup->add(Gtk::Action::create("Sparing Power take", "Take all cards from Sparing Power"), sigc::mem_fun(*this, &Game::SparingPowerTakeAll));

    _tacticUIManager = Gtk::UIManager::create();
	_tacticUIManager->insert_action_group(_tacticActionGroup);

	Glib::ustring tactic_ui_info = 
		"<ui>"
        "  <popup name='Tactic'>"
        "    <menuitem action='Long Night'/>"
		"    <menuitem action='Midnight Meditation'/>"
        "    <menuitem action='Sparing Power give'/>"
		"    <menuitem action='Sparing Power take'/>"
		"  </popup>"
        "</ui>";
    try {  
        _tacticUIManager->add_ui_from_string(tactic_ui_info);
	}
	catch (const Glib::Error& ex) {
        cout << "building menus failed: " <<  ex.what();
    }
	_tacticMenuPopup = dynamic_cast<Gtk::Menu*>(_tacticUIManager->get_widget("/Tactic")); 
	_tacticEventBox.signal_button_press_event().connect(sigc::mem_fun(*this, &Game::on_tactic_right_click));

	_handActionGroup = Gtk::ActionGroup::create("Hand");
	_handActionGroup->add(Gtk::Action::create("Meditation", "Cast Meditation"), sigc::mem_fun(_handViewer, &HandViewer::meditation));
	_handUIManager = Gtk::UIManager::create();
	_handUIManager->insert_action_group(_handActionGroup);
	Glib::ustring hand_ui_info = 
		"<ui>"
        "  <popup name='Hand'>"
		"    <menuitem action='Meditation'/>"
		"  </popup>"
        "</ui>";
    try {  
		_handUIManager->add_ui_from_string(hand_ui_info); 
	}
	catch (const Glib::Error& ex) {
        cout << "building menus failed: " <<  ex.what();
	}

	_handMenuPopup = dynamic_cast<Gtk::Menu*>(_handUIManager->get_widget("/Hand")); 
	for (auto i = _handViewer.hand._cardButtonPtrs.begin(); i != _handViewer.hand._cardButtonPtrs.end(); ++i) {
		//(*i)->signal_button_press_event().connect(sigc::mem_fun(**i, &CardButton::sendID_withEvent));
		//(*i)->IDsignal_withEvent.connect(sigc::mem_fun(*this, &Game::on_hand_card_right_click));
		(*i)->selfSignal.connect(sigc::mem_fun(*this, &Game::modifyCardMenu)); 
		(*i)->meditationSignal.connect(sigc::mem_fun(_handViewer, &HandViewer::meditation)); 
		(*i)->bannerSignal.connect(sigc::mem_fun(*this, &Game::attachBannerToUnit)); 
		(*i)->bannerToBondedSignal.connect(sigc::mem_fun(*this, &Game::attachBannerToBondedUnit)); 
	}

	_retinue.unit0.bannerSignal.connect(sigc::mem_fun(*this, &Game::removeBannerFromUnit));
	_retinue.unit1.bannerSignal.connect(sigc::mem_fun(*this, &Game::removeBannerFromUnit));
	_retinue.unit2.bannerSignal.connect(sigc::mem_fun(*this, &Game::removeBannerFromUnit));
	_retinue.unit3.bannerSignal.connect(sigc::mem_fun(*this, &Game::removeBannerFromUnit));
	_retinue.unit4.bannerSignal.connect(sigc::mem_fun(*this, &Game::removeBannerFromUnit));
	_retinue.unitBonded.bannerSignal.connect(sigc::mem_fun(*this, &Game::removeBannerFromUnit));

	
	startA = Tile(
        Space(PLAINS), Space(FOREST), 
        Space(LAKE), Space(PLAINS), Space(PLAINS), 
        Space(LAKE), Space(LAKE)
    );
    startA._center._playerHere = true;
    
    countryside01 = Tile(
        Space(FOREST, Space::ORCS), Space(LAKE), 
        Space(FOREST), Space(FOREST, Space::MAGICAL_GLADE), Space(PLAINS, Space::VILLAGE), 
        Space(PLAINS), Space(PLAINS)
    );
    countryside02 = Tile(
        Space(HILLS, Space::ORCS), Space(FOREST, Space::MAGICAL_GLADE), 
        Space(PLAINS), Space(HILLS), Space(PLAINS, Space::VILLAGE), 
        Space(HILLS, Space::CRYSTAL_MINES_GREEN), Space(PLAINS)
    );
    countryside03 = Tile(
        Space(PLAINS), Space(HILLS, Space::KEEP), 
        Space(PLAINS), Space(FOREST), Space(HILLS), 
        Space(PLAINS, Space::VILLAGE), Space(HILLS, Space::CRYSTAL_MINES_WHITE)
    );
    countryside04 = Tile(
        Space(DESERT), Space(DESERT), 
        Space(HILLS, Space::ORCS), Space(DESERT, Space::MAGE_TOWER), Space(MOUNTAIN), 
        Space(PLAINS), Space(PLAINS, Space::VILLAGE)
    );
    countryside05 = Tile(
        Space(FOREST), Space(PLAINS, Space::MONASTERY), 
        Space(FOREST, Space::MAGICAL_GLADE), Space(LAKE), Space(PLAINS, Space::ORCS), 
        Space(FOREST), Space(HILLS, Space::CRYSTAL_MINES_BLUE)
    );
    countryside06 = Tile(
        Space(MOUNTAIN), Space(FOREST), 
        Space(HILLS, Space::MONSTER_DEN), Space(HILLS, Space::CRYSTAL_MINES_RED), Space(PLAINS), 
        Space(HILLS), Space(FOREST, Space::ORCS)
    );
    countryside07 = Tile(
        Space(LAKE), Space(FOREST, Space::ORCS), 
        Space(PLAINS, Space::MONASTERY), Space(SWAMP), Space(FOREST, Space::MAGICAL_GLADE), 
        Space(PLAINS), Space(PLAINS, Space::DUNGEON)
    );
    countryside08 = Tile(
        Space(FOREST, Space::MAGICAL_GLADE), Space(FOREST, Space::ANCIENT_RUINS), 
        Space(FOREST), Space(SWAMP, Space::ORCS), Space(PLAINS), 
        Space(SWAMP), Space(SWAMP, Space::VILLAGE)
    );
    countryside09 = Tile(
        Space(WASTELAND, Space::DUNGEON), Space(MOUNTAIN), 
        Space(PLAINS), Space(MOUNTAIN), Space(WASTELAND, Space::KEEP), 
        Space(WASTELAND, Space::MAGE_TOWER), Space(PLAINS)
    );
    countryside10 = Tile(
        Space(HILLS, Space::MONSTER_DEN), Space(FOREST), 
        Space(HILLS), Space(MOUNTAIN), Space(PLAINS), 
        Space(HILLS, Space::KEEP), Space(HILLS, Space::ANCIENT_RUINS)
    );
    countryside11 = Tile(
        Space(HILLS), Space(LAKE), 
        Space(PLAINS, Space::ANCIENT_RUINS), Space(PLAINS, Space::MAGE_TOWER), Space(LAKE), 
        Space(LAKE), Space(HILLS, Space::ORCS)
    );

    core01 = Tile(
        Space(MOUNTAIN), Space(DESERT, Space::TOMB), 
        Space(HILLS, Space::SPAWNING_GROUNDS), Space(DESERT, Space::MONASTERY), Space(DESERT), 
        Space(HILLS), Space(DESERT)
    );
    core02 = Tile(
        Space(LAKE), Space(SWAMP, Space::ANCIENT_RUINS), 
        Space(FOREST), Space(LAKE), Space(HILLS, Space::CRYSTAL_MINES_GREEN), 
        Space(SWAMP, Space::MAGE_TOWER), Space(SWAMP, Space::DRACONUM)
    );
    core03 = Tile(
        Space(MOUNTAIN), Space(WASTELAND, Space::ANCIENT_RUINS),
        Space(WASTELAND, Space::TOMB), Space(WASTELAND), Space(HILLS, Space::MAGE_TOWER), 
        Space(HILLS, Space::CRYSTAL_MINES_WHITE), Space(WASTELAND)
    );
    core04 = Tile(
        Space(HILLS, Space::CRYSTAL_MINES_BLUE), Space(HILLS), 
        Space(WASTELAND), Space(MOUNTAIN, Space::DRACONUM), Space(WASTELAND, Space::KEEP), 
        Space(WASTELAND, Space::ANCIENT_RUINS), Space(WASTELAND)
    );
    core05 = Tile(
        Space(FOREST, Space::MAGICAL_GLADE), Space(SWAMP, Space::VILLAGE), 
        Space(LAKE), Space(CITY, Space::CITY_GREEN), Space(SWAMP, Space::ORCS), 
        Space(FOREST, Space::ORCS), Space(SWAMP) 
    );
    core06 = Tile(
        Space(FOREST), Space(PLAINS, Space::MONASTERY), 
        Space(MOUNTAIN, Space::DRACONUM), Space(CITY, Space::CITY_BLUE), Space(LAKE), 
        Space(HILLS), Space(LAKE)
    );
    core07 = Tile(
        Space(WASTELAND, Space::SPAWNING_GROUNDS), Space(PLAINS), 
        Space(WASTELAND, Space::KEEP), Space(CITY, Space::CITY_WHITE), Space(FOREST), 
        Space(LAKE), Space(LAKE, Space::DRACONUM)
    );
    core08 = Tile(
        Space(DESERT, Space::ANCIENT_RUINS), Space(HILLS, Space::CRYSTAL_MINES_RED), 
        Space(WASTELAND, Space::DRACONUM), Space(CITY, Space::CITY_RED), Space(DESERT), 
        Space(WASTELAND), Space(DESERT, Space::DRACONUM)
    );

    startA.setChildPixbuf(RootDirectory + "startA.png");
    /*
    countryside01.setChildPixbuf(RootDirectory + "tile1.png");
    countryside02.setChildPixbuf(RootDirectory + "tile2.png");
    countryside03.setChildPixbuf(RootDirectory + "tile3.png");
    countryside04.setChildPixbuf(RootDirectory + "tile4.png");
    countryside05.setChildPixbuf(RootDirectory + "tile5.png");
    countryside06.setChildPixbuf(RootDirectory + "tile6.png");
    countryside07.setChildPixbuf(RootDirectory + "tile7.png");
    countryside08.setChildPixbuf(RootDirectory + "tile8.png");
    countryside09.setChildPixbuf(RootDirectory + "tile9.png");
    countryside10.setChildPixbuf(RootDirectory + "tile10.png");
    countryside11.setChildPixbuf(RootDirectory + "tile11.png");

    core01.setChildPixbuf(RootDirectory + "core1.png");
    core02.setChildPixbuf(RootDirectory + "core2.png");
    core03.setChildPixbuf(RootDirectory + "core3.png");
    core04.setChildPixbuf(RootDirectory + "core4.png");
    //cities:
    core05.setChildPixbuf(RootDirectory + "core5.png");
    core06.setChildPixbuf(RootDirectory + "core6.png");
    core07.setChildPixbuf(RootDirectory + "core7.png");
    core08.setChildPixbuf(RootDirectory + "core8.png");

    */
    
    startA.setChildPixbuf(RootDirectory + "tile_startA.png");

    countryside01.setChildPixbuf(RootDirectory + "tile_countryside_01.png");
    countryside02.setChildPixbuf(RootDirectory + "tile_countryside_02.png");
    countryside03.setChildPixbuf(RootDirectory + "tile_countryside_03.png");
    countryside04.setChildPixbuf(RootDirectory + "tile_countryside_04.png");
    countryside05.setChildPixbuf(RootDirectory + "tile_countryside_05.png");
    countryside06.setChildPixbuf(RootDirectory + "tile_countryside_06.png");
    countryside07.setChildPixbuf(RootDirectory + "tile_countryside_07.png");
    countryside08.setChildPixbuf(RootDirectory + "tile_countryside_08.png");
    countryside09.setChildPixbuf(RootDirectory + "tile_countryside_09.png");
    countryside10.setChildPixbuf(RootDirectory + "tile_countryside_10.png");
    countryside11.setChildPixbuf(RootDirectory + "tile_countryside_11.png");

    core01.setChildPixbuf(RootDirectory + "tile_core_01.png");
    core02.setChildPixbuf(RootDirectory + "tile_core_02.png");
    core03.setChildPixbuf(RootDirectory + "tile_core_03.png");
    core04.setChildPixbuf(RootDirectory + "tile_core_04.png");
    core05.setChildPixbuf(RootDirectory + "tile_core_05.png");
    core06.setChildPixbuf(RootDirectory + "tile_core_06.png");
    core07.setChildPixbuf(RootDirectory + "tile_core_07.png");
    core08.setChildPixbuf(RootDirectory + "tile_core_08.png");
        
    coreTileDeck.push_back(core05);
    coreTileDeck.push_back(core06);
    coreTileDeck.push_back(core07);
    coreTileDeck.push_back(core08);
    random_shuffle(coreTileDeck.begin(), coreTileDeck.end()); 
    coreTileDeck.resize(2); //two city tiles gone for good
    
    tileDrawDeck = coreTileDeck;
    coreTileDeck.clear();

    coreTileDeck.push_back(core01);
    coreTileDeck.push_back(core02);
    coreTileDeck.push_back(core03);
    coreTileDeck.push_back(core04);
    random_shuffle(coreTileDeck.begin(), coreTileDeck.end()); 

    tileDrawDeck.push_back(coreTileDeck.back());
    coreTileDeck.pop_back();
    tileDrawDeck.push_back(coreTileDeck.back());
    coreTileDeck.pop_back();

    random_shuffle(tileDrawDeck.begin(), tileDrawDeck.end()); 

    countrysideTileDeck.push_back(countryside01);
    countrysideTileDeck.push_back(countryside02);
    countrysideTileDeck.push_back(countryside03);
    countrysideTileDeck.push_back(countryside04);
    countrysideTileDeck.push_back(countryside05);
    countrysideTileDeck.push_back(countryside06);
    countrysideTileDeck.push_back(countryside07);
    countrysideTileDeck.push_back(countryside08);
    countrysideTileDeck.push_back(countryside09);
    countrysideTileDeck.push_back(countryside10);
    countrysideTileDeck.push_back(countryside11);
    random_shuffle(countrysideTileDeck.begin(), countrysideTileDeck.end()); 
    while(countrysideTileDeck.size() > 4) {
        tileDrawDeck.push_back(countrysideTileDeck.back());
        countrysideTileDeck.pop_back();
    }
    
    cityEnemyGuides[Space::CITY_BLUE][5].push_back(Enemy::MAGE_TOWER); 
    cityEnemyGuides[Space::CITY_BLUE][5].push_back(Enemy::MAGE_TOWER); 
    cityEnemyGuides[Space::CITY_BLUE][5].push_back(Enemy::CITY); 

    cityEnemyGuides[Space::CITY_BLUE][8].push_back(Enemy::MAGE_TOWER); 
    cityEnemyGuides[Space::CITY_BLUE][8].push_back(Enemy::MAGE_TOWER); 
    cityEnemyGuides[Space::CITY_BLUE][8].push_back(Enemy::CITY); 
    cityEnemyGuides[Space::CITY_BLUE][8].push_back(Enemy::CITY); 

    cityEnemyGuides[Space::CITY_GREEN][5].push_back(Enemy::DUNGEON); 
    cityEnemyGuides[Space::CITY_GREEN][5].push_back(Enemy::DUNGEON); 
    cityEnemyGuides[Space::CITY_GREEN][5].push_back(Enemy::CITY); 
                    
    cityEnemyGuides[Space::CITY_GREEN][8].push_back(Enemy::DUNGEON); 
    cityEnemyGuides[Space::CITY_GREEN][8].push_back(Enemy::DUNGEON); 
    cityEnemyGuides[Space::CITY_GREEN][8].push_back(Enemy::CITY); 
    cityEnemyGuides[Space::CITY_GREEN][8].push_back(Enemy::CITY); 

    cityEnemyGuides[Space::CITY_RED][5].push_back(Enemy::DUNGEON); 
    cityEnemyGuides[Space::CITY_RED][5].push_back(Enemy::MAGE_TOWER); 
    cityEnemyGuides[Space::CITY_RED][5].push_back(Enemy::CITY); 
                    
    cityEnemyGuides[Space::CITY_RED][8].push_back(Enemy::DUNGEON); 
    cityEnemyGuides[Space::CITY_RED][8].push_back(Enemy::MAGE_TOWER); 
    cityEnemyGuides[Space::CITY_RED][8].push_back(Enemy::CITY); 
    cityEnemyGuides[Space::CITY_RED][8].push_back(Enemy::CITY); 

    cityEnemyGuides[Space::CITY_WHITE][5].push_back(Enemy::KEEP); 
    cityEnemyGuides[Space::CITY_WHITE][5].push_back(Enemy::CITY); 
    cityEnemyGuides[Space::CITY_WHITE][5].push_back(Enemy::CITY); 
                    
    cityEnemyGuides[Space::CITY_WHITE][8].push_back(Enemy::KEEP); 
    cityEnemyGuides[Space::CITY_WHITE][8].push_back(Enemy::CITY); 
    cityEnemyGuides[Space::CITY_WHITE][8].push_back(Enemy::CITY); 
    cityEnemyGuides[Space::CITY_WHITE][8].push_back(Enemy::CITY); 
           
    playerOnePixbuf = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, HEX_SIDE_LENGTH, HEX_SIDE_LENGTH * 1.25); //placeholder
    playerOnePixbuf->fill(0x00000000);
    
    // Initial layout: player already at start
    Coordinate wanderer(origin);
    layNewTile(startA, origin);
   
    wanderer = _board.northWestOf(wanderer);
    wanderer = _board.northWestOf(wanderer);
    layNewTile(tileDrawDeck.back(), wanderer); 
    tileDrawDeck.pop_back();
    wanderer = _board.eastOf(wanderer);
    wanderer = _board.eastOf(wanderer);
    layNewTile(tileDrawDeck.back(), wanderer); 
    tileDrawDeck.pop_back();
    screenImage.set(screen);
    
    get_settings()->property_gtk_tooltip_timeout() = 1500;

	
    _endTurnButton      .add_label("Next Turn");
    _showHandButton     .add_label("Show Hand");
    _showOfferButton    .add_label("Show Offer"); 
    _endRoundButton     .add_label("End Round"); 
    //_awardSkillButton   .add_label("Earn New Skill");
    _awardArtifactButton.add_label("Open Artifact Chest");

    _endTurnSignal = _endTurnButton.signal_clicked().connect(sigc::mem_fun(*this, &Game::endTurn));  //will be disconnected on final turn
    _endRoundButton.signal_clicked().connect(sigc::mem_fun(*this, &Game::endRound));
    _showOfferButton.signal_clicked().connect(sigc::mem_fun(_offerWindow, &OfferWindow::openOfferWindow));
    _showHandButton.signal_clicked().connect(sigc::mem_fun(*this, &Game::toggleHandViewer));
    //_awardSkillButton.signal_clicked().connect(sigc::mem_fun(_skillTokenDeck, &SkillTokenDeckWindow::openForSelection)); 
    //_awardSkillButton.signal_clicked().connect(sigc::mem_fun(_handViewer, &Gtk::Window::hide)); 
    //_awardSkillButton.signal_clicked().connect(sigc::mem_fun(_enemyViewer, &Gtk::Window::hide)); 
    _vbuttonBox1.set_layout(Gtk::ButtonBoxStyle::BUTTONBOX_SPREAD);
    _vbuttonBox1.set_spacing(0); 
    _vbuttonBox1.pack_start(_showHandButton  ,    Gtk::PACK_EXPAND_WIDGET,  5);
    _vbuttonBox1.pack_start(_showOfferButton ,    Gtk::PACK_EXPAND_WIDGET,  5);
    _vbuttonBox1.pack_start(_awardArtifactButton, Gtk::PACK_EXPAND_WIDGET,  5);
    //_vbuttonBox1.pack_start(_awardSkillButton,    Gtk::PACK_EXPAND_WIDGET,  5);
    _vbuttonBox1.pack_start(_endTurnButton   ,    Gtk::PACK_EXPAND_PADDING, 5);
    _vbuttonBox1.pack_start(_endRoundButton,      Gtk::PACK_EXPAND_PADDING, 5);
    _vbuttonBox1.foreach(sigc::bind<bool>(sigc::mem_fun(&Gtk::Button::set_can_focus), false));

	_dummyPlayer.gameEndTurnButtonPtr = &_endTurnButton;

    _fame._advanceButton.signal_clicked().connect(sigc::mem_fun(*this, &Game::levelUpCheck));
    _awardArtifactButton.signal_clicked().connect(sigc::mem_fun(*this, &Game::openArtifactChest));
    _console.modify_bg(Gtk::STATE_NORMAL, Gdk::Color("#000000"));
	_upperRightTable.set_border_width(0);

    _mainTable.set_homogeneous(false);
    _mainTable.resize(4, 5);
    size_t SCREEN_WIDTH  = 1920 - (22 * 2);
    size_t SCREEN_HEIGHT = 1080 - (22 * 2);
    _mainTable.set_size_request(SCREEN_WIDTH, SCREEN_HEIGHT);
       

    /** SIZE REQUESTS **/   
    Gtk::ShadowType myShadow = Gtk::SHADOW_IN;
    _console.set_size_request          (1130, 45);   
    mainScrollWindow.set_size_request  (1150, 700);                   
    _retinue.box.set_size_request      (-1, 250);   
    _sliderHBox.set_size_request       (280, 200);
    _upperRightTable.set_size_request  (355, 349); 
    _skillTokenDeck.playerVButtonBox.set_size_request (200, 395);                                 
    _statHbox.set_size_request         (180, -1);                    
    _vbuttonBox1.set_size_request      (200, 200);    
    Gtk::Frame* _sliderArmorFrame     = Gtk::manage(new Gtk::Frame);
    Gtk::Frame* _upperRightTableFrame = Gtk::manage(new Gtk::Frame);
    Gtk::Frame* _retinueFrame         = Gtk::manage(new Gtk::Frame);
    Gtk::Frame* _vbuttonBox1Frame     = Gtk::manage(new Gtk::Frame);
    Gtk::Frame* _skillTokenFrame      = Gtk::manage(new Gtk::Frame);
    Gtk::Frame* _statHboxFrame        = Gtk::manage(new Gtk::Frame);
    
    _sliderArmorFrame->    set_shadow_type(myShadow);
    _upperRightTableFrame->set_shadow_type(myShadow);
    _retinueFrame->        set_shadow_type(Gtk::SHADOW_ETCHED_OUT);
    _vbuttonBox1Frame->    set_shadow_type(myShadow);
    _skillTokenFrame ->    set_shadow_type(myShadow);
    _statHboxFrame   ->    set_shadow_type(myShadow);

    _sliderArmorFrame->    add(_sliderArmorTable);
    _upperRightTableFrame->add(_upperRightTable);
    _retinueFrame->        add(_retinue.box);
    _vbuttonBox1Frame->    add(_vbuttonBox1);
    _skillTokenFrame->     add(_skillTokenDeck.playerVButtonBox);
    _statHboxFrame->       add(_statHbox);
   // _skillbuttonFrame->set_border_width(0);
    //_skillbuttonFrame->add(_skillTokenDeck.playerVButtonBox);
    
    _mainTable.attach(_console._hBox,          0, 2, 0, 1, Gtk::FILL,   Gtk::SHRINK);
    _mainTable.attach(mainScrollWindow,        0, 2, 1, 3, Gtk::SHRINK, Gtk::FILL);  
    _mainTable.attach(*_retinueFrame,          0, 2, 3, 4, Gtk::FILL,   Gtk::FILL); 
    _mainTable.attach(*_sliderArmorFrame,      2, 3, 0, 2, Gtk::SHRINK, Gtk::SHRINK);
    _mainTable.attach(*_upperRightTableFrame,  3, 5, 0, 2, (Gtk::SHRINK | Gtk::EXPAND), (Gtk::SHRINK | Gtk::EXPAND), 0, 0);
    _mainTable.attach(*_skillTokenFrame,       3, 4, 2, 4, Gtk::SHRINK,   Gtk::SHRINK);
    _mainTable.attach(*_statHboxFrame,         4, 5, 2, 3, Gtk::SHRINK, Gtk::SHRINK);
    _mainTable.attach(*_vbuttonBox1Frame,      4, 5, 3, 4, Gtk::SHRINK, Gtk::SHRINK);
    //_handViewer.miniHandBox.set_size_request(680, 220);   
    //_mainTable.attach(_handViewer.miniHandBox, 0, 1, 3, 4, Gtk::SHRINK, Gtk::SHRINK); 
   // _handViewer.updateMinihand(&GdkEventAny());
    _handViewer.set_keep_above();
    _handViewer.set_transient_for(*this);
    _offerWindow.set_transient_for(*this);
	_offerWindow.monasteryActionsWindow.set_transient_for(*this);
    _mainTable.set_spacings(0);
	set_border_width(22);

	add(_mainTable);
	/*
	testActionGroup = Gtk::ActionGroup::create("Main menu");
	testActionGroup->add(Gtk::Action::create("File", "File"), sigc::mem_fun(*this, &Game::testPrintFile));
	testActionGroup->add(Gtk::Action::create("Edit", "Edit"), sigc::mem_fun(*this, &Game::testPrintEdit));
	testActionGroup->add(Gtk::Action::create("View", "View"), sigc::mem_fun(*this, &Game::testPrintView));

	testUIManager = Gtk::UIManager::create();
	testUIManager->insert_action_group(testActionGroup);
	Glib::ustring testUI_info = 
	"<ui>"
    "  <menubar name='Main menu'>"
    "    <menu action='File'>"
    "    </menu>"
    "    <menu action='Edit'>"
    "    </menu>"
	"    <menu action='View'>"
	"    </menu>"
    "  </menubar>"
	"</ui>";
	try {
		testUIManager->add_ui_from_string(testUI_info);
	}
	catch(const Glib::Error& ex) {
	  std::cerr << "building menus failed: " <<  ex.what();
	}
	
	testMenuBar = dynamic_cast<Gtk::MenuBar*>(testUIManager->get_widget("/Main menu")); 
	
	testMenuBar->show_all();
	mainMainOuterBox.pack_start(*testMenuBar);
	mainMainOuterBox.pack_start(_mainTable);
	add(mainMainOuterBox);
	*/
    
	maximize();
	
	_handViewer.selectPlayerPrompt();
	switch (_handViewer._knightName) {
    case NOROWAS:
		playerOnePixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "playerTokenNorowas.png")->scale_simple(SCALED_PLAYER_TOKEN_DIAMETER, SCALED_PLAYER_TOKEN_DIAMETER, Gdk::INTERP_HYPER); 
        playerOneShieldPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "shieldNorowas.png")->scale_simple(33, 44, Gdk::INTERP_HYPER); 
        break;
    case GOLDYX:
        playerOnePixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "playerTokenGoldyx.png")->scale_simple(SCALED_PLAYER_TOKEN_DIAMETER, SCALED_PLAYER_TOKEN_DIAMETER, Gdk::INTERP_HYPER); 
        playerOneShieldPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "shieldGoldyx.png")->scale_simple(33, 44, Gdk::INTERP_HYPER); 
        break;
    case ARYTHEA:
        playerOnePixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "playerTokenArythea.png")->scale_simple(SCALED_PLAYER_TOKEN_DIAMETER, SCALED_PLAYER_TOKEN_DIAMETER, Gdk::INTERP_HYPER);
        playerOneShieldPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "shieldArythea.png")->scale_simple(33, 44, Gdk::INTERP_HYPER); 
        break;
    case TOVAK:
        playerOnePixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "playerTokenTovak.png")->scale_simple(SCALED_PLAYER_TOKEN_DIAMETER, SCALED_PLAYER_TOKEN_DIAMETER, Gdk::INTERP_HYPER);
        playerOneShieldPixbuf = Gdk::Pixbuf::create_from_file(RootDirectory + "shieldTovak.png")->scale_simple(33, 44, Gdk::INTERP_HYPER); 
        break;
    }
    _handViewer.set_icon(playerOneShieldPixbuf); 
    _skillTokenDeck.setPlayers(_handViewer._knightName, _dummyPlayer.selectRandomDummyVersus(_handViewer._knightName));
    _dummyPlayer.setParentWindow(*this);
    renderPlayer(origin);
    show_all();

   
    mainScrollWindow.get_hadjustment()->set_value(origin.x - HEX_WIDTH * 2);
    mainScrollWindow.get_vadjustment()->set_value(mainScrollWindow.get_vadjustment()->get_upper() - mainScrollWindow.get_vadjustment()->get_page_size());



    _roundNumber = 1;
    if (isDay())
        _tacticWindow.offerDayTactic();
    else
        _tacticWindow.offerNightTactic();

    if (_tacticWindow.dummyGoesFirst())
        _dummyPlayer.takeTurn();

	if (_tacticWindow.getCurrentTactic() == 5)
		_handViewer.drawCards(2);

	ostringstream oss;
	oss << "Dummy cards remaining: " << _dummyPlayer.getCardsRemaining();
	_console.print(oss.str());
    
	_retinue.unit1.hide(); 
    _retinue.unit2.hide();
    _retinue.unit3.hide();
    _retinue.unit4.hide();
    _retinue.unitBonded.hide();
    _skillTokenDeck.playerSkillButton0.set_sensitive(false);
    _skillTokenDeck.playerSkillButton1.set_sensitive(false);
    _skillTokenDeck.playerSkillButton2.set_sensitive(false);
    _skillTokenDeck.playerSkillButton3.set_sensitive(false);
    _skillTokenDeck.playerSkillButton4.set_sensitive(false);

	_skillTokenDeck.bondsOfLoyaltyAwardedSignal.connect(sigc::mem_fun(*this, &Game::makeBondsOfLoyaltyUsable));
	_skillTokenDeck.recruitWithBondsSignal.connect(sigc::mem_fun(*this, &Game::recruitBondedUnit));

	//screenEventBox.signal_expose_event().connect(sigc::mem_fun(*this, &Game::onExpose));

}



unsigned int Game::getMovementCost(Space s) {
	//TODO: moveBonus flags here, reducing costs
    switch (s.getTerrain()) {
    case UNEXPLORED:
    case CITY:
    case PLAINS:
        return 2;
    case HILLS:
        return 3;
    case FOREST:
        return (isDay() ? 3 : 5);
    case WASTELAND:
        return 4;
    case DESERT:
        return (isDay() ? 5 : 3);
    case SWAMP:
        return 5;
    default:
        return 0;
    }
};

bool Game::showSpaceInfoTooltip(int x, int y, bool, const Glib::RefPtr<Gtk::Tooltip>& tooltip) {
    
    for (Board::iterator i = _board.begin(); i != _board.end(); ++i) {
        if (Coordinate(x, y).isWithinHexAt(i->first)) {
            string info = i->second.onMouseOver();
            if (i->second.canBeMovedThrough()) {
                ostringstream oss; 
                oss << getMovementCost(i->second);
                info.replace(info.find('X'), 1, oss.str());
            }
			if (i->second.getTerrain() == UNEXPLORED) {
				ostringstream oss; 
				if (tileDrawDeck.size() > 4) {
					oss << "\nCountryside tiles remaining: " << tileDrawDeck.size() - 4;
				}
				else if (tileDrawDeck.size() > 0) {
					oss << "\nCore tiles remaining: " << tileDrawDeck.size();
				}
				else if (countrysideTileDeck.size() > 0 || coreTileDeck.size() > 0) {
					oss << "\nUnused tiles remaining: " << countrysideTileDeck.size() + coreTileDeck.size();
				}
				else {
					oss << "\nAll tiles have been placed";
				}
				info.append(oss.str());
			}
						
            //tooltip->set_markup("<span font_family = \"Sakkal Majalla\" size=\"10240\">" + info + "</span>");
			if (i->second.getEnemies().size() > 0 || i->second.hasRuin()) {
                int diameter = 70;
				Glib::RefPtr<Gdk::Pixbuf> army = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, diameter * (i->second.getEnemies().size() + i->second.hasRuin()), diameter);
                army->fill(0x00000000);
                for (int j = 0; j < i->second.getEnemies().size(); ++j) {
                    i->second.getEnemies().at(j).getChildPixbuf()->scale_simple(diameter, diameter, Gdk::INTERP_HYPER)->
                              composite(army, diameter * j, 0, diameter, diameter, diameter * j, 0, 1, 1, Gdk::INTERP_HYPER, 255);
                }
				if (i->second.hasRuin()) {
					i->second.getRuin().getChildPixbuf()->scale_simple(diameter, diameter, Gdk::INTERP_HYPER)->
						composite(army, army->get_width() - diameter, 0, diameter, diameter, army->get_width() - diameter, 0, 1, 1, Gdk::INTERP_HYPER, 255);
				}
                tooltip->set_icon(army);
            }
            tooltip->set_text(info); 
            return true;
        }
    }
    tooltip->set_text("No space found");
    return false;
}

void Game::layNewTile(const Tile& _tile, const Coordinate& _outerUnexploredHex)  {
    Coordinate target = _board.centerHexforAnyHex(_outerUnexploredHex);
    _board[target]                     = _tile._center;
    _board[_board.northWestOf(target)] = _tile._northwest;
    _board[_board.northEastOf(target)] = _tile._northeast;
    _board[_board.southWestOf(target)] = _tile._southwest;   
    _board[_board.southEastOf(target)] = _tile._southeast;
    _board[_board.eastOf(target)]      = _tile._east;
    _board[_board.westOf(target)]      = _tile._west;
    
    vector<Space*> spacePtr;
    spacePtr.push_back(&_board[target]);
    spacePtr.push_back(&_board[_board.northWestOf(target)]);
    spacePtr.push_back(&_board[_board.northEastOf(target)]);
    spacePtr.push_back(&_board[_board.southWestOf(target)]);
    spacePtr.push_back(&_board[_board.southEastOf(target)]);
    spacePtr.push_back(&_board[_board.eastOf(target)]     );
    spacePtr.push_back(&_board[_board.westOf(target)]     );

    for (vector<Space*>::iterator i = spacePtr.begin(); i != spacePtr.end(); ++i) {
        if ((*i)->getFeature() == Space::ANCIENT_RUINS) {
            (*i)->addRuin(_ruinDeck.drawOne());
            if (isDay()) {
                (*i)->flipRuinUp();
            }
        }
        if ((*i)->getFeature() == Space::ORCS) {
            Enemy newEnemy = _enemyDecks.drawOne(Enemy::ORCS);
            newEnemy.flipUp();
            (*i)->addEnemy(newEnemy);
        }
        if ((*i)->getFeature() == Space::DRACONUM) {
            Enemy newEnemy = _enemyDecks.drawOne(Enemy::DRACONUM);
            newEnemy.flipUp();
            (*i)->addEnemy(newEnemy);
        }
        if ((*i)->getFeature() == Space::MONSTER_DEN) {
            Enemy newEnemy = _enemyDecks.drawOne(Enemy::DUNGEON);
            (*i)->addEnemy(newEnemy);
        }
        if ((*i)->getFeature() == Space::SPAWNING_GROUNDS) {
            while ((*i)->getEnemies().size() < 2) {
                Enemy newEnemy = _enemyDecks.drawOne(Enemy::DUNGEON);
                (*i)->addEnemy(newEnemy);
            }
        }
        if ((*i)->getFeature() == Space::MONASTERY) {
            _offerWindow.monasteryActionOffer.monasteryFound();
            _offerWindow.monasteryActionOffer.addMoreToOffer(1);
        }
        if ((*i)->getFeature() == Space::MAGE_TOWER) {
            Enemy newEnemy = _enemyDecks.drawOne(Enemy::MAGE_TOWER);
            (*i)->addEnemy(newEnemy);
        }
        if ((*i)->getFeature() == Space::KEEP) {
            Enemy newEnemy = _enemyDecks.drawOne(Enemy::KEEP);
            (*i)->addEnemy(newEnemy);
        }
        if ((*i)->getTerrain() == CITY) {
            list<Enemy::Type> enemyList;
            size_t cityCount = 0;
            size_t cityLevel = 0;
            for (auto j = _board.begin(); j != _board.end(); ++j) {
                if (j->second.getTerrain() == CITY) {
                    ++cityCount; 
                }
            }
            if (cityCount == 2)
                cityLevel = 8;
            else
                cityLevel = 5;
            enemyList = cityEnemyGuides[(*i)->getFeature()][cityLevel];
            while (!enemyList.empty()) {
                (*i)->addEnemy(_enemyDecks.drawOne(enemyList.back()));
                enemyList.pop_back();
            }
        }
    }
    _tilemap.insert(pair<Coordinate, Tile> (_board.tileAnchorforAnyHex(_outerUnexploredHex), _tile)); 
    checkPlayerAdjacencyForEnemyReveals();
    renderTileOnScreen(_tile, _board.tileAnchorforAnyHex(_outerUnexploredHex)); 
    

        /*
        Countryside tiles (green tile back) can only be placed
        such that they will be adjacent to at least two other tiles,
        or adjacent to a tile that borders at least two other tiles.
        ** Core tiles (brown tile back) can only be placed such that
        they will be adjacent to at least two other tiles.
        If there are no tiles left in the Map Tile deck when
        a player attempts to explore, he may use a random
        Countryside tile removed from the game during Setup.
        If all Countryside tiles have been placed, he can use
        removed non-City Core tiles instead. Tiles explored this
        way can only be placed such that they are adjacent to at
        least three other tiles (to fill up holes). If there are no tiles
        in the box, no more tiles can be explored. 

        This may just have to be done manually for now.

        Can be done, just need to write a function that can determine how many adjacent tiles are next to a given hex (most is two).
        You can eliminate 3 neighboring hexes per hex, as they would just belong to that hexes home tile.
        Might be easy: Take NW tile for example: you don't count its own tile's hexes, obviously.
        If it has exactly ONE or TWO other adjacent hexes, it's got one adjacent TILE.
        If it has all three (so no neighboring unexploreds at all), it has two adjacent tiles.
        And zero hexes is zero tiles.
        Can stop when adjacent tile count is three (max needed to place tile)

        */

}

bool Game::on_right_click(GdkEventButton* e) {
    _menuPopup->hide_all();
    if (e->button != 3) 
        return false;

    Coordinate clickedHex(e->x, e->y);
    Coordinate targetHex;
    if (!convertToValidHex(clickedHex, targetHex)) {
        _menuPopup->popdown();
        return false;
    }
    rightClickedHex = targetHex;
    _enemyViewer.hide();

    if (_board[rightClickedHex].getTerrain() == UNEXPLORED ||
       ( _board[rightClickedHex].getFeature() == Space::NO_FEATURE && (!_board.playerIsNextTo(rightClickedHex) || !_board[rightClickedHex].canBeMovedThrough()) && _board[rightClickedHex].getEnemies().empty()))
    {
        _menuPopup->popdown();
        return false;
    }
    _menuPopup->show();
    _menuPopup->popup(e->button, e->time);

    if (!_board[rightClickedHex].getEnemies().empty()) {
        _UIManager->get_widget("/Space/Engage enemies")->show_all();
    }
    else if (_board.playerIsNextTo(rightClickedHex) && _board[rightClickedHex].canBeMovedThrough()) { 
        _UIManager->get_widget("/Space/Retreat")->show_all(); 
    }

    if (_board[rightClickedHex].getFeature() == Space::NO_FEATURE)
        return true;
    
    _UIManager->get_widget("/Space/Get info")->show_all();
    _UIManager->get_widget("/Space/Add an enemy")->show_all();
	if (_board[rightClickedHex].canBeConquered() || _board[rightClickedHex].getTerrain() == CITY) {
		_UIManager->get_widget("/Space/Place shield")->show_all(); 
	}
	if (_board[rightClickedHex].getShields() > 0) {
		_UIManager->get_widget("/Space/Remove shield")->show_all(); 
	}
	
	if (_board[rightClickedHex].isAdventureSite()) {
        Gtk::MenuItem* siteName = dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Space/Enter site"));
        switch (_board[rightClickedHex].getFeature()) {
        case Space::ANCIENT_RUINS : 
			if (_board[rightClickedHex].hasFaceupRuin()) { 
                _UIManager->get_widget("/Space/Remove ruin")->show_all();
                switch (_board[rightClickedHex].getRuin().getAltarColor()) {
                case BLUE :
                    siteName->set_label("Pay 3 blue mana for 7 fame"); 
                    break;
                case RED :
                    siteName->set_label("Pay 3 red mana for 7 fame");
                    break;
                case GREEN :
                    siteName->set_label("Pay 3 green mana for 7 fame");
                    break;
                case WHITE :
                    siteName->set_label("Pay 3 white mana for 7 fame");
                    break;
                default:
                    siteName->set_label("Enter ruins");
                }
            }
            else if (_board[rightClickedHex].hasFacedownRuin()) {
                _UIManager->get_widget("/Space/Reveal ruin")->show_all();
            }
            break;
        case Space::DUNGEON :
            siteName->set_label("Enter dungeon");
            break;
        case Space::TOMB :
            siteName->set_label("Enter tomb");
            break;
        case Space::SPAWNING_GROUNDS :
            siteName->set_label("Enter spawning grounds");
            break;
        case Space::MONSTER_DEN :
            siteName->set_label("Enter monster den");
            break;
        }
        if (!_board[rightClickedHex].hasFacedownRuin())
            siteName->show_all();
    }

    if (_board[rightClickedHex].getFeature() == Space::VILLAGE) {
        Gtk::MenuItem* _item = dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Space/Heal"));
        _item->set_label("Heal (3 Influence)");
        _UIManager->get_widget("/Space/Recruit")->show_all();
        _UIManager->get_widget("/Space/Heal")   ->show_all();
        _UIManager->get_widget("/Space/Plunder")->show_all();
    }

    if (_board[rightClickedHex].getFeature() == Space::MONASTERY) {
        Gtk::MenuItem* _item = dynamic_cast<Gtk::MenuItem*>(_UIManager->get_widget("/Space/Heal"));
        _item->set_label("Heal (2 Influence)");
        _UIManager->get_widget("/Space/Recruit")           ->show_all();
        _UIManager->get_widget("/Space/Heal")              ->show_all();
        _UIManager->get_widget("/Space/Train at monastery")->show_all();
        _UIManager->get_widget("/Space/Burn monastery")    ->show_all();
    }

    if (_board[rightClickedHex].isConquered()) {
        switch (_board[rightClickedHex].getFeature()) {
        case Space::MAGE_TOWER :
            _UIManager->get_widget("/Space/Recruit")     ->show_all();
            _UIManager->get_widget("/Space/Buy spell")   ->show_all();
            break;                                       
        case Space::KEEP :                               
            _UIManager->get_widget("/Space/Recruit")     ->show_all();
            break;                                       
        case Space::CITY_BLUE :                          
            _UIManager->get_widget("/Space/Buy spell")   ->show_all();
            break;                                       
        case Space::CITY_RED :
            _UIManager->get_widget("/Space/Buy artifact")->show_all();
            break;
        case Space::CITY_GREEN :
            _UIManager->get_widget("/Space/Buy advanced action")                 ->show_all();
            _UIManager->get_widget("/Space/Buy advanced action from top of deck")->show_all();
            break;
        case Space::CITY_WHITE :
            _UIManager->get_widget("/Space/Recruit")      ->show_all();
            _UIManager->get_widget("/Space/Add one elite")->show_all();
            break;
        }
    }
    if (_board[rightClickedHex].getFeature() == Space::MAGICAL_GLADE) {
        _UIManager->get_widget("/Space/Gain mana")              ->show_all();
        _UIManager->get_widget("/Space/Heal from hand")         ->show_all();
        _UIManager->get_widget("/Space/Heal from discard pile") ->show_all();
    }
    if (_board[rightClickedHex].isCrystalMine()) {
        _UIManager->get_widget("/Space/Gain crystal")              ->show_all();
    }

    return true;
}

bool Game::movePlayer(GdkEventButton* e) {
    Coordinate clickedHex(e->x, e->y);
    Coordinate targetHex;
    ostringstream oss;
    if (e->button == 1 && e->type == GdkEventType::GDK_2BUTTON_PRESS) {
        if (!convertToValidHex(clickedHex, targetHex)) {
            oss << "Invalid hex: " << clickedHex << endl;
            //_console.print(oss.str());
            return false; 
        }
        if (!_board.playerIsNextTo(targetHex)) {
            oss << "Select an adjacent space to move into.";
            _console.print(oss.str());
            return false;
        }
      // if (!_board[targetHex].canBeMovedThrough()) {
      //     oss << "That terrain is impassible.";
      //     _console.print(oss.str());
      //     return false;
      // }

      // if (_board[targetHex].hasRampaging()) {
      //     oss << "Rampaging enemies occupy that space.";
      //     _console.print(oss.str());
      //     return false;
      // }

        if (getMovementCost(_board[targetHex]) > _movePoints.get_value_as_int()) {
            int result = getMovementCost(_board[targetHex]) - _movePoints.get_value_as_int();
            oss << "You need " << result << " more Move point" << (result == 1 ? "" : "s") <<  " to enter that space.";
            _console.print(oss.str());
            return false;
        }
        if (_board[targetHex].getTerrain() == UNEXPLORED) {
            Tile newTile;
			if (!tileDrawDeck.empty()) {
				if (tileDrawDeck.size() > 4 && !_board.isValidForCountrysidePlacement(targetHex)) {
					oss << "Too few adjacent tiles for countryside to be explored.";
					_console.print(oss.str());
					return true;
				}
				else if (tileDrawDeck.size() <= 4 && _board.getNumberOfNeighboringTiles(targetHex) < 2) {
					oss << "Too few adjacent tiles for core tile to be laid down.";
					_console.print(oss.str());
					return true;
				}
				newTile = tileDrawDeck.back();
                tileDrawDeck.pop_back();
				if (tileDrawDeck.size() < 4) { 
					_offerWindow.coreTileRevealed(); 
				}
            }
			else if ( !(countrysideTileDeck.empty() && coreTileDeck.empty()) && _board.getNumberOfNeighboringTiles(targetHex) < 3) {
				oss << "Too few adjacent tiles for unused tile to be laid down.";
				_console.print(oss.str());
				return true;
			}
			else if (!countrysideTileDeck.empty()) {
				newTile = countrysideTileDeck.back();
                countrysideTileDeck.pop_back();
            }
            else if (!coreTileDeck.empty()) {
				newTile = coreTileDeck.back();
                coreTileDeck.pop_back();
			}
            else {
                oss << "You have used every tile in the game. There is nowhere left to run.";
                _console.print(oss.str());
                return false;
            }                
            layNewTile(newTile, targetHex);
            _movePoints.decreaseBy(2);
            return true;
        }
        Coordinate source = _board.playerAt();  
        _board.movePlayerTo(targetHex);
        checkPlayerAdjacencyForEnemyReveals();
        renderAndUpdateTileForHex(source);               
        renderAndUpdateTileForHex(targetHex);   
        if (_board.provokesEnemy(source, targetHex))
            _console.print("Enemies provoked.");
        else
            _console.print("");
        _movePoints.decreaseBy(getMovementCost(_board[targetHex]));  

        //can lower rep if assaulting site
        //enemies in space? begin combat
		
        return true;

    }

    return false;
}

/*
Game state info
Player deck
Player hand
Player discard
Dummy deck
Dummy discard
Dummy crystals
Player position
Entire board state (enemys, spaces, tiles)
Player mana
Player crystals
Player fame
Player rep
Points to 0
Player skills
Player retinue
Offers
Day/Night

*/

/*
class TestTree
    :public Gtk::Window
{
    typedef  Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > PictureColumn;
    Gtk::TreeView tree;
    Glib::RefPtr<Gtk::ListStore> _liststore;
    vector<PictureColumn> rowOfPictures;

    Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > singleColumn;
    Gtk::TreeModelColumnRecord record;
    vector<Deed> deck;
    Gtk::ScrolledWindow _scrolledWindow;

public:

    void printSelected() {
        
        if (tree.get_selection()->get_selected())
        //cout << tree.get_selection()->get_selected()->get_value(name) << endl;
        ;
    }
    bool deleteSelected(GdkEventButton* e) {
        if (e->type == GDK_2BUTTON_PRESS && tree.get_selection()->get_selected())
        {
            //Gtk::TreeModel::iterator nuker;
            //nuker = 
           // Gtk::TreeModel::Row row = *(tree.get_selection()->get_selected());
            _liststore->erase(tree.get_selection()->get_selected());
            return true;
        }
        return false;
        
    }

    
    TestTree()
        //: colum
    {
       
        deck.push_back(Deed ("Rage",               Deed::RED,   Deed::BASIC_ACTION));
        deck.push_back(Deed ("Rage",               Deed::RED,   Deed::BASIC_ACTION));
        deck.push_back(Deed ("Determination",      Deed::BLUE,  Deed::BASIC_ACTION));
        deck.push_back(Deed ("Swiftness",          Deed::WHITE, Deed::BASIC_ACTION));
        deck.push_back(Deed ("Swiftness",          Deed::WHITE, Deed::BASIC_ACTION));
        deck.push_back(Deed ("March",              Deed::GREEN, Deed::BASIC_ACTION));
        deck.push_back(Deed ("March",              Deed::GREEN, Deed::BASIC_ACTION));
        deck.push_back(Deed ("Stamina",            Deed::BLUE , Deed::BASIC_ACTION));
        deck.push_back(Deed ("Stamina",            Deed::BLUE , Deed::BASIC_ACTION));
        deck.push_back(Deed ("Tranquility",        Deed::GREEN, Deed::BASIC_ACTION));
        deck.push_back(Deed ("Promise",            Deed::WHITE, Deed::BASIC_ACTION));
        deck.push_back(Deed ("Threaten",           Deed::RED,   Deed::BASIC_ACTION));
        deck.push_back(Deed ("Crystallize",        Deed::BLUE,  Deed::BASIC_ACTION));
        deck.push_back(Deed ("Mana Draw",          Deed::WHITE, Deed::BASIC_ACTION));
        deck.push_back(Deed ("Concentration",      Deed::GREEN, Deed::BASIC_ACTION));
        deck.push_back(Deed ("Improvisation",      Deed::RED,   Deed::BASIC_ACTION));

        rowOfPictures.resize(deck.size()); 
        for (int i = 0; i < deck.size(); ++i) {
            record.add(rowOfPictures[i]);
        }
        
        _liststore = Gtk::ListStore::create(record);
        
        Gtk::TreeModel::iterator it;
        Gtk::TreeRow row;
        it =_liststore->append();
        row = *it; 
        for (int i = 0; i < deck.size(); ++i) {
            
            row[rowOfPictures[i]] = deck[i].getChildPixbuf()->scale_simple(40, 50, Gdk::INTERP_HYPER);
         
            //row[ rowOfPictures[i] ] = deck[i].getChildPixbuf()->scale_simple(40, 50, Gdk::INTERP_HYPER);
            //record.add(rowOfPictures[i]);
        }
       
       
     

      //  for (int i = 0; i < deck.size(); ++i) {
      //      it =_liststore->append();
      //      row = *it; 
      //      row[ rowOfPictures[i] ] = deck[i].getChildPixbuf()->scale_simple(40, 50, Gdk::INTERP_HYPER);
      //      //Gtk::Button* temp = (new Gtk::Button);
      //  }
        
        
        ostringstream oss;
        tree.set_model(_liststore);
        for (int i = 0; i <  deck.size(); ++i) {
            oss << i;
            tree.append_column(deck[i].getName(), rowOfPictures[i]);
            //tree.append_column(string("Card " + oss.str()), singleColumn);
            oss.str("");
        }

       
        add(tree);
        show_all(); 
      
       
         
       //  V.pack_start(left);
       //  V.pack_start(right);
       //  show_all_children();

   //    Glib::RefPtr<Gtk::TreeSelection> chosen = tree.get_selection();
   //    //Gtk::TreeModel::iterator it = chosen->get_selected();
   //    chosen->signal_changed().connect(sigc::mem_fun(*this, &testwin::printSelected));
   //  // tree.signal_button_press_event().connect(sigc::mem_fun(*this, &testwin::printSelected));
   //    tree.signal_button_press_event().connect(sigc::mem_fun(*this, &testwin::deleteSelected), false);
    }
    
};
*/



//_vbuttonBox1.foreach(sigc::bind<bool>(sigc::mem_fun(&Gtk::Button::set_sensitive), false));
/*

*/




/*
class TestNotebook
    :public Gtk::Notebook
{
public:
    Gtk::Image blueCityImage;
    Gtk::Image redCityImage;
    Gtk::Image greenCityImage;
    Gtk::Image whiteCityImage;
    Gtk::Window win;
    vector<Deed> deck;


    TestNotebook() {
        

       //blueCityImage. set(RootDirectory + "city_card_blue1.jpg" );
       //redCityImage.  set(RootDirectory + "city_card_red1.jpg"  );
       //greenCityImage.set(RootDirectory + "city_card_green1.jpg");
       //whiteCityImage.set(RootDirectory + "city_card_white1.jpg");
        set_tab_pos(Gtk::POS_RIGHT);
        set_scrollable();
        
        for (int i = 0; i < deck.size(); ++i) {
            Gtk::Image* image_ptr = Gtk::manage(new Gtk::Image);
            Gtk::Label* label_ptr = Gtk::manage(new Gtk::Label);
            string caps = deck[i].getName();
            for (int j = 0; j < caps.size(); ++j)
                caps[j] = toupper(caps[j]);
            label_ptr->set_markup("<span font_family = \"Gazette\" size=\"16384\">" + caps + "</span>");
            image_ptr->set(deck[i].getScaledChildPixbuf());
            append_page(*image_ptr, *label_ptr);
        }
        win.add(*this);
        show_all();

    }


};
*/


int main() {

    srand(unsigned int(time(NULL)));
    cout << "Loading...";
	Game g1;
    Gtk::Main::run(g1);
   
}



