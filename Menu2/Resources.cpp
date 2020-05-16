/***************************************************
* AtmosFear 2.1
* Characters.cpp
*
* Resources, Files and Memory Management
*
*/

#include "Hunt.h"
#include "Targa.h"

#include <iostream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdlib>


void ReadWeapons(FILE*);
void ReadCharacters(FILE*);


/*
! DEPRECATED !
This function is deprecated and set for removal in future versions
! WARNING !
This is a kludge meant to support the original games method of loading resources via the _RES file.

Construct an AreaInfo object and return it with the correct information stored for use by the menu,
this is a backwards compatability function to support the original vanilla _RES and file structure.
*/
AreaInfo MakeOldAreaInfo(int index, int price)
{
	std::stringstream ss;
	AreaInfo a;

	// Set the cost (required score) rank and the dinosaurs available
	a.m_Price = price;
	a.m_Rank = RANK_BEGINNER; // Not used in vanilla
	a.m_DinosAvail.clear(); // Not used currently but empty would mean [all]

	// Load the description
	ss << "huntdat/menu/txt/area" << index << ".txt";
	std::ifstream txt(ss.str());

	if (txt.is_open())
	{
		while (!txt.eof())
		{
			std::string line = "";
			std::getline(txt, line);
			a.m_Description.push_back(line);
		}
		txt.close();
	}
	else
	{
		std::string txt_s = ss.str();
		ss.str(""); ss.clear();
		ss << "MakeOldAreaInfo(" << index << "," << price << ") -> Failed to open the description file: '" << txt_s << "'";
		throw std::runtime_error(ss.str());
		return a;
	}

	// Set the readable name
	if (a.m_Description.size() > 0)
		a.m_Name = a.m_Description[0];
	else
		a.m_Name = "(Unknown)";

	// Set the project name (filename for .MAP and .RSC files)
	ss.str(""); ss.clear();
	ss << "area" << index;
	a.m_ProjectName = ss.str();

	// Load the map thumbnail
	ss.str(""); ss.clear();
	ss << "huntdat/menu/pics/area" << index << ".tga";
	LoadPicture(a.m_Thumbnail, ss.str());

	return a;
}


void ReadWeapons(FILE* stream)
{
	char line[256], * value;

	while (fgets(line, 255, stream))
	{
		if (strstr(line, "}")) break;
		if (strstr(line, "{"))
		{
			WeapInfo wi;

			std::stringstream spp;

			while (fgets(line, 255, stream))
			{
				if (strstr(line, "}")) { break; }
				value = strstr(line, "=");
				if (!value) throw std::runtime_error("Script loading error");
				value++;

				if (strstr(line, "power"))  wi.m_Power = (float)atof(value);
				if (strstr(line, "prec"))   wi.m_Prec = (float)atof(value);
				if (strstr(line, "loud"))   wi.m_Loud = (float)atof(value);
				if (strstr(line, "rate"))   wi.m_Rate = (float)atof(value);
				if (strstr(line, "shots"))  wi.m_Shots = atoi(value);
				if (strstr(line, "reload")) wi.m_Reload = atoi(value);
				if (strstr(line, "trace"))  wi.m_TraceC = atoi(value) - 1;
				if (strstr(line, "optic"))  wi.m_Optic = (float)atof(value);
				if (strstr(line, "fall"))   wi.m_Fall = atoi(value);
				if (strstr(line, "price"))	wi.m_Price = atoi(value);
				if (strstr(line, "rank"))	wi.m_Rank = atoi(value);

				if (strstr(line, "name")) {
					value = strstr(line, "'"); if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value) - 2] = 0;
					wi.m_Name = &value[1];
				}

				if (strstr(line, "file")) {
					value = strstr(line, "'"); if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value) - 2] = 0;
					wi.m_FilePath = &value[1];
				}

				if (strstr(line, "pic")) {
					value = strstr(line, "'"); if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value) - 2] = 0;
					wi.m_BulletFilePath = &value[1];
				}
			}

			spp << "huntdat/menu/pics/weapon" << (g_WeapInfo.size() + 1) << ".tga";
			LoadPicture(wi.m_Thumbnail, spp.str());

			g_WeapInfo.push_back(wi);
		}

	}

}


void ReadCharacters(FILE* stream)
{
	char line[256], * value;
	while (fgets(line, 255, stream))
	{
		if (strstr(line, "}")) break;
		if (strstr(line, "{"))
		{
			DinoInfo di;

			while (fgets(line, 255, stream))
			{
				if (strstr(line, "}"))
				{
					//AI_to_CIndex[DinoInfo[TotalC].AI] = TotalC;
					//TotalC++;
					break;
				}

				value = strstr(line, "=");
				if (!value)
					throw std::runtime_error("Script loading error");
				value++;

				if (strstr(line, "mass")) di.m_Mass = (float)atof(value);
				if (strstr(line, "length")) di.m_Length = (float)atof(value);
				if (strstr(line, "radius")) di.m_Radius = (float)atof(value);
				if (strstr(line, "health")) di.m_BaseHealth = atoi(value);
				if (strstr(line, "basescore")) di.m_BaseScore = atoi(value);
				if (strstr(line, "ai")) di.m_AI = atoi(value);
				if (strstr(line, "smell")) di.m_SmellK = (float)atof(value);
				if (strstr(line, "hear")) di.m_HearK = (float)atof(value);
				if (strstr(line, "look")) di.m_LookK = (float)atof(value);
				// -> Safety Check
				if (strstr(line, "smellk")) di.m_SmellK = (float)atof(value);
				if (strstr(line, "heark")) di.m_HearK = (float)atof(value);
				if (strstr(line, "lookk")) di.m_LookK = (float)atof(value);
				// <- End
				if (strstr(line, "shipdelta")) di.m_ShDelta = (float)atof(value);
				if (strstr(line, "scale0")) di.m_BaseScale = atoi(value);
				if (strstr(line, "scaleA")) di.m_ScaleA = atoi(value);
				if (strstr(line, "danger")) di.m_DangerCall = true;

				if (strstr(line, "name"))
				{
					value = strstr(line, "'");
					if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value) - 2] = 0;
					di.m_Name = &value[1];
				}

				if (strstr(line, "file"))
				{
					/*int inc = atoi(line);
					char error[256];
					sprintf(error, "The file value is integer %d", inc);
					MessageBox(hwndMain, error, "Integer", MB_OK);*/
					value = strstr(line, "'"); if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value) - 2] = 0;
					di.m_FilePath = &value[1];
				}

				if (strstr(line, "pic"))
				{
					value = strstr(line, "'");
					if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value) - 2] = 0;
					di.m_PicturePath = &value[1];
				}
			}

			if (di.m_AI >= 10)
			{
				std::stringstream spp;
				spp << "huntdat/menu/pics/dino" << (di.m_AI - 9) << ".tga";
				LoadPicture(di.m_Thumbnail, spp.str());
			}

			g_DinoInfo.push_back(di);
		}

	}
}


void ReadAreas(FILE* stream)
{
	char line[256], * value;
	while (fgets(line, 255, stream))
	{
		if (strstr(line, "}")) break;
		if (strstr(line, "{"))
		{
			AreaInfo area;

			while (fgets(line, 255, stream))
			{
				if (strstr(line, "}"))
				{
					break;
				}

				value = strstr(line, "=");
				if (!value)
					throw std::runtime_error("Script loading error");
				value++;

				if (strstr(line, "price")) area.m_Price = atoi(value);
				if (strstr(line, "rank"))  area.m_Rank = atoi(value);

				if (strstr(line, "name"))
				{
					value = strstr(line, "'");
					if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value) - 2] = 0;
					area.m_Name = &value[1];
				}

				if (strstr(line, "pname"))
				{
					value = strstr(line, "'"); if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value) - 2] = 0;
					area.m_ProjectName = &value[1];
				}

				if (strstr(line, "thumbnail"))
				{
					value = strstr(line, "'");
					if (!value) throw std::runtime_error("Script loading error");
					value[strlen(value) - 2] = 0;
					///TODO: Load TPicture
					//strcpy(area.Thumbnail, &value[1]);
				}
			}

			g_AreaInfo.push_back(area);
		}

	}
}


void ReadPrices(FILE* stream)
{
	uint32_t CurA = 0;
	uint32_t CurW = 0;
	uint32_t CurD = 0;
	uint32_t CurU = 0;
	char line[256], * value;
	int dummy = 0;

	// Initialise the `CurD` variable to the first huntable index
	for (uint32_t i = 0; i < g_DinoInfo.size(); i++)
	{
		if (g_DinoInfo.at(i).m_AI == 10)
		{
			CurD = i;
			break;
		}
	}

	while (fgets(line, 255, stream))
	{
		std::string line_str = line;
		if (line_str.empty()) { continue; }
		if (line_str.compare("\n") == 0) { continue; }
		if (line_str.compare("\r\n") == 0) { continue; }

		if (strstr(line, "}")) { break; }
		value = strstr(line, "=");
		if (!value) throw std::runtime_error("Script loading error: Expected assignment '='");
		value++;

		if (strstr(line, "start"))  g_StartCredits = (int)atoi(value);
		if (strstr(line, "area")) {
			g_AreaInfo.push_back(MakeOldAreaInfo(CurA + 1, (int)atoi(value)));
			CurA++;
		}
		if (strstr(line, "dino")) {
			g_DinoInfo[CurD].m_Price = (int)atoi(value);
			CurD++;
		}
		if (strstr(line, "weapon")) {
			g_WeapInfo[CurW].m_Price = (int)atoi(value);
			CurW++;
		}

		if (strstr(line, "acces")) {
			dummy = atoi(value);// We don't use this right now
			CurU++;
		}
	}
}


void LoadResourcesScript()
{
	FILE* file;
	char line[256];

	// Initialise some things
	g_StartCredits = 100; // Default

	file = fopen("huntdat/_res.txt", "r");
	if (!file) {
		throw std::runtime_error("Can't open resources file _res.txt");
		return;
	}

	while (fgets(line, 255, file))
	{
		if (line[0] == '.') break;			//endoffile EOF
		if (line[0] == '#') continue;	//comment
		if (line[0] == ';') continue;	//comment
		//if (strstr(line, "version") ) continue; //TODO: read version
		if (strstr(line, "weapons")) ReadWeapons(file);
		if (strstr(line, "characters")) ReadCharacters(file);
		//if (strstr(line, "areas") ) ReadAreas(file); //TODO: Add areas section to _RES
		if (strstr(line, "prices")) ReadPrices(file);
	}

	fclose(file);
}


void LoadResources()
{
	//TODO use this and ignore old _RES stuff

	// TODO: enable these and use them instead of GDI drawing for the trackbars
	//LoadPicture(g_TrackBar[0], "huntdat/menu/sl_bar.tga");
	//LoadPicture(g_TrackBar[1], "huntdat/menu/sl_but.tga");

	UtilInfo ui;
	ui.m_Name = "Camoflauge";
	ui.m_Description = LoadText("huntdat/menu/txt/camoflag.nfo");
	ui.m_Command = "";
	LoadPicture(ui.m_Thumbnail, "huntdat/menu/pics/equip1.tga");
	g_UtilInfo.push_back(ui);

	ui.m_Name = "GPS Radar";
	ui.m_Description = LoadText("huntdat/menu/txt/radar.nfo");
	ui.m_Command = "-radar";
	LoadPicture(ui.m_Thumbnail, "huntdat/menu/pics/equip2.tga");
	g_UtilInfo.push_back(ui);

	ui.m_Name = "Cover Scent";
	ui.m_Description = LoadText("huntdat/menu/txt/scent.nfo");
	ui.m_Command = "";
	LoadPicture(ui.m_Thumbnail, "huntdat/menu/pics/equip3.tga");
	g_UtilInfo.push_back(ui);

	ui.m_Name = "Double Ammo";
	ui.m_Description = LoadText("huntdat/menu/txt/double.nfo");
	ui.m_Command = "";
	LoadPicture(ui.m_Thumbnail, "huntdat/menu/pics/equip4.tga");
	g_UtilInfo.push_back(ui);

#ifdef _iceage
	ui.m_Name = "Ammo Resupply";
	ui.m_Description = LoadText("huntdat/menu/txt/supply.nfo");
	ui.m_Command = "";
	LoadPicture(ui.m_Thumbnail, "huntdat/menu/pics/equip5.tga");
	g_UtilInfo.push_back(ui);
#endif //_iceage

	LoadWave(g_MenuSound_Go, "huntdat/soundfx/menugo.wav");
	LoadWave(g_MenuSound_Ambient, "huntdat/soundfx/menuamb.wav");
	LoadWave(g_MenuSound_Move, "huntdat/soundfx/menumov.wav");
}


void ReleaseResources()
{
	g_WeapInfo.clear();
	g_DinoInfo.clear();
	g_AreaInfo.clear();
}

void TrophyLoad(Profile& profile, int pr)
{
	//PlayerProfile.RegNumber
	memset(&profile, 0, sizeof(Profile));
	profile.RegNumber = pr;

	std::stringstream fname;
	fname << "trophy" << std::setfill('0') << std::setw(2) << profile.RegNumber << ".sav";

	std::ifstream fs(fname.str(), std::ios::binary);

	if (!fs.is_open()) {
		std::stringstream ss;
		ss << "Unable to find the save file: " << fname.str();
		throw std::runtime_error(ss.str());
		return;
	}

	// Check version
	fs.seekg(0, std::ios::end);
	auto file_size = fs.tellg();

#ifdef _iceage
	if (file_size != 1664)
	{
		ShowErrorMessage("Not a compatible Carnivores: Ice Age save file!");
		fs.close();
		return;
	}
#else
	if (file_size != 1660)
	{
		ShowErrorMessage("Not a compatible Carnivores 2 save file!");
		fs.close();
		return;
	}
#endif

	fs.seekg(0, std::ios::beg);

	fs.read((char*)&profile, sizeof(Profile));

	fs.read((char*)&g_Options.Aggression, 4);
	fs.read((char*)&g_Options.Density, 4);
	fs.read((char*)&g_Options.Sensitivity, 4);

	fs.read((char*)&g_Options.Resolution, 4);
	fs.read((char*)&g_Options.Fog, 4);
	fs.read((char*)&g_Options.Textures, 4);
	fs.read((char*)&g_Options.ViewRange, 4);
	fs.read((char*)&g_Options.Shadows, 4);
	fs.read((char*)&g_Options.MouseSensitivity, 4);
	fs.read((char*)&g_Options.Brightness, 4);

	fs.read((char*)&g_Options.KeyMap, sizeof(TKeyMap));
	fs.read((char*)&g_Options.MouseInvert, 4);

	fs.read((char*)&g_Options.ScentMode, 4);
	fs.read((char*)&g_Options.CamoMode, 4);
	fs.read((char*)&g_Options.RadarMode, 4);
	fs.read((char*)&g_Options.TranqMode, 4);
	fs.read((char*)&g_Options.AlphaColorKey, 4);

	fs.read((char*)&g_Options.OptSys, 4);
	fs.read((char*)&g_Options.SoundAPI, 4);
	fs.read((char*)&g_Options.RenderAPI, 4);

	// Append any data you want, the original games do not check the file size and stop reading at this point

	std::cout << "Profile Loaded." << std::endl;
}


void TrophySave(Profile& profile)
{
	std::stringstream fname;
	fname << "trophy" << std::setfill('0') << std::setw(2) << profile.RegNumber << ".sav";

	std::ofstream fs(fname.str(), std::ios::binary | std::ios::trunc);

	if (!fs.is_open()) {
		std::cout << "Profile: Error saving trophy!" << std::endl;
		return;
	}

	fs.write((char*)&profile, sizeof(Profile));

	fs.write((char*)&g_Options.Aggression, 4);
	fs.write((char*)&g_Options.Density, 4);
	fs.write((char*)&g_Options.Sensitivity, 4);
	fs.write((char*)&g_Options.Resolution, 4);
	fs.write((char*)&g_Options.Fog, 4);
	fs.write((char*)&g_Options.Textures, 4);
	fs.write((char*)&g_Options.ViewRange, 4);
	fs.write((char*)&g_Options.Shadows, 4);
	fs.write((char*)&g_Options.MouseSensitivity, 4);
	fs.write((char*)&g_Options.Brightness, 4);
	fs.write((char*)&g_Options.KeyMap, sizeof(TKeyMap));
	fs.write((char*)&g_Options.MouseInvert, 4);
	fs.write((char*)&g_Options.ScentMode, 4);
	fs.write((char*)&g_Options.CamoMode, 4);
	fs.write((char*)&g_Options.RadarMode, 4);
	fs.write((char*)&g_Options.TranqMode, 4);
	fs.write((char*)&g_Options.AlphaColorKey, 4);
	fs.write((char*)&g_Options.OptSys, 4);
	fs.write((char*)&g_Options.SoundAPI, 4);
	fs.write((char*)&g_Options.RenderAPI, 4);

	/*
	You can append any data you want to once you remove the check for file size I added,
	in the TrophyLoad(...) function.
	The original games do not check the file size and stop reading at this point
	*/

	std::cout << "Profile Saved." << std::endl;
}


bool ReadTGAFile(const std::string& path, TargaImage& tga)
{
	std::ifstream fs(path, std::ios::binary);

	if (!fs.is_open()) {
		std::cout << "Failed to open TGA file: " << path << std::endl;
		return false;
	}

	fs.read((char*)&tga.m_Header, sizeof(TARGAINFOHEADER));

	if (tga.m_Header.tgaColorMapType) {
		std::cout << "Has a color palette: " << path << std::endl;
		return false;
	}

	if (tga.m_Header.tgaImageType != TGA_IMAGETYPE_RGB) {
		std::cout << "Not an RGB image: " << path << std::endl;
		return false;
	}

	if (tga.m_Header.tgaIdentSize) {
		fs.seekg(tga.m_Header.tgaIdentSize, std::ios::cur); // Skip Ident header
	}

	if (tga.m_Data)
		delete[] tga.m_Data;

	int size = (tga.m_Header.tgaWidth * (tga.m_Header.tgaBits / 8)) * tga.m_Header.tgaHeight;
	tga.m_Data = new uint8_t[size];
	fs.read((char*)tga.m_Data, size);

	return true;
}


void LoadPicture(Picture& pic, const std::string& fpath)
{
	TargaImage tga;
	if (ReadTGAFile(fpath, tga)) {

		pic.m_Width = tga.m_Header.tgaWidth;
		pic.m_Height = tga.m_Header.tgaHeight;

		if (pic.m_Data)
			delete[] pic.m_Data;

		pic.m_Data = new uint16_t[pic.m_Width * pic.m_Height];
		memcpy(pic.m_Data, tga.m_Data, pic.m_Width * pic.m_Height * sizeof(uint16_t));
	}
}


std::vector<std::string> LoadText(const std::string& path)
{
	std::vector<std::string> txt;
	std::ifstream tf(path);

	if (tf.is_open())
	{
		while (!tf.eof())
		{
			std::string line = "";
			std::getline(tf, line);
			txt.push_back(line);
		}
		tf.close();
	}
	else
	{
		std::cout << "LoadText() : Unable to open text file for reading: '" << path << "'" << std::endl;
	}

	return txt;
}


bool LoadWave(SoundFX& sfx, const std::string& path)
{
	std::ifstream tf(path, std::ios::binary);

	if (tf.is_open())
	{
		tf.seekg(36);

		char c[5]; c[4] = 0;

		while (true)
		{
			c[0] = tf.get();
			if (c[0] == 'd')
			{
				tf.read(&c[1], 3);
				if (!std::string(c).compare("data")) break;
				else tf.seekg(-3, std::ios::cur);
			}
		}
		
		tf.read((char*)&sfx.m_Length, 4);

		if (sfx.m_Data)
			delete[] sfx.m_Data;

		sfx.m_Data = new int16_t[sfx.m_Length / sizeof(int16_t)];
		tf.read((char*)sfx.m_Data, sfx.m_Length);

		tf.close();
		return true;
	}
	else
	{
		std::cout << "LoadWave() : Unable to open .wav file for reading: '" << path << "'" << std::endl;
	}

	return false;
}


void TrophyDelete(uint32_t trophy_index)
{
	std::stringstream fname;
	fname << "trophy" << std::setfill('0') << std::setw(2) << trophy_index << ".sav";
	std::filesystem::path fp = std::filesystem::current_path();
	fp /= std::filesystem::path(fname.str());
	std::cout << "Deleting trophy: \'" << fp << "\'" << std::endl;
	std::filesystem::remove(fp);
}


void Profile::New(const std::string& name, int32_t index)
{
	if (index == -1)
		index = g_ProfileIndex;

	memset(Name, 0, 128);
	memcpy(Name, name.data(), name.length());
	RegNumber = index;
	Score = g_StartCredits;
	Rank = RANK_BEGINNER;
	memset(&Last, 0, sizeof(ProfileStats));
	memset(&Total, 0, sizeof(ProfileStats));
	memset(Body, 0, sizeof(TrophyItem) * 24);
}


void Options::Default()
{
	this->Aggression = 128;
	this->Density = 128;
	this->Sensitivity = 128;
	this->Resolution = 5;
	this->Fog = true;
	this->Textures = 1;
	this->ViewRange = 128;
	this->Shadows = true;
	this->MouseSensitivity = 128;
	this->Brightness = 128;
	// -- Set default controls
	this->KeyMap.fkForward = 'W';
	this->KeyMap.fkBackward = 'S';
	this->KeyMap.fkSLeft = 'A';
	this->KeyMap.fkSRight = 'D';
	this->KeyMap.fkFire = VK_LBUTTON;
	this->KeyMap.fkShow = VK_RBUTTON;
	this->KeyMap.fkJump = VK_SPACE;
	this->KeyMap.fkCall = VK_MENU;
	this->KeyMap.fkBinoc = 'B';
	this->KeyMap.fkCrouch = 'C';
	this->KeyMap.fkRun = VK_LSHIFT;
	this->KeyMap.fkUp = VK_UP;
	this->KeyMap.fkDown = VK_DOWN;
	this->KeyMap.fkLeft = VK_LEFT;
	this->KeyMap.fkRight = VK_RIGHT;
#ifdef _iceage
	this->KeyMap.fkSupply = 'O';
#endif //_iceage
	this->MouseInvert = false;
	this->ScentMode = false;
	this->CamoMode = false;
	this->RadarMode = false;
	this->TranqMode = false;
	this->AlphaColorKey = 1;
	this->OptSys = 1;
	this->SoundAPI = 0; // Default to software
	this->RenderAPI = 0; // Default to software
}


SoundFX::SoundFX() :
	m_Data(nullptr),
	m_Length(0U),
	m_Frequency(22050U)
{
}


SoundFX::SoundFX(const SoundFX& w)
{
	if (w.m_Length && w.m_Data) {
		m_Data = new int16_t[w.m_Length];
		memcpy(m_Data, w.m_Data, w.m_Length);
	}

	m_Length = w.m_Length;
	m_Frequency = w.m_Frequency;
}


SoundFX::~SoundFX()
{
	if (m_Data)
		delete[] m_Data;
}


Picture::Picture() :
	m_Width(0),
	m_Height(0),
	m_Data(nullptr)
{}


Picture::Picture(const Picture& p)
{
	m_Data = nullptr;

	if (p.m_Width && p.m_Height && p.m_Data) {
		m_Data = new uint16_t[p.m_Width * p.m_Height];
		memcpy(m_Data, p.m_Data, (p.m_Width * 2) * p.m_Height);
	}

	m_Width = p.m_Width;
	m_Height = p.m_Height;
}


Picture::~Picture()
{
	if (m_Data)
		delete[] m_Data;
}